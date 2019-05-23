
#include <string.h>
#include <encoding_manage.h>


static int IsUft16leCode(unsigned char *pucBufHead);
static int Utf16leGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code);


static T_EncodingOperate g_Uft16leEncodingOperate = {
	.name			= "utf-16le",
	.iHeadLen 		= 2,
	.IsSupport		= IsUft16leCode,
	.GetCodeFromBuf = Utf16leGetCodeFromBuf, 
};


static int IsUft16leCode(unsigned char *pucBufHead)
{
	const char Utf16leHead[] = {0xFF, 0xFE, 0};      

	if(!strncmp(Utf16leHead, pucBufHead, 2)) {
		/* utf-16le */
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
static int Utf16leGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code)
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

/*
 * utf-16 little endian
 */
int Uft16leEnCodingInit(void)
{

	return RegisterEncodingOpr(&g_Uft16leEncodingOperate);
}




