#ifndef __ADC_H
#define	__ADC_H


#include "stm32f4xx.h"

// RHEOSTAT: 滑动变阻器/电位器
// 多通道ADC采集


//开发板中 PC3 通过调帽接了 滑动变阻器
//         PA4 通过调帽接了 光敏电阻
//         PA6 悬空，可通过杜邦线接3V3或者GND来实验
//转换的通道个数
//这里使用 ADC1的通道13-PC3、通道4-PA4、通道6-PA6
#define RHEOSTAT_NOFCHANEL      3

// ADC 序号相关宏定义
#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_DR_ADDR      ((u32)ADC1+0x4c)

/*=====================通道1 IO======================*/
// PC3 通过调帽接电位器
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT1    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOC
#define RHEOSTAT_ADC_CHANNEL1      ADC_Channel_13
/*=====================通道2 IO ======================*/
// PA4 通过调帽接光敏电阻
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT2    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN2     GPIO_Pin_4
#define RHEOSTAT_ADC_GPIO_CLK2     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL2      ADC_Channel_4
/*=====================通道3 IO ======================*/
// PA6 悬空，可用杜邦线接3V3或者GND来实验
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT3    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN3     GPIO_Pin_6
#define RHEOSTAT_ADC_GPIO_CLK3     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL3     ADC_Channel_6


// DMA2 数据流0 通道0
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0
 

void Rheostat_Init(void);


#endif /* __ADC_H */

