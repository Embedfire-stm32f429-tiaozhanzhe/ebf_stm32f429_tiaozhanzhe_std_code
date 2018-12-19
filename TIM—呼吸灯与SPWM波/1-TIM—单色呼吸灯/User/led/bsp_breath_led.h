#ifndef __BREATH_LED_H
#define	__BREATH_LED_H

#include "stm32f4xx.h"

/*PWM表中的点数*/
extern uint16_t  POINT_NUM	;
//控制输出波形的频率
extern __IO uint16_t period_class ;


#define RED_LIGHT 		1
#define GREEN_LIGHT 	2
#define BLUE_LIGHT		3

/*要使用什么颜色的呼吸灯，可选RED_LIGHT、GREEN_LIGHT、BLUE_LIGHT*/
#define LIGHT_COLOR 	RED_LIGHT

/********************定时器通道**************************/
#if  LIGHT_COLOR == RED_LIGHT
	//R 红色灯
	#define BRE_TIM           							TIM5
	#define BRE_TIM_CLK       						RCC_APB1Periph_TIM5
	#define BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd

	#define BRE_TIM_IRQn								TIM5_IRQn
	#define BRE_TIM_IRQHandler 				TIM5_IRQHandler


	#define BRE_LED_PIN                  GPIO_Pin_10                 
	#define BRE_LED_GPIO_PORT            GPIOH                      
	#define BRE_LED_GPIO_CLK             RCC_AHB1Periph_GPIOH
	#define BRE_LED_PINSOURCE							GPIO_PinSource10
	#define BRE_LED_AF										GPIO_AF_TIM5

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIMx->BRE_RED_CCRx ，可访问该通道的比较寄存器
	#define BRE_LED_CCRx									CCR1
	#define BRE_LED_TIM_CHANNEL					TIM_Channel_1

	#define  BRE_TIM_OCxInit              TIM_OC1Init            //通道选择，1~4
	#define  BRE_TIM_OCxPreloadConfig    TIM_OC1PreloadConfig 


#elif LIGHT_COLOR == GREEN_LIGHT
	//G 绿色灯
	#define BRE_TIM           							TIM5
	#define BRE_TIM_CLK       						RCC_APB1Periph_TIM5
	#define BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd

	#define BRE_TIM_IRQn								TIM5_IRQn
	#define BRE_TIM_IRQHandler 				TIM5_IRQHandler


	#define BRE_LED_PIN                  GPIO_Pin_11                 
	#define BRE_LED_GPIO_PORT            GPIOH                      
	#define BRE_LED_GPIO_CLK             RCC_AHB1Periph_GPIOH
	#define BRE_LED_PINSOURCE							GPIO_PinSource11
	#define BRE_LED_AF										GPIO_AF_TIM5

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIM->BRE_LED_CCRx ，可访问该通道的比较寄存器
	#define BRE_LED_CCRx									CCR2
	#define BRE_LED_TIM_CHANNEL					TIM_Channel_2

	#define  BRE_TIM_OCxInit              TIM_OC2Init            //通道选择，1~4
	#define  BRE_TIM_OCxPreloadConfig    TIM_OC2PreloadConfig 

#elif LIGHT_COLOR == BLUE_LIGHT
	//B 蓝色灯
	#define BRE_TIM           							TIM5
	#define BRE_TIM_CLK       						RCC_APB1Periph_TIM5
	#define BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd

	#define BRE_TIM_IRQn								TIM5_IRQn
	#define BRE_TIM_IRQHandler 				TIM5_IRQHandler


	#define BRE_LED_PIN                  GPIO_Pin_12                 
	#define BRE_LED_GPIO_PORT            GPIOH                      
	#define BRE_LED_GPIO_CLK             RCC_AHB1Periph_GPIOH
	#define BRE_LED_PINSOURCE							GPIO_PinSource12
	#define BRE_LED_AF										GPIO_AF_TIM5

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIM->BRE_LED_CCRx ，可访问该通道的比较寄存器
	#define BRE_LED_CCRx									CCR3
	#define BRE_LED_TIM_CHANNEL					TIM_Channel_3

	#define  BRE_TIM_OCxInit              TIM_OC3Init            //通道选择，1~4
	#define  BRE_TIM_OCxPreloadConfig    TIM_OC3PreloadConfig 

#endif

void BreathLED_Config(void);

#endif /* __BREATH_LED_H */
