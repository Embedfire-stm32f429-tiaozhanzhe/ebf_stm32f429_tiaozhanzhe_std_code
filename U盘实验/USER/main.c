/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
/**
  ******************************************************************************
  *                              头文件
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/key/bsp_key.h"
#include "ff.h"
#include "./APP/usbh_bsp.h"

/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FRESULT result;
FATFS fs;
FIL file;

KEY Key1,Key2;

/* 仅允许本文件内调用的函数声明 */
static void ViewRootDir(void);
static void ReadFileData(void);
/**
  ******************************************************************************
  *                                任务函数
  ******************************************************************************
  */
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 初始化LED */
	LED_GPIO_Config();
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	
  printf("\r\n this is a fatfs test demo \r\n");
	
	/* Init Host Library */
		USBH_Init(&USB_OTG_Core,
			USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb,
            &USR_cb);
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		
		Key_RefreshState(&Key1);//刷新按键状态
		Key_RefreshState(&Key2);//刷新按键状态
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			LED2_TOGGLE;
			ViewRootDir();		/* 显示SD卡根目录下的文件名 */
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			LED3_TOGGLE;
			ReadFileData();		/* 读取根目录下test.txt的内容 */
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示根目录下的文件名
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ViewRootDir(void)
{
	DIR DirInf;
	FILINFO FileInf;
	uint32_t cnt = 0;
	char lfname[256];

 	/* 挂载文件系统 */
	result = f_mount(&fs,"0:",1);	/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败 (%d)\r\n", result);
		return;
//    result=f_mkfs("0:",0,0);							//格式化
//		printf("\r\nmkfs result=%d",result);
//		result = f_mount(&fs,"0:",0);						//格式化后，先取消挂载
//		printf("\r\n f_mount result=%d \r\n",result);
//		result = f_mount(&fs,"0:",1);						//重新挂载
//		printf("\r\n f_mount result=%d \r\n",result);
	}
  
  printf("挂载文件系统 (%d)\r\n", result);
  
	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK)
	{
		printf("打开根目录失败 (%d)\r\n", result);
		return;
	}
  
  printf("打开根目录 (%d)\r\n", result);
  
	/* 读取当前文件夹下的文件和目录 */
	FileInf.lfname = lfname;
	FileInf.lfsize = 256;

	printf("    属性    |  文件大小  |   短文件名   |  长文件名\r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* 判断是文件还是子目录 */
		if (FileInf.fattrib & AM_DIR)
		{
			printf("(0x%02d)目录  ", FileInf.fattrib);
		}
		else
		{
			printf("(0x%02d)文件  ", FileInf.fattrib);
		}

		/* 打印文件大小, 最大4G */
		printf(" %10d ", FileInf.fsize);
		
		printf("  %s |", FileInf.fname);	/* 短文件名 */
		
		printf("    %s\r\n", (char *)FileInf.lfname);	/* 长文件名 */
	}

	/* 卸载文件系统 */
	f_mount(&fs,"0:",0);
}


/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件test.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(void)
{	
	DIR DirInf;
	uint32_t bw;
	char buf[128];

 	/* 挂载文件系统 */
	result = f_mount(&fs,"0:",1);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("挂载文件系统失败(%d)\r\n", result);    
		return;    
	}

	/* 打开根文件夹 */
	result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK)
	{
		printf("打开根目录失败(%d)\r\n", result);
		return;
	}

	/* 打开文件 */
	result = f_open(&file, "test.txt", FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : test.txt\r\n");
		return;
	}

	/* 读取文件 */
	result = f_read(&file, &buf, sizeof(buf) - 1, &bw);
	if (bw > 0)
	{
		buf[bw] = 0;
		printf("\r\ntest.txt 文件内容 : \r\n%s\r\n", buf);
	}
	else
	{
		printf("\r\ntest.txt 文件内容 : \r\n");
	}

	/* 关闭文件*/
	f_close(&file);

	/* 卸载文件系统 */
	f_mount(&fs,"0:",0);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
