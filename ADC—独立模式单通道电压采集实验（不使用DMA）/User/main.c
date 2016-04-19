/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ADC采集电压，ADC中断模式，非DMA模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"
#include "./led/bsp_led.h"


// ADC转换的电压值通过MDA方式传到SRAM
__IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_Vol; 

static void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

		LED_GPIO_Config();
    /*初始化USART1*/
    Debug_USART_Config();
 
		/* 初始化滑动变阻器用到的ADC，ADC采集完成会产生ADC中断，
		在stm32f10x_it.c文件中的中断服务函数更新ADC_ConvertedValue的值 */
    Rheostat_Init();
  
    printf("\r\n 这是一个ADC 电压采集 实验(不使用DMA)\r\n");
	
    
    while (1)
    {
    
      printf("\r\n The current AD value = 0x%04X \r\n", ADC_ConvertedValue); 
      printf("\r\n The current AD value = %f V \r\n",ADC_Vol);     

			ADC_Vol =(float) ADC_ConvertedValue/4096*(float)3.3; // 读取转换的AD值


      Delay(0xffffee);  
    }
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/

