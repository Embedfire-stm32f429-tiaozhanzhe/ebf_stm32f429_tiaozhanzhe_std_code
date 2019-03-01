#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"

/*=====================通道1 IO======================*/
// PC3 通过调帽接电位器
// ADC IO宏定义
#if 1
#define RHEOSTAT_ADC_GPIO_PORT1    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define RHEOSTAT_ADC1             ADC1
#define RHEOSTAT_ADC1_CLK         RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL1     ADC_Channel_13
/*=====================通道2 IO ======================*/
// PA4 通过杜邦线链接0V-3.3V测试
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT2    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN2     GPIO_Pin_4
#define RHEOSTAT_ADC_GPIO_CLK2     RCC_AHB1Periph_GPIOA

#define RHEOSTAT_ADC2             ADC2
#define RHEOSTAT_ADC2_CLK         RCC_APB2Periph_ADC2
#define RHEOSTAT_ADC_CHANNEL2     ADC_Channel_4

#else

#define RHEOSTAT_ADC_GPIO_PORT1    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_4
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOA

// ADC 序号宏定义
#define RHEOSTAT_ADC1             ADC1
#define RHEOSTAT_ADC1_CLK         RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL1     ADC_Channel_4
/*=====================通道2 IO ======================*/
// PA4 通过杜邦线链接0V-3.3V测试
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT2    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN2     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK2     RCC_AHB1Periph_GPIOC

#define RHEOSTAT_ADC2             ADC2
#define RHEOSTAT_ADC2_CLK         RCC_APB2Periph_ADC2
#define RHEOSTAT_ADC_CHANNEL2     ADC_Channel_13

#endif

// ADC CDR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_CDR_ADDR    ((uint32_t)0x40012308)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0


void Rheostat_Init(void);

#endif /* __BSP_ADC_H */



