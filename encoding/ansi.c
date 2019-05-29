
#include <config.h>
#include <string.h>
#include <encoding_manage.h>


static int IsAnsiCode(const char *pucBufHead);  
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
static int IsAnsiCode(const char *pucBufHead)
{
	const char Utf8Head[]    = {0xEF, 0xBB, 0xBF, 0};
	const char Utf16leHead[] = {0xFF, 0xFE, 0};
	const char Utf16beHead[] = {0xFE, 0xFF, 0};

	if (!strncmp(pucBufHead, Utf8Head, 3)) {
		/*utf-8 */
		return 0;
	}
	else if (!strncmp(pucBufHead, Utf16beHead, 2)) {
		/*utf-16 big endian */
		return 0;
	}
	else if (!strncmp(pucBufHead, Utf16leHead, 2)) {
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
	int err;
	
	err = AddFontOperateForEncoding(&g_AnsiEncodingOperate, GetFontOperate("freetype"));
	if( err )
		return err;
	err = AddFontOperateForEncoding(&g_AnsiEncodingOperate, GetFontOperate("ascii"));
	if( err )
		return err;
		
 	err = AddFontOperateForEncoding(&g_AnsiEncodingOperate, GetFontOperate("gbk"));
	if( err )
		return err;
	
	return RegisterEncodingOpr(&g_AnsiEncodingOperate);
}


