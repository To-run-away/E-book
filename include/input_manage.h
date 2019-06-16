

#ifndef __INPUT_MANAGE_H__
#define __INPUT_MANAGE_H__

#include <sys/time.h>

/*
 * 输出设备类型,比如标准输入,触摸屏,按键
 */
typedef enum _InputType{
	INPUT_TYPE_STDIN,
	INPUT_TYPE_TOUCHSCREAM,
	INPUT_TYPE_KEYBOARD,
}E_InputType;


/*
 * 输入值
 */
typedef enum _InputValue {
	INPUT_VALUE_UP,
	INPUT_VALUE_DOWN,
	INPUT_VALUE_EXIT,
	INPUT_VALUE_UNKNOWN,
}E_InputValue;

/*
 * 输入事件
 */
typedef struct _InputEvent {
	struct timeval 	time;
	E_InputType 	type;
	E_InputValue 	value;
}T_InputEvent,*PT_InputEvent;

/*
 * 输入操作
 */
typedef struct _InputOperate {
	char *name;	
	int fd;
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct _InputOperate *ptNext;
}T_InputOperate, *PT_InputOperate ;

int StdinInit(void);
int TouchScreenInit(void);
int RegisterInputOperate(PT_InputOperate ptInputOperate);
void ShowInputOperate(void);
int GetInputEvent(PT_InputEvent ptInputEvent);
int InputDeviceInit(void);
int InputInit(void) ;
#endif /* __INPUT_MANAGE_H__ */

