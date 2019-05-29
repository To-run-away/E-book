
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <encoding_manage.h>
#include <fonts_manage.h>
#include <display_manage.h>
#include <string.h>
#include <config.h>

typedef struct _PageDesc {
	int iPage;
	unsigned char *pucLcdFirstPos;
	unsigned char *pucLcdNextPageFirstPos;
	struct _PageDesc *ptPrevPage;
	struct _PageDesc *ptNextPage;
} T_PageDesc,* PT_PageDesc;

static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFileMemEnd;

/*
 * 选择可以解码这个文件的解码器
 */
static PT_EncodingOperate g_ptEnCodingOperateForFile;

/*
 * 选择的显示器
 */
static PT_DisplayOperate g_ptDisplayOperate;

/*
 * lcd上显示的第一个字体在文件的位置
 */
static unsigned char *g_pucLcdFirstPosAtFile;
static unsigned char *g_pucLcdNextPosAtFile;

/*
 * 字体的大小(bit)
 */
static int g_FontSize;

/*
 * 链接所有页,表示当前页
 */
static PT_PageDesc g_ptPages;
static PT_PageDesc g_ptCurPage;



/*
 * 打开要显示的文本
 * 做好映射,同时选择好编码器
 */

int OpenTextFile(char *FileName)
{
	struct stat tmp_stat;
	int Text_fd;

	/*
 	 * 获取文件描述符
  	 */
	Text_fd = open(FileName, O_RDONLY);
	if( Text_fd < 0 )  {
		DBG_PRINTF("con't open text file %s\n", FileName);
		return -1;
	}
	
	/* 
	 * 获取文件状态信息
	 */
 	if( fstat(Text_fd, &tmp_stat)) {
		DBG_PRINTF("con't get fstat");
		return -1;
	}

	g_pucTextFileMem = mmap(NULL, tmp_stat.st_size, PROT_READ, MAP_SHARED, Text_fd, 0);	
	if((unsigned char *)-1 == g_pucTextFileMem) {
		DBG_PRINTF("con't mmap  for text %s\n", FileName);
		return -1;
	}
	g_pucTextFileMemEnd = g_pucTextFileMem + tmp_stat.st_size;

	/*
 	 * 获取可以解析这个文件编码的解码器
 	 */
	g_ptEnCodingOperateForFile = SelectEncodingOperate((const char *) g_pucTextFileMem);

	/*
	 * 记录第一个有效字体编码的位置
	 */
	if( g_ptEnCodingOperateForFile ) {
		g_pucLcdFirstPosAtFile = g_ptEnCodingOperateForFile->iHeadLen + g_pucTextFileMem;
		return 0;
	}
	else  {
		return -1;
	}
}

/*
 * 设置字体细节
 */
int SetTextDetail(char *pcHzkFile,char *pcFileFreeType, unsigned int FontSize)
{
	int err = 0;
	PT_FontOperate tmp_ptFontOperate;
	PT_FontOperate ptFontOpr;
	int ret = -1;

	g_FontSize = FontSize;
	/*
	 * 获取支持这种编码的字体链表
 	 */
	ptFontOpr = g_ptEnCodingOperateForFile->ptFontOprSupportHead;
	
	while( ptFontOpr ) {

		/*
		 * 检查是那种字体文件
		 */
		if( !strcmp( ptFontOpr->name, "ascii") ) { 
			/* ascii字符的初始化 */
			err = ptFontOpr->FontInit(NULL, FontSize);
		}
		else if( !strcmp( ptFontOpr->name, "gbk") ) {
			/* gbk字符的初始化 */
			err = ptFontOpr->FontInit(pcHzkFile, FontSize);
		}
		else {
			/* freetype字体的初始化 */
			err = ptFontOpr->FontInit(pcFileFreeType, FontSize);
		}
	
		DBG_PRINTF("FontName:%s ,err: %d\n", ptFontOpr->name, err);

		/*
		 * 删除不支持这种字体的,只要知道支持的就退出
 		 */
		tmp_ptFontOperate = ptFontOpr->ptNext;
		if( !err ) {
			/*
		 	* 对于ansi编码的文件,可能用ascii编码也可可能用gbk编码，
		 	* 所以只要有一个FontInit成功,这个函数就返回成功
		 	*/
			ret = 0;
		}
		else {
			DelFontOperateFromEncoding(g_ptEnCodingOperateForFile, tmp_ptFontOperate);			
		}
		ptFontOpr = tmp_ptFontOperate;
	}

	return ret;	
}


/*
 * 选择和初始化显示器
 */
int SelectAndInitDisplay(char *Name)
{
	int err = 0;

	g_ptDisplayOperate = GetDisplayOperate(Name);
	if( !g_ptDisplayOperate ) {
		DBG_PRINTF("con't find Display %s\n",Name);
		return -1;
	}
	err = g_ptDisplayOperate->DeviceInit(); 
	
	return err;	
}

/*
 * x坐标增加1
 */
static inline int IncLcdX(int x)
{
	if( (x + 1) < g_ptDisplayOperate->iXres )
		return (x + 1);
	else
		return 0;
}

/*
 * y坐标增加一个字体的大小(高度)
 */
static inline int IncLcdY(int y)
{
	if( (y + g_FontSize) < g_ptDisplayOperate->iYres )
		return (y + g_FontSize);
	else
		return 0;
}


static int ShowOneFont(PT_FontBitmap ptFontBitmap)
{
	int x , y;
	unsigned int byte = 0;
	int i , bit;

	if( 1 == ptFontBitmap->iBpp ) {
		
		/*
		 * y表示需要显示的行数
 		 */
 		for(y = ptFontBitmap->iYtop; y < ptFontBitmap->iYmax; y++) {
			/*
			 * 取出一行字体图像在buf中的偏移首部
 			 */
			i = (y - ptFontBitmap->iYtop) * ptFontBitmap->iPitch;

			/*
 			 * 显示一行点阵
 			 */
			for(x = ptFontBitmap->iXletf, bit = 7; x < ptFontBitmap->iXmax; x++) {

				/* 取出下一个字符 */
				if(7 == bit ) {
					byte = ptFontBitmap->pucBuffer[i++];
				}
				if(byte & (1 << bit)) {
					/* 显示一个点 */
					g_ptDisplayOperate->ShowPixel(x, y, COLOR_FOREGROUND);
				}
				else {
					;	/* 显示背景 */
				}
				
				bit --;
				if(-1 == bit)
					bit = 7;
			}
		
		}
	}
	else if(8 == ptFontBitmap->iBpp) {
		i = 0;
		for(y = ptFontBitmap->iYtop; y < ptFontBitmap->iYmax; y++) {
			for(x = ptFontBitmap->iXletf; x < ptFontBitmap->iXmax; x++) {
				if(ptFontBitmap->pucBuffer[i++])
					g_ptDisplayOperate->ShowPixel(x , y ,COLOR_FOREGROUND);
				else
					;//g_ptDisplayOperate->ShowPixel(x, y , COLOR_BACKGROUND);
			}
		}
	}
	else {
		DBG_PRINTF("Show one Font error, con't support %d bpp\n",ptFontBitmap->iBpp);
		return -1;
	}

	return 0;
}

/*
 * 重新计算字体位置
 */
int RelocateFontPos(PT_FontBitmap ptFontBitmap)
{
	int LcdY;
	int DelteX;
	int DelteY;

	if(ptFontBitmap->iYmax > g_ptDisplayOperate->iYres)  {
		
		/* 满页了 */
		return -1;
	}

	/* 超过lcd最右侧来 */
	if(ptFontBitmap->iXmax > g_ptDisplayOperate->iXres ) {

		/* 进行换行操作 */
		LcdY = IncLcdY(ptFontBitmap->iCurOriginY);
		if( !LcdY ) {
			/* 满页了 */
			return -1;
		}
		else {
			/* 调到下一页继续处理 */
			DelteX = 0 - ptFontBitmap->iCurOriginX;
			DelteY = LcdY - ptFontBitmap->iCurOriginY;
	
			/*
 			 * 下一行的起始位置
 			 */		
			ptFontBitmap->iCurOriginX = 0;
			ptFontBitmap->iCurOriginY = LcdY;
	
			ptFontBitmap->iNextOriginX += DelteX;
			ptFontBitmap->iNextOriginY += DelteY;

			ptFontBitmap->iXletf += DelteX;
			ptFontBitmap->iXmax  += DelteX;

			ptFontBitmap->iYtop  += DelteY;
			ptFontBitmap->iYmax  += DelteY;

			return 0;
		}
	}
	else {
		;/* 不用换行,也不用换页 */
	}

	return 0;
}




int ShowOnePage(unsigned char *pucTextFileCurPos)
{
	int len;
	int err;

	unsigned int code;
	PT_FontOperate tmp_ptFontOperate;
	T_FontBitmap tmp_tFontBitmap;

	int has_get_code = 0;
	int has_not_clr_sceen = 1;
	unsigned char *buf_start = pucTextFileCurPos;

	tmp_tFontBitmap.iCurOriginX = 0;
	tmp_tFontBitmap.iCurOriginY = g_FontSize;
	
		
	while(1) {
		len = g_ptEnCodingOperateForFile->GetCodeFromBuf(buf_start, g_pucTextFileMemEnd, &code);		
		if( !len ) {
			/* 文件结束 */
			if( !has_get_code )
				return -1;
			else 
				return 0;
		}
		/* 确保已经获取到数据 */
		has_get_code = 1;
		
		/* 下一个字符的起始地址 */
		buf_start += len;
		

		/* 处理文本文件中的换行 */
		if('\n' == code) {
			g_pucLcdNextPosAtFile = buf_start;
					
			tmp_tFontBitmap.iCurOriginX = 0;
			tmp_tFontBitmap.iCurOriginY = IncLcdY(tmp_tFontBitmap.iCurOriginY);
			if(0 == tmp_tFontBitmap.iCurOriginY ) {
				/* 当前屏幕已经显示完了 */
				return 0;
			}
			else {
				continue;
			}
		}
		else if('\r' == code) {
			continue;
		}
 		else if('\t' == code) {
			/* tab键使用一个空格代替 */
			code = ' ';
		}
	//	DBG_PRINTF("code = 0x%x\n", code);	

		tmp_ptFontOperate = g_ptEnCodingOperateForFile->ptFontOprSupportHead;
		
		while( tmp_ptFontOperate ) {
			
			err = tmp_ptFontOperate->GetFontBitmap(code, &tmp_tFontBitmap);
		//	DBG_PRINTF("%s %s %d,tmp_ptFontOperate->name = %s, err = %d\n",__FILE__, __FUNCTION__, __LINE__, tmp_ptFontOperate->name, err);
			
			if( !err ) {
				
				if( RelocateFontPos(&tmp_tFontBitmap) ) {

					/* 本页已满不能继续显示这个字符了 */
					return 0;
				}
				
				if( has_not_clr_sceen ) {
					/*
					 * 刚进入这个函数需要清屏
					 */
					g_ptDisplayOperate->ClearScreen(COLOR_BACKGROUND);
					has_not_clr_sceen = 0;
				}
				
				if( ShowOneFont(&tmp_tFontBitmap) )
					return -1;
				
				tmp_tFontBitmap.iCurOriginX = tmp_tFontBitmap.iNextOriginX;
				tmp_tFontBitmap.iCurOriginY = tmp_tFontBitmap.iNextOriginY;
				g_pucLcdNextPosAtFile = buf_start;
		
				/* 继续取出下一个编码显示 */
				break;
			}
			/*
			 * tmp_ptFontBitmap不能获取字字模,
			 * 采用下一个支持该字体的编码来获取字模
			 * 
			 */
			tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
		}

	}
	
	return 0;
}


/*
 * 记录已经阅读的页到链表中
 */
static void RecordPage(PT_PageDesc ptPageNew)
{
	PT_PageDesc ptPage;

	if( !g_ptPages ) {
		g_ptPages = ptPageNew;
	}
	else {

		/*
		 * 新页串到链表末尾
 		 */
		ptPage = g_ptPages;
		while( ptPage->ptNextPage )
			ptPage = ptPage->ptNextPage;
		ptPage->ptNextPage 		= ptPageNew;
		ptPageNew->ptPrevPage 	= ptPage;
	}
}



/*
 * 显示下一页 
 */
int ShowNextPage(void)
{
	int err;
	PT_PageDesc ptPage;
	unsigned char *pucTextFileCurPos;

	if( g_ptCurPage ) {
		/* 显示下一页 */
		pucTextFileCurPos = g_ptCurPage->pucLcdNextPageFirstPos;
	}
	else  {
		/* 显示第一页 */
		pucTextFileCurPos = g_pucLcdFirstPosAtFile;
	}

	err = ShowOnePage(pucTextFileCurPos);
	
	if( !err ) {

		/*
		 * 显示的下一页已经记录好了
		 */
		if(g_ptCurPage && g_ptCurPage->ptNextPage) {
			g_ptCurPage = g_ptCurPage->ptNextPage;
			return 0;
		}
		
		/*
		 * 显示的是新页,则申请空间记录好新页位置
		 */
		ptPage = malloc(sizeof(T_PageDesc));
		if(ptPage) {
			/* 当前页的第一个字符位置 */
			ptPage->pucLcdFirstPos 			= pucTextFileCurPos;
			/* 下一页(新页)的要显示的第一个字符存的地址 */
			ptPage->pucLcdNextPageFirstPos  = g_pucLcdNextPosAtFile;
			ptPage->ptPrevPage = NULL;
			ptPage->ptNextPage = NULL;
			g_ptCurPage = ptPage;
			RecordPage(ptPage);
			return 0;
		}
		else {
			DBG_PRINTF("no more memory can use\n");
			return -1;
		}	
	}
	return err;
}

/*
 * 显示前一页
 */
int ShowPrevPage(void)
{
	int err;

	if( (!g_ptCurPage) || (!g_ptCurPage->ptPrevPage) ) {
		DBG_PRINTF("prev page inexistence\n");
		return -1;
	}

	err = ShowOnePage(g_ptCurPage->ptPrevPage->pucLcdFirstPos);
	if( !err ) {
		DBG_PRINTF("ShowPrevPage ShowOnePage error\n");
		g_ptCurPage = g_ptCurPage->ptPrevPage;
	}
	
	return err;
}



