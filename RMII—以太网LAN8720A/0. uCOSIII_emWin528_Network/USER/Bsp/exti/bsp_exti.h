#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

#define EXTI_INT_GPIO_PORT                GPIOA
#define EXTI_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define EXTI_INT_GPIO_PIN                 GPIO_Pin_8
#define EXTI_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOA
#define EXTI_INT_EXTI_PINSOURCE           EXTI_PinSource8
#define EXTI_INT_EXTI_LINE                EXTI_Line8
#define EXTI_INT_EXTI_IRQ                 EXTI9_5_IRQn

#define EXTI_IRQHandler                   EXTI9_5_IRQHandler

void EXTI_MPU_Config(void);
void EXTI_MPU_STOP(void);
#endif /* __EXTI_H */
