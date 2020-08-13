#ifndef __USART_DMA_H
#define	__USART_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>


//USART
#define DEBUG_USART                       USART1
#define DEBUG_USART_CLK                   RCC_APB2Periph_USART1
#define DEBUG_USART_RX_GPIO_PORT          GPIOA
#define DEBUG_USART_RX_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                GPIO_Pin_10
#define DEBUG_USART_RX_AF                 GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE             GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT          GPIOA
#define DEBUG_USART_TX_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                GPIO_Pin_9
#define DEBUG_USART_TX_AF                 GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE             GPIO_PinSource9

#define DEBUG_USART_BAUDRATE              115200

#define  DEBUG_USART_IRQ                  USART1_IRQn
#define  DEBUG_USART_IRQHandler           USART1_IRQHandler
//DMA
#define DEBUG_USART_DR_BASE               (USART1_BASE+0x04)		
#define RECEIVEBUFF_SIZE                  5000				//接收缓冲大小
#define DEBUG_USART_DMA_CLK               RCC_AHB1Periph_DMA2	
#define DEBUG_USART_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_DMA_STREAM            DMA2_Stream2


extern uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];

void Debug_USART_Config(void);
void USART_DMA_Config(void);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);

#endif /* __USART1_H */
