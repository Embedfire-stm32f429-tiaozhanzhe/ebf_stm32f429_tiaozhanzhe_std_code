#ifndef __LED_H_
#define	__LED_H_



#include "stm32f4xx.h"
 
 
 
/****************************** LED 引脚配置参数定义***************************************/
#define             macLED1_GPIO_xClock_FUN                 RCC_AHB1PeriphClockCmd
#define             macLED1_GPIO_CLK                        RCC_AHB1Periph_GPIOH
#define             macLED1_GPIO_PORT                       GPIOH
#define             macLED1_GPIO_PIN                        GPIO_Pin_10

#define             macLED2_GPIO_xClock_FUN                 RCC_AHB1PeriphClockCmd
#define             macLED2_GPIO_CLK                        RCC_AHB1Periph_GPIOH
#define             macLED2_GPIO_PORT                       GPIOH
#define             macLED2_GPIO_PIN                        GPIO_Pin_11

#define             macLED3_GPIO_xClock_FUN                 RCC_AHB1PeriphClockCmd
#define             macLED3_GPIO_CLK                        RCC_AHB1Periph_GPIOH
#define             macLED3_GPIO_PORT                       GPIOH
#define             macLED3_GPIO_PIN                        GPIO_Pin_12



/****************************** LED 函数宏定义***************************************/
#define             macLED1_ON()                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_OFF()                           GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) : GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )

#define             macLED2_ON()                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_OFF()                           GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) : GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )

#define             macLED3_ON()                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_OFF()                           GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) : GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )



/****************************** LED 函数声明 ***************************************/
void                               LED_Init                         ( void );
 
 

#endif /* __LED_H_ */

