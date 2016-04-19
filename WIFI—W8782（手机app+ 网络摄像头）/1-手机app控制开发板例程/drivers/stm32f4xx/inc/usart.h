#ifndef __USART_DMA_H
#define	__USART_DMA_H

#include "stm32f4xx.h"
#include "api.h"




#define UART_PKG_SIZE  			256

#define UART_RECV_BUFF_SIZE 	(UART_PKG_SIZE + 32)

#define UART1_DEFAULT_BAUD  115200
#define UART3_DEFAULT_BAUD  115200

typedef struct _UART_PKG
{
 	atomic		pending;
	u32_t   	size;
  u8_t 			buff[UART_PKG_SIZE];    //buff包含格式:0xaa +CMD +LENL+LENH+SEQ +DATA0+…+DATAN+ SUM + 0x55
}UART_PKG;

typedef struct _UART_RECV
{	 
	u32_t 		over_err_num;   
	u32_t 		ne_err_num;  
	u32_t 		fe_err_num; 
	u32_t 		pkg_over_err;  
	u32_t 		recv_num;
	uint32_t 	pos;
	u8_t 		c_buff[UART_RECV_BUFF_SIZE]; 		//循环缓冲区
	UART_PKG 	pkg;  //保存一帧数据	 
}UART_RECV;

typedef struct _UART_SEND
{
	wait_event_t 	wait;
	u32_t 		send_num;
	u32_t 		cnt;
	u32_t 		size;
	atomic		sending;
	u8_t 		err;
	u8_t*		buff; 
}UART_SEND;


//USART
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_USART_IRQn													USART1_IRQn
#define DEBUG_USART_IRQnHandler									USART1_IRQHandler

//DMA
#define DEBUG_USART_DR_BASE                     (USART1_BASE+0x04)		// 0x40013800 + 0x04 = 0x40013804，串口数据寄存器地址
#define DEBUG_USART_DMA_CLK                      RCC_AHB1Periph_DMA2	
#define DEBUG_USART_DMA_TX_CHANNEL              DMA_Channel_4
#define DEBUG_USART_DMA_TX_STREAM               DMA2_Stream7
#define DEBUG_USART_DMA_TX_IRQn									DMA2_Stream7_IRQn
#define DEBUG_USART_DMA_TX_IRQHandler						DMA2_Stream7_IRQHandler

#define DEBUG_USART_DMA_RX_CHANNEL              DMA_Channel_4
#define DEBUG_USART_DMA_RX_STREAM               DMA2_Stream5
#define DEBUG_USART_DMA_RX_IRQn									DMA2_Stream5_IRQn
#define DEBUG_USART_DMA_RX_IRQHandler						DMA2_Stream5_IRQHandler


void Debug_USART_Config(void);
void uart_interface_off(int onoff);
int set_baud(int uart_num,uint32_t baud);
void enable_rcvdata(void);
int uart1_send(u8_t *buff,u32_t size);
int handle_uart1_rcv(void);
void send_cmd_to_self(char cmd);

#endif /* __USART1_H */

