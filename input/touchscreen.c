#include <config.h>
#include <termios.h>
#include <unistd.h>
#include <input_manage.h>
#include <tslib.h>
#include <draw.h>
#include <stdlib.h>


int TouchScreenDeviceInit(void);
int TouchScreenDeviceExit(void);
int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent);

static T_InputOperate g_tTouchScreenInputOperate = {
	.name 			= "touchscreen",
	.DeviceInit 	= TouchScreenDeviceInit,
	.DeviceExit 	= TouchScreenDeviceExit,
	.GetInputEvent 	= TouchScreenGetInputEvent,

};

static struct tsdev *g_ptTs;

static int g_Xres,g_Yres;




int TouchScreenDeviceInit(void)
{
	/*
	 * 以非阻塞方式打开触摸屏
	 */
	g_ptTs = ts_setup(TOUCHSCREEN_DEVICE_NAME, 1);

	GetDisplayResolution(&g_Xres, &g_Yres);

	g_tTouchScreenInputOperate.fd = ts_fd(g_ptTs);
	//g_tTouchScreenInputOperate.fd = g_ptTs->fd;

	return 0;
}

/*
 * 关闭触摸屏
 */
int TouchScreenDeviceExit(void) 
{
	ts_close(g_ptTs);
	return 0;
}

/*
 * 两次时间间隔是否超过
 */
static int IsOutofTime(struct timeval *pPreTime,struct timeval *pNowTime)
{
	long pre_ms;
	long now_ms;

	pre_ms = pPreTime->tv_sec * 1000 + pPreTime->tv_usec / 1000;
	now_ms = pNowTime->tv_sec * 1000 + pNowTime->tv_usec / 1000;
	
	return ((pre_ms + 100) < now_ms);
}

/*
 * 获得按键值
 */
int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	struct ts_sample samp;
	int ret;
	static struct timeval pretime;

	ret = ts_read(g_ptTs, &samp, 1);
	if (ret < 0) {
		/* 查询方式,没读到数据 */
		DBG_PRINTF("ts_read err\n");
		return -1;
	}



	if(IsOutofTime(&pretime ,&samp.tv))
	{
		/* 两次上传坐标大于500ms再来处理按键事件 */
		pretime = samp.tv;
		ptInputEvent->type 	= INPUT_TYPE_TOUCHSCREAM;
		if(samp.x < (g_Xres / 3))
			ptInputEvent->value = INPUT_VALUE_UP;
		else if((g_Xres / 3 * 2) < samp.x)
			ptInputEvent->value = INPUT_VALUE_DOWN;
		else if(((g_Yres / 3 * 2) < samp.y ))		
			ptInputEvent->value  = INPUT_VALUE_EXIT;	//在中间位置,最下面部分为退出
		else
			ptInputEvent->value = INPUT_VALUE_UNKNOWN;
		
		return 0;
	}
	else
	{
		return -1;
	}
}

int TouchScreenInit(void)
{
	return RegisterInputOperate(&g_tTouchScreenInputOperate);
}

