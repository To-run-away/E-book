
#include <config.h>
#include <input_manage.h>
#include <draw.h>

static PT_InputOperate g_ptInputOperateHead;

/*
 * 注册输入操作
 */
int RegisterInputOperate(PT_InputOperate ptInputOperate)
{
	PT_InputOperate tmp_ptInputOperate;

	if(!ptInputOperate)
		return -1;
	
	if(!g_ptInputOperateHead) {
		g_ptInputOperateHead = ptInputOperate;
		ptInputOperate->ptNext = NULL;
	}
	else {
		tmp_ptInputOperate = g_ptInputOperateHead;
		
		while(tmp_ptInputOperate->ptNext)
			tmp_ptInputOperate = tmp_ptInputOperate->ptNext;

		tmp_ptInputOperate->ptNext = ptInputOperate;
		ptInputOperate->ptNext = NULL;
	}

	return 0;
}

/*
 * 展示支持的输入设备
 */
void ShowInputOperate(void)
{
	int i = 0;
	PT_InputOperate tmp_ptInputOperate = g_ptInputOperateHead;
	
	while(tmp_ptInputOperate) {
		printf("%2d %s\n", i++, tmp_ptInputOperate->name);
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}
}



/*
 * 初始化输入设备
 */
int InputDeviceInit(void)
{
	PT_InputOperate tmp_ptInputOperate = g_ptInputOperateHead;
	int ret = -1;

	while(tmp_ptInputOperate)
	{
		if(!tmp_ptInputOperate->DeviceInit())
			ret = 0;
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}

	return ret;
}
/*
 * 获取输入事件
 */
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	PT_InputOperate tmp_ptInputOperate = g_ptInputOperateHead;

	while(tmp_ptInputOperate)
	{
		/* 查询方式 */
		if(!tmp_ptInputOperate->GetInputEvent(ptInputEvent))
			return 0;
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}

	return -1;
}



/*
 * 初始化输入操作
 */
int InputInit(void)
{
	int err;
	
	err = StdinInit();
	if(err)
		return err;

	err = TouchScreenInit();
	if(err)
		return err;
	return 0;
}


