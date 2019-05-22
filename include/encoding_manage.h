

#ifndef __ENCODING_MANAGE_H__
#define __ENCODING_MANAGE_H__

/*
 * name:字体文件类型
 * iHeadLen:头部的长度
 * IsSupport:检查是否支持这种头部
 * ptFontOprSupportHead:支持这种编码的字体链表头
 * GetCodeFromBuf:从编码获取对应的字模·
 */

typedef struct _EncodingOperate {
	char *name;
	int iHeadLen;
	PT_FontOperate ptFontOprSupportHead;		
	int (*IsSupport)(unsigned char *pucBufHead);
	int (*GetCodeFromBuf)(unsigned char *pucStartBuf, unsigned char *pucEndBuf, unsigned int *Code);
	struct _EncodingOperate *ptNext;
}T_EncodingOperate,*PT_EncodingOperate;



int Uft8EncodingInit(void);
int Uft16leEnCodingInit(void);
int Uft16beEnCodingInit(void);
void ShowEncodingOperate(void);

#endif /* __ENCODING_MANAGE_H__ */


