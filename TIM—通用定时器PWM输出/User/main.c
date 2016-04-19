/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   通用定时器PWM输出
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
#include "./tim/bsp_general_tim.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{	
  /* 初始化通用定时器PWM输出 */
	TIMx_Configuration();
  
  while(1)
  {       
  }
}



/*********************************************END OF FILE**********************/

