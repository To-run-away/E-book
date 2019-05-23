
#ifndef __FONTS_MANAGE_H__
#define __FONTS_MANAGE_H__


/******************************
↑
|
|
|  (bitmap_lift,bitmap_top)			笛卡尔坐标系
| /
|/width
|-----
|	  | rows
|	  |
-------------------->
(0.0)



(0,0)
-------------------->
|	  |
|	  |
|-----
|\	LCD坐标系
| \
|  \
|	(iCurOriginX,iCurOriginY) 这里就是相当于笛卡尔坐标系的原点的字体位置
↓

***********************/

/*
 * 输入参数
 * iCurOriginX:当前字体原点所在位置(笛卡尔坐标)
 * iCurOriginY:当前字体原点所在位置(笛卡尔坐标)
 * 
 * 输出参数
 * iXletf:字体左上角x坐标
 * iYtop:字体左上角y坐标
 * iXmax:字体x方向的最大宽度
 * iYmax:字体y方向的最大高度
 * iBpp:一个像素所占的bit数量
 * iPitch:跨度信息,一行像素所占字节数
 * iNextOriginX:下一个字体x轴的起始坐标
 * iNextOriginY:下一个字体y轴的起始坐标
 */
typedef struct _FontBirmap {
	int iCurOriginX;
	int iCurOriginY;

	int iXletf;
	int iYtop;
	int iXmax;
	int iYmax;
	int iBpp;
	int iPitch;
	int iNextOriginX;
	int iNextOriginY;
	unsigned char *pucBuffer;
}T_FontBitmap,*PT_FontBitmap;




/*
 * name:字体名字
 * FontInit:字体文件初始化函数
 * GetFontBitmap:字体渲染函数
 */
typedef struct _FontOperate {
	char *name;
	int (*FontInit)(char *FontFile,unsigned int FontSize);
	int (*GetFontBitmap)(unsigned int FontCode, PT_FontBitmap ptFontBitmap);
	struct _FontOperate *ptNext;
}T_FontOperate,*PT_FontOperate;


int FontsInit(void);
void ShowFontOperate(void);
int RegisterFontOperate(PT_FontOperate ptFontOperate);

int AsciiInit(void);
int FreeTypeInit(void);
int GBKInit(void);


#endif /* __FONTS_MANAGE_H__ */

