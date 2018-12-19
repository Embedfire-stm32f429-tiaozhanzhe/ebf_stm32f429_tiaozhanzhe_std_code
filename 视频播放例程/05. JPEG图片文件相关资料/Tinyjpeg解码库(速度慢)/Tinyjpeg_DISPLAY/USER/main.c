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
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
#include "./Bsp/lcd/bsp_lcd.h"
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT result; 
/**
  ******************************************************************************
  *                                任务函数
  ******************************************************************************
  */
extern void jpgDisplay(char *pic_name);
static  void maindelay(uint32_t time)
{
  for(;time!=0;time--)
  {
    __NOP(); __NOP();
    __NOP(); __NOP();
  }
}
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
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 初始化LED */
	LED_GPIO_Config();
	
  BL8782_PDN_INIT();
  
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	
  printf("\r\n this is a fatfs test demo \r\n");
/*************************   flash 文件系统   *********************************************/
	//在外部Flash挂载文件系统
	result = f_mount(&fs,"0:",1);
	printf("\r\n f_mount res_flash=%d \r\n",result);
	
	/*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);

	LCD_SetLayer(LCD_BACKGROUND_LAYER);//LCD_BACKGROUND_LAYER LCD_FOREGROUND_LAYER
  
	/* 整屏清为白色 */
  LCD_Clear(LCD_COLOR_CYAN);

	while(1)
	{
    jpgDisplay("0:/musicplayer.jpg");
    maindelay(18000000);
    LCD_Clear(LCD_COLOR_CYAN);
    maindelay(18000000);
    jpgDisplay("0:/LuFu.jpg");	 
    maindelay(18000000);
    LCD_Clear(LCD_COLOR_CYAN);
    maindelay(18000000);
    jpgDisplay("0:/456.jpg");	 
    maindelay(18000000);
    LCD_Clear(LCD_COLOR_CYAN);
    maindelay(18000000);
    jpgDisplay("0:/LuFuu.jpg");	 
    maindelay(18000000);
    LCD_Clear(LCD_COLOR_CYAN);
    maindelay(18000000);
    jpgDisplay("0:/123.jpg");	 
    maindelay(18000000);
    LCD_Clear(LCD_COLOR_CYAN);
    maindelay(18000000);
	}

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
