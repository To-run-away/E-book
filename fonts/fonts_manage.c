#include <stdio.h>
#include <config.h>
#include <string.h>
#include <fonts_manage.h>




static PT_FontOperate g_ptFontOperateHead;



/*
 * 注册一个字体文件
 */
int RegisterFontOperate(PT_FontOperate ptFontOparate)
{
	PT_FontOperate tmp_ptFontOperate;

	/*
	 * 输入参数有误
	 */
	if( !ptFontOparate ) {
		DBG_PRINTF("RegisterFontOperate value is NULL\n");
		return -1;
	}
	
	
	if( !g_ptFontOperateHead ){
		/*
		 * 第一个注册的字体
		 */
		g_ptFontOperateHead = ptFontOparate;
		ptFontOparate->ptNext = NULL;	
	} else {
		tmp_ptFontOperate = g_ptFontOperateHead;

		/*
		 * 新注册的字体注册在已经注册的字体后面
 		 */
		while( tmp_ptFontOperate->ptNext ) 
			tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
		
		tmp_ptFontOperate->ptNext = ptFontOparate;
		ptFontOparate->ptNext = NULL;
	}

	return 0;
}


/*
 * 展示所支持的字体
 */
void ShowFontOperate(void)
{
	int i = 0;
	PT_FontOperate tmp_ptFontOperate = g_ptFontOperateHead;

	while( tmp_ptFontOperate ) {
		printf("%02d %s\n",i++, tmp_ptFontOperate->name);
		tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
	}
}

PT_FontOperate GetFontOperate(char *name)
{
	PT_FontOperate tmp_ptFontOperate = g_ptFontOperateHead;
	
	while( tmp_ptFontOperate ) {
		if( !strcmp(tmp_ptFontOperate->name, name) ) 
			return tmp_ptFontOperate;
		tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
	}
	
	return NULL;
}

int FontsInit(void)
{
	int err;
	
	err = AsciiInit();
	if( err ) {
		DBG_PRINTF("AsciiFontInit err\n");
		return -1;
	}
	
	err = FreeTypeInit();
	if( err ) {
		DBG_PRINTF("FreeTypeInit err\n");
		return -1;
	}
	
	err = GBKInit();
	if( err ) {
		DBG_PRINTF("GBKInit err\n");
		return -1;
	}
	
	
	return 0;
}



