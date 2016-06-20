/**
  ******************************************************************************
  * @file    hard_timer.c
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   定时器
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
	

#include "drivers.h"
#include "bsp.h"

 void (*hard_timer_call_back)(void) = 0;

//period us

/**
  * @brief  开启定时器
  * @param  period: 定时周期，单位为us
  * @param  call_back: 定时时间到的回调函数
  * @retval None
  */
void start_timer(uint32_t period, void *call_back)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	uint32_t apbclock, Counter;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	hard_timer_call_back = (void (*)(void))call_back;
		
	TIM_DeInit(TIM2); 

	RCC_GetClocksFreq(&RCC_ClocksStatus);
	apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	Counter = apbclock*2/1000000*period;

	TIM_TimeBaseStructure.TIM_Period=Counter;
  	TIM_TimeBaseStructure.TIM_Prescaler=0;//设置为0即二分频
  	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	//TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_IRQn_Priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2,ENABLE);
}

/**
  * @brief  获取定时器的时钟频率
  * @param  None
  * @retval 定时器时钟频率
  */
uint32_t get_hard_timer_clk()
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	return RCC_ClocksStatus.PCLK1_Frequency;
}

/**
  * @brief  暂停定时器
  * @param  None
  * @retval None
  */
void pause_timer()
{
	TIM_Cmd(TIM2,DISABLE);	
}

/**
  * @brief  继续定时器
  * @param  None
  * @retval None
  */
void continue_timer()
{
	TIM_Cmd(TIM2,ENABLE);	
}

/**
  * @brief  重新倒计时
  * @param  None
  * @retval None
  */
void mode_timer_by_counter(uint32_t counter)
{
	TIM_SetAutoreload(TIM2, counter);
	TIM_Cmd(TIM2,ENABLE);	
}

/**
  * @brief  定时器时间到中断，会执行回调函数
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	if(hard_timer_call_back)
		hard_timer_call_back();
}



