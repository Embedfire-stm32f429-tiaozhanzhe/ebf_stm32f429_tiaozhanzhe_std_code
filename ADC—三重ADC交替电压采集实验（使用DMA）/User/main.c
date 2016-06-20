/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ADC采集电压，DMA模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"


// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint32_t ADC_ConvertedValue[3];

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[3]; 

static void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /*初始化USART1*/
  Debug_USART_Config();

  /* 初始化滑动变阻器用到的DAC，ADC数据采集完成后直接由DMA运输数据到
     ADC_ConvertedValue变量DMA直接改变ADC_ConvertedValue的值*/
  Rheostat_Init();

  printf("\r\n ----这是一个ADC实验(DMA传输)----\r\n");
  
  while (1)
  {
    Delay(0xffffee);  
    
    ADC_ConvertedValueLocal[0] =(float)((uint16_t)ADC_ConvertedValue[0]*3.3/4096); 
    ADC_ConvertedValueLocal[1] =(float)((uint16_t)ADC_ConvertedValue[2]*3.3/4096);
    ADC_ConvertedValueLocal[2] =(float)((uint16_t)ADC_ConvertedValue[1]*3.3/4096);
    
    printf("\r\n The current AD value = 0x%08X \r\n", ADC_ConvertedValue[0]); 
    printf("\r\n The current AD value = 0x%08X \r\n", ADC_ConvertedValue[1]); 
    printf("\r\n The current AD value = 0x%08X \r\n", ADC_ConvertedValue[2]); 
    
    printf("\r\n The current ADC1 value = %f V \r\n",ADC_ConvertedValueLocal[0]); 
    printf("\r\n The current ADC2 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
    printf("\r\n The current ADC3 value = %f V \r\n",ADC_ConvertedValueLocal[2]);
  }
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/

