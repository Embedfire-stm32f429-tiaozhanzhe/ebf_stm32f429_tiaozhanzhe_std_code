#ifndef __USART_H
#define	__USART_H

#include "stm32f4xx.h"
#include <stdio.h>


//引脚定义
/*******************************************************/
#define USARTx                             USART2
#define USARTx_CLK                         RCC_APB1Periph_USART2
#define USARTx_CLOCKCMD                    RCC_APB1PeriphClockCmd
#define USARTx_BAUDRATE                    115200  //串口波特率

#define USARTx_RX_GPIO_PORT                GPIOD
#define USARTx_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define USARTx_RX_PIN                      GPIO_Pin_6
#define USARTx_RX_AF                       GPIO_AF_USART2
#define USARTx_RX_SOURCE                   GPIO_PinSource6

#define USARTx_TX_GPIO_PORT                GPIOD
#define USARTx_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define USARTx_TX_PIN                      GPIO_Pin_5
#define USARTx_TX_AF                       GPIO_AF_USART2
#define USARTx_TX_SOURCE                   GPIO_PinSource5

/************************************************************/

void USARTx_Config(void);

#endif /* __USART_H */
