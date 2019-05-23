
#include <fonts_manage.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>


static int GBKFontInit(char *FontFile,unsigned int FontSize);
static int GBKGetFontBitmap(unsigned int FontCode, PT_FontBitmap ptFontBitmap);


/*
 * 分配,设置,注册一个T_FontOperate结构体
 */
static T_FontOperate g_GBKFontOperate = {
	.name 			= "gbk",
	.FontInit 		= GBKFontInit,
	.GetFontBitmap  = GBKGetFontBitmap,
};


/*
 * 字体文件的文件描述符
 */
static int g_fd_hzk;

/*
 * 字体文件在内存中的起始和结束位置
 */
static unsigned char *g_hzk_start_mem;
static unsigned char *g_hzk_end_mem;


/*
 * gbk 字体的初始化函数
 */
static int GBKFontInit(char *FontFile,unsigned int FontSize)
{
	struct stat tmp_stat;

	if (16 != FontSize)
	{
		printf("GBK can't support %d fontsize\n", FontSize);
		return -1;
	}

	g_fd_hzk = open(FontFile, O_RDONLY);
	if(g_fd_hzk < 0) {
		printf("open %s file",FontFile);
		return -1;
	}

	if(fstat(g_fd_hzk, &tmp_stat)) {
		perror("get fstat err");
		printf("get fstat err,%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);
		return -1;
	}

	g_hzk_start_mem = mmap(NULL, tmp_stat.st_size, PROT_READ, MAP_SHARED, g_fd_hzk, 0);
	if( g_hzk_start_mem ) {
		perror("hzk mmap err\n");
		return -1;
	}
	g_hzk_end_mem = g_hzk_start_mem + tmp_stat.st_size;
	
	return 0;
}

static int GBKGetFontBitmap(unsigned int FontCode, PT_FontBitmap ptFontBitmap)
{

	int AreaCode;
	int WhereCode;

	/* 获取原点坐标 */
	int iPenX = ptFontBitmap->iCurOriginX;
	int iPenY = ptFontBitmap->iCurOriginY;

	/*
	 * 每一个字符都有对应一个编号。其中前两位为“区”，后两位为“位”，中文汉字的编号区号
	 * 是从16开始的，位号从1开始。前面的区号有一些符号、数字、字母、注音符号（台）、
	 * 制表符、日文等等。
	 * 而GB2312编码就是基于区位码的，用双字节编码表示中文和中文符号。
	 * 一般编码方式是：0xA0+区号，0xA0+位号。
	 * GB 2312 非汉字符号区。即 GBK/1: A1A1-A9FE
	 * 如 “安”字，区位号是1618（十进制），那么“安”字的GB2312编码就是 
	 * 0xA0+16  0xA0+18 也就是 0xB0 0xB2 。根据区位码表，GB2312的汉字编码范围是0xB0A1~0xF7FE
 	 */
	
	if(0xffff0000 & FontCode ) {
		printf("don't support this code: %d, %s %s %d\n", FontCode, __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	AreaCode  = (FontCode & 0xff) - 0xa1;
	WhereCode = ((FontCode >> 8) & 0xff) - 0xa1;;
	
	if( (AreaCode < 0) || (WhereCode < 0) ) {
		printf("gbk don't support the code %s %s %d", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	ptFontBitmap->iXletf = iPenX;
	ptFontBitmap->iYtop  = iPenY - 16;
	ptFontBitmap->iXmax  = iPenX + 16;
	ptFontBitmap->iYmax  = iPenY;
	ptFontBitmap->iBpp   = 1;
	ptFontBitmap->iPitch = 2;

	/* value << 5 ==  value * 32  16*16的汉字,占用32个字节  */
	ptFontBitmap->pucBuffer = &g_hzk_start_mem[ (AreaCode * (0xff - 0xA1) + WhereCode) << 5 ];
	
	if( g_hzk_end_mem < ptFontBitmap->pucBuffer ) {
		printf("Thsi code value too big %d \n", FontCode);
		return -1;
	}

	return 0;
}


int GBKInit(void)
{
	return RegisterFontOperate(&g_GBKFontOperate);
}





