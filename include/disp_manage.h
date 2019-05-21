
#ifndef __DISP_MANAGE_H__
#define __DISP_MANAGE_H__

/*
 * name:表示是那个设备
 * iXres:表示显示设备在x方向的分辨率
 * iYres:表示显示设备在Y方向的分辨率
 * DeviceInit:初始化显示设备
 * ShowPixel:显示一个点的像素
 * ClearScreen:清除屏幕颜色 
 */
typedef struct _DisplayOperate {
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int (*DeviceInit)(void);
	int (*ShowPixel)(int iPenX,int iPeny, unsigned int Colo);
	int (*ClearScreen)(unsigned int Color);
	struct _DisplayOperate *ptNext;
}T_DisplayOperate,*PT_DisplayOperate;

#endif /* __DISP_MANAGE_H__ */


