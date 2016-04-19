#ifndef __BREATH_LED_H
#define	__BREATH_LED_H

#include "stm32f4xx.h"

//引脚定义
/*******************************************************/
//R 红色灯
#define BREATH_LED1_PIN                  GPIO_Pin_10                 
#define BREATH_LED1_GPIO_PORT            GPIOH                      
#define BREATH_LED1_GPIO_CLK             RCC_AHB1Periph_GPIOH
#define BREATH_LED1_PINSOURCE						GPIO_PinSource10
#define BREATH_LED1_AF										GPIO_AF_TIM5



//G 绿色灯
#define BREATH_LED2_PIN                  GPIO_Pin_11                 
#define BREATH_LED2_GPIO_PORT            GPIOH                      
#define BREATH_LED2_GPIO_CLK             RCC_AHB1Periph_GPIOH
#define BREATH_LED2_PINSOURCE						GPIO_PinSource11
#define BREATH_LED2_AF										GPIO_AF_TIM5

//B 蓝色灯
#define BREATH_LED3_PIN                  GPIO_Pin_12                 
#define BREATH_LED3_GPIO_PORT            GPIOH                       
#define BREATH_LED3_GPIO_CLK             RCC_AHB1Periph_GPIOH
#define BREATH_LED3_PINSOURCE						GPIO_PinSource12
#define BREATH_LED3_AF										GPIO_AF_TIM5

//小指示灯，普通IO口，不能用PWM控制
#define LED4_PIN                  GPIO_Pin_11                 
#define LED4_GPIO_PORT            GPIOD                       
#define LED4_GPIO_CLK             RCC_AHB1Periph_GPIOD



/*定时器*/

#define BREATH_TIM           		TIM5
#define BREATH_TIM_CLK       		RCC_APB1Periph_TIM5

#define BREATH_LED1_CCRx								CCR1
#define BREATH_LED2_CCRx								CCR2
#define BREATH_LED3_CCRx								CCR3

#define BREATH_LED1_TIM_CHANNEL				TIM_Channel_1
#define BREATH_LED2_TIM_CHANNEL				TIM_Channel_2
#define BREATH_LED3_TIM_CHANNEL				TIM_Channel_3

#define BREATH_TIM_IRQn					TIM5_IRQn
#define BREATH_TIM_IRQHandler 	TIM5_IRQHandler
/************************************************************/


/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */
//
#define BREATH_LED1(a)	if (a)	\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED1_TIM_CHANNEL,TIM_CCx_Disable);\
					else		\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED1_TIM_CHANNEL,TIM_CCx_Enable)

#define BREATH_LED2(a)	if (a)	\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED2_TIM_CHANNEL,TIM_CCx_Disable);\
					else		\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED2_TIM_CHANNEL,TIM_CCx_Enable)

#define BREATH_LED3(a)	if (a)	\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED3_TIM_CHANNEL,TIM_CCx_Disable);\
					else		\
					TIM_CCxCmd (BREATH_TIM,BREATH_LED3_TIM_CHANNEL,TIM_CCx_Enable)
		
//LED 4是普通IO口					
#define LED4(a)	if (a)	\
					GPIO_SetBits(LED4_GPIO_PORT,LED4_PIN);\
					else		\
					GPIO_ResetBits(LED4_GPIO_PORT,LED4_PIN)


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRRL=i;}			  //设置为高电平		
#define digitalLo(p,i)			{p->BSRRH=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/*控制呼吸灯的宏*/
#define BREATH_LED1_OFF				TIM_CCxCmd (BREATH_TIM,BREATH_LED1_TIM_CHANNEL,TIM_CCx_Disable);
#define BREATH_LED1_ON				TIM_CCxCmd (BREATH_TIM,BREATH_LED1_TIM_CHANNEL,TIM_CCx_Enable);

#define BREATH_LED2_OFF				TIM_CCxCmd (BREATH_TIM,BREATH_LED2_TIM_CHANNEL,TIM_CCx_Disable);
#define BREATH_LED2_ON				TIM_CCxCmd (BREATH_TIM,BREATH_LED2_TIM_CHANNEL,TIM_CCx_Enable);

#define BREATH_LED3_OFF				TIM_CCxCmd (BREATH_TIM,BREATH_LED3_TIM_CHANNEL,TIM_CCx_Disable);
#define BREATH_LED3_ON				TIM_CCxCmd (BREATH_TIM,BREATH_LED3_TIM_CHANNEL,TIM_CCx_Enable);

/* 定义控制IO的宏 */
#define LED4_TOGGLE		digitalToggle(LED4_GPIO_PORT,LED4_PIN)
#define LED4_OFF				digitalHi(LED4_GPIO_PORT,LED4_PIN)
#define LED4_ON				digitalLo(LED4_GPIO_PORT,LED4_PIN)


/* 基本混色，后面高级用法使用PWM（全彩LED灯实验）可混出全彩颜色,且效果更好 */

//红
#define BREATH_LED_RED  \
					BREATH_LED1_ON;\
					BREATH_LED2_OFF\
					BREATH_LED3_OFF

//绿
#define BREATH_LED_GREEN		\
					BREATH_LED1_OFF;\
					BREATH_LED2_ON\
					BREATH_LED3_OFF

//蓝
#define BREATH_LED_BLUE	\
					BREATH_LED1_OFF;\
					BREATH_LED2_OFF\
					BREATH_LED3_ON

					
//黄(红+绿)					
#define BREATH_LED_YELLOW	\
					BREATH_LED1_ON;\
					BREATH_LED2_ON\
					BREATH_LED3_OFF
//紫(红+蓝)
#define BREATH_LED_PURPLE	\
					BREATH_LED1_ON;\
					BREATH_LED2_OFF\
					BREATH_LED3_ON

//青(绿+蓝)
#define BREATH_LED_CYAN \
					BREATH_LED1_OFF;\
					BREATH_LED2_ON\
					BREATH_LED3_ON
					
//白(红+绿+蓝)
#define BREATH_LED_WHITE	\
					BREATH_LED1_ON;\
					BREATH_LED2_ON\
					BREATH_LED3_ON
					
//黑(全部关闭)
#define BREATH_LED_RGBOFF	\
					BREATH_LED1_OFF;\
					BREATH_LED2_OFF\
					BREATH_LED3_OFF
					




void BreathLED_Config(void);

#endif /* __BREATH_LED_H */
