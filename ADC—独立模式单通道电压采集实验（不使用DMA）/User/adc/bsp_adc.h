#ifndef __ADC_H
#define	__ADC_H


#include "stm32f4xx.h"

// RHEOSTAT: 滑动变阻器/电位器

// 如何换成其他ADC IO，只要修改这些宏即可，1有效，0无效，开发板默认接PC3
// 注意PF6 PF7 PF8 PF9在开发板中已经用在了其他地方，不能再做ADC实验
// 这里提供代码只是为了演示，如果你在自己的项目中这几个IO口没有复用，则可以这样使用 
#define RHEOSTAT_PC3     1
#define RHEOSTAT_PF6     0
#define RHEOSTAT_PF7     0
#define RHEOSTAT_PF8     0
#define RHEOSTAT_PF9     0

// ADC 中断宏定义
#define Rheostat_ADC_IRQ            ADC_IRQn
#define Rheostat_ADC_INT_FUNCTION   ADC_IRQHandler

#if     RHEOSTAT_PC3 //PC3 接电位器
#define RHEOSTAT_ADC_DR_ADDR       ((u32)ADC1+0x4c)

// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_13

// ADC DMA 通道宏定义，这个例程暂时不用DMA，我们使用中断读取转换值
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0
 
#elif   RHEOSTAT_PF6 // PF6 接电位器
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC3+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_6
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOF

#define RHEOSTAT_ADC              ADC3
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC3
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_4

#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_2
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

#elif   RHEOSTAT_PF7 // PF7 接电位器
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC3+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_7
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOF

#define RHEOSTAT_ADC              ADC3
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC3
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_5

#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_2
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

#elif   RHEOSTAT_PF8 // PF8 接电位器
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC3+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_8
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOF

#define RHEOSTAT_ADC              ADC3
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC3
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_6

#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_2
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

#elif   RHEOSTAT_PF9 // PF9 接电位器
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC3+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_9
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOF

#define RHEOSTAT_ADC              ADC3
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC3
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_7

#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_2
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

#endif  

void Rheostat_Init(void);


#endif /* __ADC_H */

