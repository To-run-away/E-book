
#include <stdio.h>
#include <string.h>
#include <encoding_manager.h>

/*
 * 多支持的编码链表头
 */
static PT_EncodingOperate g_ptEncodingOperateHead;


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
		ptEncodingOpr->ptNext = NULL:
	}
	else {
		
		/*
	 	 * 其余注册的在前面注册的后面(ptNext)挂接
		 */
		tmp_ptEncodingOpr = g_ptEncodingOperateHead->ptNext;
		
		while( tmp_ptEncodingOpr->ptNext ) 
			tmp_ptEncodingOpr = tmp_ptEncodingOpr->ptNext;
		
		tmp_ptEncodingOpr = ptEncodingOpr;
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
PT_EncodingOperate SelectEncodingOperate(unsigned char *pucFileHeadBuf)
{
	PT_EncodingOperate tmp_ptEncodingOperate = g_ptEncodingOperateHead;

	/*
	 * 查找到支持的编码格式
 	 */
	while( tmp_ptEncodingOperate ) 
		if( tmp_ptEncodingOperate->IsSupport( pucFileHeadBuf ) )
			return tmp_ptEncodingOperate;
		else
			tmp_ptEncodingOperate = tmp_ptEncodingOperate->ptNext;
	}
	printf("not find support encoding \n");
	
	return NULL;
}


