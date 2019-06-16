
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>

#define FB_DEVICE_NAME "/dev/fb0"
#define TOUCHSCREEN_DEVICE_NAME	"/dev/input/event0"

#define COLOR_BACKGROUND   0xE7DBB5  /* 泛黄的纸 */
#define COLOR_FOREGROUND   0x514438  /* 褐色字体 */

#define DBG_PRINTF(...)  
//#define DBG_PRINTF printf

/*
 * 输入方式分为三种：
 * 1:轮询方式
 * 2:select方式
 * 3:多线程方式
 * 
 */
#define INPUT_WAY 2	

#endif /* __CONFIG_H__ */


