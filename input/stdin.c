#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <input_manage.h>
#include <sys/select.h>


int StdinDeviceInit(void);
int StdinDeviceExit(void);
int StdinGetInputEvent(PT_InputEvent ptInputEvent);

static T_InputOperate g_ptStdInputOperate = {
	.name 			= "stdin",
	.DeviceInit 	= StdinDeviceInit,
	.DeviceExit 	= StdinDeviceExit,
	.GetInputEvent 	= StdinGetInputEvent,

};

int StdinDeviceInit(void)
{
#if 0
	if ( (flags = fcntl(STDIN_FILENO, F_GETFL, 0)) < 0)
	{
		printf("F_GETFL error");
        exit(0);
    }

 	flags |= O_NONBLOCK;
    if (fcntl(STDIN_FILENO, F_SETFL, flags) < 0)
    {
        printf("F_SETFL error");
        exit(0);
    }

#endif

    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn off canonical mode
    tTTYState.c_lflag &= ~ICANON;
    //minimum of number input read.
    tTTYState.c_cc[VMIN] = 1;   /* 有一个数据时就立刻返回 */

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	g_ptStdInputOperate.fd = STDIN_FILENO;


	return 0;
}



int StdinDeviceExit(void)
{

#if 0
	if ( (flags = fcntl(STDIN_FILENO, F_GETFL, 0)) < 0)      
	{
		printf("F_SETFL error");  
		exit(1);
	}
	
	flsgs &= ~ O_NONBLOCK;


    if (fcntl(STDIN_FILENO, F_SETFL, flags) < 0)
    {
        printf("F_SETFL error");
        exit(0);
    }

#endif

	struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn on canonical mode
    tTTYState.c_lflag |= ICANON;
	
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);	
	
	return 0;
}

int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{

	char c;

	ptInputEvent->type = INPUT_TYPE_STDIN;
	c = fgetc(stdin); 

	gettimeofday(&ptInputEvent->time, NULL);

	if('u'  == c) 
		ptInputEvent->value = INPUT_VALUE_UP;
	else if('n' == c)
		ptInputEvent->value = INPUT_VALUE_DOWN;
	else if('q' == c)
		ptInputEvent->value = INPUT_VALUE_EXIT;
	else 
		ptInputEvent->value = INPUT_VALUE_UNKNOWN;

	return 0;
}

int StdinInit(void)
{
	return RegisterInputOperate(&g_ptStdInputOperate);
}


