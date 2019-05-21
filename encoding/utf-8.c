
#include <string.h>

static int IsUtf8Coding(unsigned char *pucBufHead);
static int Utf8GetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code);



static T_EncodingOperate g_tUTF8EncodingOperate = {
	.name 			= "utf-8",
	.iHeadLen 		= 3,
	.IsSupport		= IsUtf8Coding,  
	.GetCodeFromBuf = ,
};



/*
 * 判断这个文件里是不是utf-8编码的字符
 */
static int IsUtf8Coding(unsigned char *pucBufHead)
{

	const unsigned char Utf8Head[] = {0xef, 0xbb, 0xbf, 0};

	/* 所有的utf-8编码的文件头都是上面的三字节 */	
	if(!strncmp(Utf8Head, pucBufHead, 3))
	{
		/* utf-8 */
		return 1;
	}

	return 0;
}


/* 获得前导为1的位的个数
 * 比如二进制数 11001111 的前导1有2位
 *              11100001 的前导1有3位
 */
static int GetPreOneBits(unsigned char ucVal)
{
	int i;
	int num_bits = 0;
	
	for (i = 7; i >= 0; i--)
	{
		if (ucVal & (1 << i))
			num_bits ++;
		else
			break;
	}
	return num_bits;

}



/*
 * 返回值：当前字体编码使用的字节个数,0表示文件结束
 */
static int Utf8GetCodeFromBuf(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int Code)
{
	/*
 	 *	UTF-8是Unicode的实现方式之一
 	 *  UTF-8最大的一个特点，就是它是一种变长的编码方式。它可以使用1~4个字节表示一个符号，根据不同的符号而变化字节长度
	 *  UTF-8的编码规则很简单，只有二条：
	 *	1）对于单字节的符号，字节的第一位设为0，后面7位为这个符号的unicode码。因此对于英语字母，UTF-8编码和ASCII码是相同的。
	 * 	2）对于n字节的符号（n>1），第一个字节的前n位都设为1，第n+1位设为0，后面字节的前两位一律设为10。剩下的没有提及的二进制位，全部为这个符号的unicode码。
	 * 	下表总结了编码规则，字母x表示可用编码的位。
	 *	Unicode符号范围 | UTF-8编码方式
	 *	(十六进制) | （二进制）
	 *	0000 0000-0000 007F | 0xxxxxxx
	 * 	0000 0080-0000 07FF | 110xxxxx 10xxxxxx
	 * 	0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
	 *	0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	 * 	......
	 *
	 *	下面，还是以汉字"严"为例，演示如何实现UTF-8编码:
	 * 	已知"严"的unicode是4E25（100111000100101）,根据上表，可以发现4E25处在第三行的范围内（0000 0800-0000 FFFF）
	 *	，因此"严"的UTF-8编码需要三个字节，即格式是"1110xxxx 10xxxxxx 10xxxxxx"。
	 *	然后，从"严"的最后一个二进制位开始，依次从后向前填入格式中的x，多出的位补0。
	 *	这样就得到了，"严"的UTF-8编码是"11100100 10111000 10100101"，转换成十六进制就是E4B8A5。
	 *	
 	 */
	
	unsigned char val;
	int num, i;
	unsigned int unicode;

	if(pucStartBuf >= pucEndBuf) 
		return 0;

	val = pucStartBuf[0];
	num = GetPreOneBits(val);
	
	if( !num ) {
		/* ascii */
		*Code = val;
		return 1;
	}
	else
	{
		/*
		 * 1.先取出第一个字节中的有效unicode数据 
		 * 2.接下来取出剩余字节中有效的unicode数据并组个起来 
		 */
		val <<= num;
		unicode = val >> num;
	
		for(i = 1; i < num; i ++) {
			val = pucStartBuf[i] & 0x3f;
			unicode <<= 6;
			unicode |= val;
		}
		*Code = unicode;

		return num;
	}
}

/*
 * utf8编码注册函数
 */
int Uft8EncodingInit(void)
{
	
	return RegisterEncodingOpr(&g_tUTF8EncodingOperate);	
}


