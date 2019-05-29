
#include <display_manage.h>
#include <string.h>
#include <stdio.h>
#include <config.h>


/*
 * 可支持的显示器链表头
 */
static PT_DisplayOperate g_ptDisplayOperateHead;


/*
 * 注册一个显示器
 */
int RegisterDisplayOperate(PT_DisplayOperate ptDisplayOperate)
{
	PT_DisplayOperate tmp_ptDisplayOperate;

	if( !ptDisplayOperate ) {
		/*
		 * 注册的显示器无效
 		 */
		return -1;
	}

	if( !g_ptDisplayOperateHead ) {
		/*
		 * 第一个注册的显示器
		 */
		g_ptDisplayOperateHead = ptDisplayOperate;
		ptDisplayOperate->ptNext = NULL;
	} else {
		
		/*
		 * 在链表尾部注册一个显示器
		 */
		tmp_ptDisplayOperate = g_ptDisplayOperateHead;
		
		while( tmp_ptDisplayOperate->ptNext )  
			tmp_ptDisplayOperate = tmp_ptDisplayOperate->ptNext;

		tmp_ptDisplayOperate->ptNext = ptDisplayOperate;
		ptDisplayOperate->ptNext = NULL; 
	}
	return 0;
}

/*
 * 展示可支持的显示器有哪些
 */
void ShowDisplayOperate(void)
{
	int i = 0;
	PT_DisplayOperate tmp_ptDisplayOperate = g_ptDisplayOperateHead;

	while( tmp_ptDisplayOperate )	{
		printf("%02d %s\n", i++, tmp_ptDisplayOperate->name);
		tmp_ptDisplayOperate = tmp_ptDisplayOperate->ptNext;
	}
}

/*
 * 获取name显示器
 */
PT_DisplayOperate GetDisplayOperate(char *Name)
{
	PT_DisplayOperate tmp_ptDisplayOperate = g_ptDisplayOperateHead;

	while( tmp_ptDisplayOperate ) {
		if(!strcmp(tmp_ptDisplayOperate->name, Name))
			return tmp_ptDisplayOperate;
		tmp_ptDisplayOperate = tmp_ptDisplayOperate->ptNext;
	}
	
	return NULL;
}

/*
 * 初始化显示器
 */
int DisplayInit(void)
{	
	int err = 0;

	err = FBInit();

	return err;
}

