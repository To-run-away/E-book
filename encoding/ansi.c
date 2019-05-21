

#include <encoding_manage.h>


static nt IsAnsiCode(unsigned char *pucBufHead);  
static int AnsiGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code);


static T_EncodingOperate g_AnsiEncodingOperate = {
	.name 			= "ansi",
	.iHeadLen 		= 0,
	.IsSupport 		= IsAnsiCode,
	.GetCodeFromBuf = AnsiGetCodeFromBuf,
};

/*
 * 判断是不是
 */
static int IsAnsiCode(unsigned char *pucBufHead)
{
	const unsigned char Utf8Head[]    = {0xEF, 0xBB, 0xBF, 0};
	const unsigned char Utf16leHead[] = {0xFF, 0xFE, 0};
	const unsigned char Utf16beHead[] = {0xFE, 0xFF, 0};

	if (!strncmp((const char*)pucBufHead, Utf8Head, 3)) {
		/*utf-8 */
		return 0;
	}
	else if (!strncmp((const char*)pucBufHead, Utf16beHead, 2)) {
		/*utf-16 big endian */
		return 0;
	}
	else if (!strncmp((const char*)pucBufHead, Utf16leHead, 2)) {
		/*utf-16 little endian */
		return 0;
	}
	else
	{
		/* ansi */
		return 1;
	}	
}


/*
 * 从ansi编码的文件文件读取数据
 */
static int AnsiGetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code)
{
	if( (pucStartBuf < pucEndBuf) && (pucStartBuf[0] < 0x80) ) {
		/* ascii */
		*Code = pucStartBuf[0];	
		return 1;	
	}

	if( ((pucStartBuf + 1) < pucEndBuf) && (0x80 < pucStartBuf[0]) ) {
		/* gbk */
		*Code = (pucStartBuf[1] << 8) | pucStartBuf[0];
		return 2;
	}

	if(pucStartBuf < pucEndBuf ) {
		/* 无效编码 */
		*Code = 0;
		return 1;
	}
	else
	{
		/* 文件结束 */
		return 0;
	}
}


/*
 * 注册ascii编码的字体
 */
int AnsiEncodingInit(void)
{

	return RegisterEncodingOpr(&g_AnsiEncodingOperate);
}


