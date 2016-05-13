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
#include "ff.h"
#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "aux_data.h"


#define USE_SPI_FLASH        0

#define XBF_XINSONGTI25      1
#define XBF_XINSONGTI19      1
#define UNIGBK               1
#define GB2312               1
#define SD8782               1
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
extern FIL fnew;													/* file objects */
extern FATFS fs;													/* Work area (file system object) for logical drives */
extern FRESULT result; 
extern UINT  bw;            					    /* File R/W count */

/*
*********************************************************************************************************
* Description : 初始化BL8782 wifi模块使能引脚，并关闭使能
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOG,GPIO_Pin_9);  //关闭BL_8782wifi使能
}
/*
*********************************************************************************************************
* Description : 初始化KEY1
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void KEY1_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* 设置引脚速度 */
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  /*使用上面的结构体初始化按键*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
	* @note    读取SD卡XBF字库文件(eg.xinsongti25.xbf)内容烧写到SPI Flash无文件系统
	*          扇区中。
  */
int main(void)
{
  FRESULT res;
  
	BL8782_PDN_INIT();
	/* 初始化LED */
  LED_GPIO_Config();
  KEY1_INIT();

  /* 初始化调试串口，一般为串口1 */
  Debug_USART_Config();
  TM_FATFS_FLASH_SPI_disk_initialize();

    
  printf("\r\n 按一次KEY1开始烧写字库 \r\n"); 
  
  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0);
  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1); 

  //烧录文件    
   burn_file_sd2flash(burn_data,AUX_MAX_NUM);  

   while(1);


}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
