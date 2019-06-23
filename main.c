#include <stdio.h>
#include <config.h>
#include <display_manage.h>
#include <encoding_manage.h>
#include <fonts_manage.h>
#include <input_manage.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <draw.h>

#define STRING_LEN 128


int Initialize(void)
{
	int err;


	/* 显示器初始化 */
	err = DisplayInit();
	if( err ) {
		DBG_PRINTF("DisplayInit error\n");
		return err;	
	} 
	
	/* 字体初始化 */
	err = FontsInit();
	if( err ) {
		DBG_PRINTF("FontsInit error\n");
		return err;
	}	
	
	/* 初始化编码器 */
	err = EncodingInit();
	if( err ) {
		DBG_PRINTF("EncodingInit error\n");
		return err;
	}

	/*
	 * 初始化输入
	 */
	err = InputInit();	
	if( err ) {
		DBG_PRINTF("InputInit error\n");
		return err;
	}


	return 0;
}


/*
 * 显示支持的格式等
 */
void ShowSupportOption(void)
{	
	printf("support display:\n");
	ShowDisplayOperate();

	printf("support font:\n");
	ShowFontOperate();

	printf("support encoding:\n");
	ShowEncodingOperate();

	printf("support input\n");
	ShowInputOperate();
}


int main(int argc, char *argv[])
{
	int err;
	unsigned int FontSize = 16;
	char HzkFile[STRING_LEN];
	char FreeTypeFile[STRING_LEN];
	char TextFile[STRING_LEN];
	char Display[STRING_LEN];
	int ret;
	int list = 0;
	T_InputEvent tInputEvent;


	Display[0] = '\0';
	HzkFile[0] = '\0';
	FreeTypeFile[0] = '\0';
	TextFile[0] = '\0';
	
	strcpy(Display, "fb");

	/*
	 * 依次取出每个选项,以及选项里面的参数
  	 */
	while(-1 != (ret = getopt(argc, argv , "ls:f:h:d:"))) {

		switch( ret ) {
		
			case 'l':
				/* 显示支持的字体,显示器,解码器 */
				list = 1;
			break;
		
			case 's':
				/* 获取要显示的字符大小 */
				FontSize = strtoul(optarg , NULL, 0);
			break;
			
			case 'f':
				/* 获取freetype库中字体文件的位置 */
				strncpy(FreeTypeFile, optarg, STRING_LEN);
				FreeTypeFile[STRING_LEN - 1] = '\0';
			break;
			
			case 'h':
				strncpy(HzkFile, optarg, STRING_LEN);
				HzkFile[STRING_LEN - 1] = '\0';
			break;

			case 'd':
				strncpy(Display, optarg, STRING_LEN);
				Display[STRING_LEN - 1] = '\0';
			break;
			
			default:
				printf("Usage: %s [-s Size] [-d Display] [-f font_file] [-h hzk] <text_file>\n", argv[0]);
				printf("Usage: %s [-l list] \n", argv[0]);
				return -1;
			break;
		}
	}

	/*
 	 * 检查是否所有选项都已经取出来来
 	 */
	if((!list) && (optind >= argc)) {
		printf("Usage: %s [-s Size] [-d Display] [-f font_file] [-h hzk] <text_file>\n", argv[0]);
		printf("Usage: %s [-l list] \n", argv[0]);
		return -1;
	}

	/*
	 * 初始化设备
	 */
	err = Initialize();
	if( err ) {
		DBG_PRINTF("Initialize error\n");
		return -1;
	}

	/*
	 * 显示支持的设
 	 */
	if( list ) {
		ShowSupportOption();
		return 0;
	}

	strncpy(TextFile, argv[optind], STRING_LEN);
	TextFile[STRING_LEN - 1] = '\0';
	
	/*
 	 * 打开文本文件
	 */
	err = OpenTextFile(TextFile); 
	if( err ) {
		DBG_PRINTF("OpenTextFile error\n");
		return -1;
	}
	
	/*
	 * 选择文本文件细节(编码器,字体大小等)
	 */
	err = SetTextDetail(HzkFile, FreeTypeFile, FontSize);
	if( err ) {
		DBG_PRINTF("SetTextDetail error\n");
		return -1;
	}

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	/*
	 * 选择在哪显示
	 */
	err = SelectAndInitDisplay(Display);
	if( err ) {
		DBG_PRINTF("SelectAndInitDisplay error\n");
		return -1;
	}
	
	/*
	 * 初始化输入设备
	 */
	err = InputDeviceInit();
	if( err ) {
		DBG_PRINTF("InputDeviceInit error\n");
		return -1;
	}

	err = ShowNextPage();
	if( err ) {
		DBG_PRINTF("Error to show first page\n");
		return -1;
	}

	printf("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit:\n");

	while(1) {
		
		if(!GetInputEvent(&tInputEvent))
		{		
		
			if(INPUT_VALUE_DOWN == tInputEvent.value)
				ShowNextPage();
			else if(INPUT_VALUE_UP == tInputEvent.value)
				ShowPrevPage();
			else if(INPUT_VALUE_EXIT == tInputEvent.value)
				return 0;
		}
	}

	return 0;
}


