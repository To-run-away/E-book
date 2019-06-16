#include <sys/select.h>
#include <config.h>
#include <input_manage.h>
#include <draw.h>

static PT_InputOperate g_ptInputOperateHead;
#if (INPUT_WAY == 2)
static fd_set g_Readfds;
static int g_Maxfd;
#endif

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

#if (INPUT_WAY == 2)
	FD_ZERO(&g_Readfds);
#endif

	while(tmp_ptInputOperate)
	{
		if(!tmp_ptInputOperate->DeviceInit()) {
			ret = 0;
#if (INPUT_WAY == 2)
			FD_SET(tmp_ptInputOperate->fd,&g_Readfds);
			if(g_Maxfd < tmp_ptInputOperate->fd)
				g_Maxfd = tmp_ptInputOperate->fd;
#endif
		}
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}
#if (INPUT_WAY == 2)	
	/*
 	 * select中最大文件句柄是fd + 1
	 */
	g_Maxfd += 1;
#endif

	return ret;
}
/*
 * 获取输入事件
 */
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	PT_InputOperate tmp_ptInputOperate = g_ptInputOperateHead;

#if (1 == INPUT_WAY)
	while(tmp_ptInputOperate)
	{
		/* 查询方式 */
		if(!tmp_ptInputOperate->GetInputEvent(ptInputEvent))
			return 0;
		tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
	}
#elif (2 == INPUT_WAY)
	int ret;


	/*
	 * 查询等待输入事件到来
 	 */
	ret = select(g_Maxfd, &g_Readfds, NULL, NULL, NULL);
	if(ret > 0) {
		while(tmp_ptInputOperate) {

			/*
			 * 查询是那个输入设备有数据了,然后获取数据
			 */
			if(FD_ISSET(tmp_ptInputOperate->fd, &g_Readfds)) {
				if(!tmp_ptInputOperate->GetInputEvent(ptInputEvent))
					return 0;
			}
			tmp_ptInputOperate = tmp_ptInputOperate->ptNext;
		}

	}
	else if(ret < 0){

		perror("select\n");
		return -1;
	}
	else {

		DBG_PRINTF("select timeout\n");
	}

#elif (3 == INPUT_WAY)



#endif

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


