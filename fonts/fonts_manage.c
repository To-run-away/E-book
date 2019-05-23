#include <stdio.h>

#include <fonts_manage.h>




static PT_FontOperate g_ptFontOperate;



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
		printf("RegisterFontOperate value is NULL\n");
		return -1;
	}
	

	
	if( !g_ptFontOperate ){
		/*
		 * 第一个注册的字体
		 */
		g_ptFontOperate = ptFontOparate;
		ptFontOparate->ptNext = NULL;	
	} else {
		tmp_ptFontOperate = g_ptFontOperate;

		/*
		 * 新注册的字体注册在已经注册的字体后面
 		 */
		while( tmp_ptFontOperate ) 
			tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
		
		tmp_ptFontOperate = ptFontOparate;
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
	PT_FontOperate tmp_ptFontOperate = g_ptFontOperate;

	while( tmp_ptFontOperate ) {
		printf("%02d %s\n",i++, tmp_ptFontOperate->name);
		tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
	}
}


int FontsInit(void)
{
	int err;
	
	err = AsciiInit();
	if( err ) {
		printf("AsciiFontInit err\n");
		return -1;
	}
	
	err = FreeTypeInit();
	if( err ) {
		printf("FreeTypeInit err\n");
		return -1;
	}
	
	err = GBKInit();
	if( err ) {
		printf("GBKInit err\n");
		return -1;
	}
	
	
	return 0;
}



