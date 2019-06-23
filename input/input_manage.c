#include <sys/select.h>
#include <config.h>
#include <input_manage.h>
#include <draw.h>
#include <string.h>
#include <pthread.h>

static PT_InputOperate g_ptInputOperateHead;
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_InputMutex;
static pthread_cond_t g_InputCond;


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


static void *InputEventThreadFunc(void *arg)
{
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	T_InputEvent tInputEvent;

	GetInputEvent = arg;
	

	while(1)
	{
		if(!GetInputEvent(&tInputEvent))
		{
			/*
			 * 使用全局变量,放置并发错误,放到临界区
			 */
			pthread_mutex_lock(&g_InputMutex);	
			g_tInputEvent = tInputEvent;
						
			/* 得到数据了唤醒显示线程 */
			pthread_cond_signal(&g_InputCond);
			pthread_mutex_unlock(&g_InputMutex);
		}

	}

	return NULL;
}



/*
 * 初始化输入设备
 */
int InputDeviceInit(void)
{
	PT_InputOperate tmp_ptInputOperate = g_ptInputOperateHead;
	int ret = -1;

	pthread_mutex_init(&g_InputMutex,NULL);
	pthread_cond_init(&g_InputCond,NULL);

	while(tmp_ptInputOperate)
	{
		if(!tmp_ptInputOperate->DeviceInit()) {
			ret = 0;

			/* 创建子线程 */
			ret = pthread_create(&tmp_ptInputOperate->tid, NULL, InputEventThreadFunc, tmp_ptInputOperate->GetInputEvent);
			if(ret) {
				DBG_PRINTF("pthread_create fail\n");
			}
		}
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}

	return ret;
}
/*
 * 获取输入事件
 */
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/*
	 * 访问临界资源
	 */
	pthread_mutex_lock(&g_InputMutex);

	/*
	 * 获取数据的线程休眠
	 */
	pthread_cond_wait(&g_InputCond, &g_InputMutex);

	/*
	 * 唤醒后标明数据已经获得按键
	 */
	*ptInputEvent = g_tInputEvent;

	pthread_mutex_unlock(&g_InputMutex);

	return 0;
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


