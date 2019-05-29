
#include <config.h>
#include <string.h>
#include <encoding_manage.h>


static int IsUft16beCode(const char *pucBufHead);
static int Utf16beGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code);


static T_EncodingOperate g_Uft16beEncodingOperate = {
	.name			= "utf-16be",
	.iHeadLen 		= 2,
	.IsSupport		= IsUft16beCode,
	.GetCodeFromBuf = Utf16beGetCodeFromBuf, 
};


static int IsUft16beCode(const char *pucBufHead)
{
	const char Utf16beHead[] = {0xFE, 0xFF, 0};      

	if(!strncmp(Utf16beHead, (const char *)pucBufHead, 2)) {
		/* utf-16be */
		return 1;
	}
	else
	{
		/* other */
		return 0;
	}
}

/*
 * 获得uft16文件的某个字节的编码
 * 返回值为该编码的字节数 
 */
static int Utf16beGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code)
{
	if( (pucStartBuf + 1) < pucEndBuf) {
		*Code = (pucStartBuf[0] << 8) | pucStartBuf[0];
		return 2;
	}
	else
	{	
		/* 文件结束 */
		return 0;
	}	
}


int Uft16beEnCodingInit(void)
{
	int err;

	err = AddFontOperateForEncoding(&g_Uft16beEncodingOperate, GetFontOperate("ascii"));
	if( err ) 
		return err;

	err = AddFontOperateForEncoding(&g_Uft16beEncodingOperate, GetFontOperate("freetype"));
	if( err )
		return err;
	return RegisterEncodingOpr(&g_Uft16beEncodingOperate);
}




