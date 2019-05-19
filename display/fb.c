
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>




static int FBDeviceInit(void );
static int FBShowPixel(int iPenX, int iPeny, unsigned int Color);
static int FBClearScreen(unsigned int Color);


/* Current var framebuffer的可变参数*/
static struct fb_var_screeninfo g_tVar;   

/* Current fix framebuffer的固定参数 */
static struct fb_fix_screeninfo g_tFix;  

/*
 * 显示器fb映射在内存的地址
 */
static unsigned char *g_fbmem;

/*
 * 显示器fb的显存大小
 */
static unsigned int g_FbScreenSize;


/*
 * 显示一行索要占据的字节
 * 显示一个像素要占据的字节
 */
static int g_iLineWightSize;
static int g_iPixelWightSize;


static T_DisplayOperate g_tFBDisplayOperate = {
	.name 	= "fb",
	.DeviceInit = FBDeviceInit,
	.ShowPixel  = FBShowPixel,
	.ClearScreen= FBClearScreen,
};






/*
 * 初始化framebuffer
 */
static int FBDeviceInit(void )
{
	int fb_fd;


	/*  
     * 打开显示屏
     */
    fb_fd = open("/dev/fb0",O_RDWR);
    if(fb_fd < 0) {
        perror("open /dev/fb0 fail\n"); 
        exit(1);
    }   


    /*  
     * 获取显示屏可变的参数 
     */
	if(ioctl(fb_fd, FBIOGET_VSCREENINFO, &g_tVar)) {
        perror("ioctl fb0 FBIOGET_VSCREENINFO fail\n");
        exit(1);
    }   

  	/*  
     * 获取显示屏固定的参数 
     */
    if(ioctl(fb_fd, FBIOGET_FSCREENINFO, &g_tFix)) {
        perror("ioctl fb0 FBIOGET_FSCREENINFO fail\n");
        exit(1);
    }

	/*
	 * 设置fb设备的屏幕尺寸参数
	 */
	g_tFBDisplayOperate.iXres = g_tVar.xres;
	g_tFBDisplayOperate.iYres = g_tVar.yres;
	g_tFBDisplayOperate.iBpp  = g_tVar.bits_per_pixel;
	
	/*
 	 * 计算好显示一行所要的字节数和显示一个像素所要的字节数量
 	 */
	g_iLineWightSize = g_tVar.xres * g_tVar.bits_per_pixel / 8;
	g_iLineWightSize = g_tVar.bits_per_pixel >> 3;
	
    /*
     * 计算整个屏幕所使用的字节大小,并进行映射
	 * 这里采用共享方式映射,表示其他设备程序也可以对fb设备进行映射
     */
    g_FbScreenSize = (var.xres * var.yres * var.bits_per_pixel) >> 3;

    g_fbmem = mmap(NULL, g_FbScreenSize, PROT_READ |  PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if((void *)-1 == fbmem) {
        perror("fb0 mmap fail\n");
        exit(1);
    }


	return 0;
}


/*
 * 显示一个像素位置
 */
static int FBShowPixel(int iPenX, int iPenY, unsigned int Color)
{
	/*
  	 * 使用指针的特性,每次加1就是加的这个数据类型的大小来确定坐标
  	 */
	unsigned char *pen8   = (unsigned char *)g_fbmem;
	unsigned short *pen16 = (unsigned short *)g_fbmem;
	unsigned int *pen32   = (unsigned int *)g_fbmem;


	unsigned int red;
	unsigned int blue;
	unsigned int green;

	switch(g_tVar.bits_per_pixel)
	{

		case 32:
			pen32[iPenY * g_tVar.xres + iPenX] = (unsigned int)Color;
		break;

		case 16:
			red   = (Color >> 16) & 0x1f;
			green = (Color >> 8) & 0x3f;
			blue  = (Color >> 0) & 0x1f;
            Color = (red << 11) | (green << 5)| blue;

			pen16[iPenY * g_tVar.xres + iPenX] = (unsigned short)Color;
		break;

		case 8:
			pen8[iPenY * g_tVar.xres + iPenX] = (unsigned char)Color;
		break;
	
		default:
			printf("not support bits_per_pixel\n");
			return -1;
		break;
	}
	

	return 0;
}

/*
 * 清除屏幕背景
 */
static int FBClearScreen(unsigned int Color)
{
	unsigned int red;                                            
	unsigned int blue;
	unsigned int green;                                          
	unsigned int i;
	unsigned short* pen16 = (unsigned short *)g_fbmem; 
	unsigned int* pen32 = (unsigned int *)g_fbmem;  

	switch(g_tVar.bits_per_pixel)
    {

        case 32:
			/*
			 * 清除屏幕
 			 */
			for(i = 0; i < g_FbScreenSize; i += 4)
			{
				pen32[i] = (unsigned int)Color;
			}
        break;

        case 16:

			/*
			 * 清除屏幕
 			 */
			red   = (Color >> 16) & 0x1f;
			green = (Color >> 8) & 0x3f;
			blue  = (Color >> 0) & 0x1f;
			Color = (red << 11) | (green << 5)| blue;
			for(i = 0; i < g_FbScreenSize; i += 2)
			{   
				pen16[i] = (unsigned short)Color;
			}
        break;                                                   
        
        case 8:
			memset(g_fbmem, Color, g_FbScreenSize);
        break;

        default:
			printf("not support bits_per_pixel\n");
			return -1;
        break;
    }

	return 0;
}




