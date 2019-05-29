
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <encoding_manage.h>

/*
 * 多支持的编码链表头
 */
static PT_EncodingOperate g_ptEncodingOperateHead;


/*
 * 注册一个字体文件
 */
int RegisterEncodingOpr(PT_EncodingOperate ptEncodingOpr)
{
	PT_EncodingOperate tmp_ptEncodingOpr;

	
	/*
	 * 传入NULL指针表示无效编码
	 */
	if(!ptEncodingOpr)
		return -1;

	/*
	 * 第一个注册可以使用的编码
	 */
	if( !g_ptEncodingOperateHead ) {
		g_ptEncodingOperateHead = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}
	else {
		
		/*
	 	 * 其余注册的在前面注册的后面(ptNext)挂接
		 */
		tmp_ptEncodingOpr = g_ptEncodingOperateHead;
		
		while( tmp_ptEncodingOpr->ptNext ) 
			tmp_ptEncodingOpr = tmp_ptEncodingOpr->ptNext;
		
		tmp_ptEncodingOpr->ptNext = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}
	
	return 0;
}


/*
 * 展示可支持的编码有哪些
 */
void ShowEncodingOperate(void)
{
	int i = 0;
	PT_EncodingOperate tmp_ptEncodingOperate = g_ptEncodingOperateHead;

	while( tmp_ptEncodingOperate ) {
		printf("%2d, %s\n", i++, tmp_ptEncodingOperate->name);
		tmp_ptEncodingOperate = tmp_ptEncodingOperate->ptNext;
	}
}

/*
 * 获取当前文件所支持的编码格式
 */
PT_EncodingOperate SelectEncodingOperate(const char *pucFileHeadBuf)
{
	PT_EncodingOperate tmp_ptEncodingOperate = g_ptEncodingOperateHead;

	/*
	 * 查找到支持的编码格式
 	 */
	while( tmp_ptEncodingOperate )  {
		if( tmp_ptEncodingOperate->IsSupport( pucFileHeadBuf ) )
			return tmp_ptEncodingOperate;
		else
			tmp_ptEncodingOperate = tmp_ptEncodingOperate->ptNext;
	}
	DBG_PRINTF("not find support encoding \n");
	
	return NULL;
}

int AddFontOperateForEncoding(PT_EncodingOperate ptEncodingOperate,PT_FontOperate ptFontOperate)
{
	PT_FontOperate tmp_ptFontOperate;
		
	if( (!ptEncodingOperate) || (!ptFontOperate) )  {
		/* 参数无效 */
		DBG_PRINTF("Input paramate fail, %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	else {
		
		tmp_ptFontOperate = malloc( sizeof(T_FontOperate) );
		if( !tmp_ptFontOperate )  {
			return -1;
		}
		else {
			/*
			 * 在ptEncodingOperate解码器支持的链表头插入一个新的字体
			 * 表示这种字体可以被这个解码器解码
			 */
			memcpy(tmp_ptFontOperate, ptFontOperate, sizeof(T_FontOperate));
			tmp_ptFontOperate->ptNext = ptEncodingOperate->ptFontOprSupportHead;
			ptEncodingOperate->ptFontOprSupportHead = tmp_ptFontOperate;
			return 0;
		}	
	}
}


int DelFontOperateFromEncoding(PT_EncodingOperate ptEncodingOperate, PT_FontOperate ptFontOperate)
{
	PT_FontOperate tmp_ptFontOperate;
	PT_FontOperate tmp_ptFontOperate_Pre;

	if( (!ptEncodingOperate) || (!ptFontOperate) ) {
		DBG_PRINTF("Input paramate fail, %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	else {
		tmp_ptFontOperate = ptEncodingOperate->ptFontOprSupportHead;
		if( !strcmp(tmp_ptFontOperate->name, ptFontOperate->name) )	{
			/* 删除头结点 */
			ptEncodingOperate->ptFontOprSupportHead = tmp_ptFontOperate->ptNext;
			free(tmp_ptFontOperate);
			return 0;
		}
		tmp_ptFontOperate_Pre = ptEncodingOperate->ptFontOprSupportHead;
		tmp_ptFontOperate     = tmp_ptFontOperate_Pre->ptNext;
		while( tmp_ptFontOperate ) {
			/* 删除非头结点的节点 */
			if( !strcmp(tmp_ptFontOperate->name , ptFontOperate->name) ) {
				tmp_ptFontOperate_Pre->ptNext = tmp_ptFontOperate->ptNext;
				free(tmp_ptFontOperate);
				return 0;
			}
			else {
				tmp_ptFontOperate_Pre = tmp_ptFontOperate;
				tmp_ptFontOperate = tmp_ptFontOperate->ptNext;
			}
		}
	}
	return -1;
}


int EncodingInit(void)
{
	int err;
	
	err = AnsiEncodingInit();
	if( err ) {
		DBG_PRINTF("AnsiEncodingInit error!\n");
		return -1;
	}

	err = Uft16leEnCodingInit();
	if( err ) {
		DBG_PRINTF("Uft16leEnCodingInit error!\n");
		return -1;
	}

	err = Uft16beEnCodingInit();
	if( err ) {
		DBG_PRINTF("Uft16beEnCodingInit error !\n");
		return -1;
	}

	err = Uft8EncodingInit();
	if( err) {
		DBG_PRINTF("Uft8EncodingInit error!\n");
		return -1;
	}
	
	return 0;
}


