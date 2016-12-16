/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   呼吸灯例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_breath_led.h"
#include "./usart/bsp_debug_usart.h"

static void Delay(__IO u32 nCount); 


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{

  /*初始化串口*/
  Debug_USART_Config();
 
	printf("\r\n 欢迎使用秉火  STM32 F429 开发板。\r\n");		
	printf("\r\n 呼吸灯例程\r\n");	  
  printf("\r\n RGB LED 以呼吸灯的形式闪烁\r\n ");

  /* 初始化呼吸灯 */
	BreathLED_Config(); 
  
  
  while(1)
  {
		BREATH_LED_RED;
		Delay(0xAFFFFFF); 
		
		BREATH_LED_GREEN;	
		Delay(0xAFFFFFF);
		
		BREATH_LED_BLUE;	
		Delay(0xAFFFFFF);
		
		BREATH_LED_YELLOW;
		Delay(0xAFFFFFF); 

		
		BREATH_LED_PURPLE;	
		Delay(0xAFFFFFF);
		
		BREATH_LED_CYAN;	
		Delay(0xAFFFFFF);		
	
		BREATH_LED_WHITE;	
		Delay(0xAFFFFFF);
		
		BREATH_LED_RGBOFF;	
		Delay(0xAFFFFFF);


  }

}


static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

