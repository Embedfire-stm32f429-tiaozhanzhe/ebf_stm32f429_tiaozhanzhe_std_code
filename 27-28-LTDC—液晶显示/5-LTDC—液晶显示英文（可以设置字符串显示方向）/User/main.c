/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDC-液晶显示英文，字库在内部FLASH
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include <string.h>

void Delay(__IO u32 nCount); 
void LCD_Test(void);
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
  
  /*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
  LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
  LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
  LCD_Clear(LCD_COLOR_RED);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/		
	
  LED_BLUE;    

  Delay(0xfff);  
  LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLUE);
  while(1)
	{

	  LCD_DisplayStringLine(LINE(0),(uint8_t* )"BH 5.0 inch LCD para:");
	  LCD_DisplayStringLine(LINE(2),(uint8_t* )"Image resolution:800x480 px");
	  LCD_DisplayStringLine(LINE(4),(uint8_t* )"Touch pad:5 point touch supported");
	  LCD_DisplayStringLine(LINE(6),(uint8_t* )"Use STM32-LTDC directed driver,");
	  LCD_DisplayStringLine(LINE(8),(uint8_t* )"no extern lcd driver needed,RGB888,24bits data bus");
	  LCD_DisplayStringLine(LINE(10),(uint8_t* )"Touch pad use IIC to communicate");
	}  

}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

