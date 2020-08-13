/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   演示使用DMA进行串口传输
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
#include "./usart/bsp_usart_dma.h"
#include "./led/bsp_led.h"


static void Delay(__IO u32 nCount); 


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /* 初始化USART */
  Debug_USART_Config(); 

  /* 配置使用DMA模式 */
  USART_DMA_Config();
  
  /* 配置RGB彩色灯 */
  LED_GPIO_Config();

  printf("\r\n USART1 DMA RX 测试 \r\n");
  
  USART_DMACmd(DEBUG_USART, USART_DMAReq_Rx, ENABLE);


  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/

