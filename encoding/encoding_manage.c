
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


