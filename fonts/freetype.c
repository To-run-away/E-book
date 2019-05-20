#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


static int FreeTypeFontInit(char *FontFile,unsigned int FontSize);
static int FreeTypeGetFontBitmap(unsigned int FontCode,PT_FontBitmap ptFontBitmap);



static FT_Library    g_tLibrary;
static FT_Face       g_tFace;
static FT_GlyphSlot  g_tSlot; 




/*
 * 分配,设置,注册一个T_FontOperate结构体
 */
static T_FontOperate g_FreeTypeFontOperate = {
	.name 			= "freetype",
	.FontInit 		= FreeTypeFontInit,
	.GetFontBitmap  = FreeTypeGetFontBitmap,
	
};



/*
 * 初始化FreeType字体库
 */
static int FreeTypeFontInit(char *FontFile,unsigned int FontSize)
{
	FT_Error      error;

	/*
     * 初始化freetype库
     */
    error = FT_Init_FreeType( &g_tLibrary);              /* initialize library */
    /* error handling omitted */
	if( error ) {
		printf("FT_Init_FreeType fail\n");
		return -1;
	}

    /*
     * 打开字符集
     */
    error = FT_New_Face( g_tLibrary, FontFile, 0, &g_tFace);/* create face object */
    /* error handling omitted */
	if( error ) {
		printf("FT_New_Face fial\n");
		return -1;
	}
 
    /* 获取字符集的插槽 */
    g_tSlot = face->glyph;
	
    /* 设置字体大小 */
    FT_Set_Pixel_Sizes(face, FontSize, 0 );


	return 0;
}


/*
 * 根据字符编码获得字体的渲染后的效果
 */
static int FreeTypeGetFontBitmap(unsigned int FontCode,PT_FontBitmap ptFontBitmap)
{
	
	FT_Error      error;


	/* 获取原点坐标 */
	int iPenX = ptFontBitmap->iCurOriginX;
	int iPenY = ptFontBitmap->iCurOriginY;


	/*
	 * 装载,并渲染字符 
	 * FT_LOAD_RENDER 表示转换成位图
  	 * FT_LOAD_MONOCHROME 表示转换成单色位图
	 */

    /* load glyph image into the slot (erase previous one) */
	error = FT_Load_Char( g_tFace, FontCode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if( error ) { 
		printf("FT_Load_Char err\n");
	}   

	/*
 	 * 获得字体左上角在lcd坐标系的坐标
 	 */
	ptFontBitmap->iXletf = iPenX + g_tSlot->bitmap_left;
	ptFontBitmap->iYtop  = iPenY - g_tSlot->bitmap_top;

	ptFontBitmap->iXmax  = ptFontBitmap->iXletf + g_tSlot->bitmap.width;
	ptFontBitmap->iYmax  = ptFontBitmap->iYtop  + g_tSlot->bitmap.rows;

	ptFontBitmap->iBpp   = 1;
	ptFontBitmap->iPatch = g_tSlot->bitmap.pitch;

	/*
 	 * 获取下一个字体的原点方向,我们在同一行显示,所以Y坐标保持不变
	 * advance里面的点的单位是1/64个点 
	 */
	ptFontBitmap->iNextOriginX = iPenX + g_tSlot->advance.x / 64 ;
	ptFontBitmap->iNextOriginY = iPenY;
	
	/* 设置显示数据 */
	ptFontBitmap->pucBuffer = g_tSlot->bitmap.buffer;

	return 0;
}


/*
 * 注册一个字体文件
 */
int FreeTypeInit()
{
	return RegisterFontOperate(&g_FreeTypeFontOperate);
}









