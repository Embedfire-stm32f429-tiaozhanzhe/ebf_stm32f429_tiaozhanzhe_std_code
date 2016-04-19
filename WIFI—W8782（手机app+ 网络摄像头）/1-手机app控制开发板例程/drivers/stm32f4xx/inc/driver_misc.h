#ifndef _DRIVER_MISC_H
#define _DRIVER_MISC_H

#include "stm32f4xx.h"

typedef enum
{ GPIO_Mode_AINN = 0x0,             // 模拟输入
  GPIO_Mode_IN_FLOATING = 0x04,		 // 浮空输入
  GPIO_Mode_IPD = 0x28,						 // 下拉输入
  GPIO_Mode_IPU = 0x48,						 // 上拉输入
  GPIO_Mode_Out_OD = 0x14,				 // 开漏输出
  GPIO_Mode_Out_PP = 0x10,				 // 推免输出
  GPIO_Mode_AF_OD = 0x1C,					 // 复用开漏输出
  GPIO_Mode_AF_PP = 0x18,					 // 复用推免输出
  GPIO_Mode_AF_IF = 0x19,					 // 复用浮空输入
  GPIO_Mode_AF_IPU = 0x1A					 // 复用上拉输入
}GPIOMode_TypeDef1;



//define sdio PORT

#define SDIO_CLK_PORT				GPIOC
#define SDIO_CLK_PIN					GPIO_Pin_12
#define SDIO_CLK_SOURCE				GPIO_PinSource12
#define SDIO_D0_PORT				GPIOC
#define SDIO_D0_PIN					GPIO_Pin_8
#define SDIO_D0_SOURCE				GPIO_PinSource8
#define SDIO_D1_PORT				GPIOC
#define SDIO_D1_PIN					GPIO_Pin_9
#define SDIO_D1_SOURCE				GPIO_PinSource9
#define SDIO_D2_PORT				GPIOC
#define SDIO_D2_PIN					GPIO_Pin_10
#define SDIO_D2_SOURCE				GPIO_PinSource10
#define SDIO_D3_PORT				GPIOC
#define SDIO_D3_PIN					GPIO_Pin_11
#define SDIO_D3_SOURCE				GPIO_PinSource11

#define SDIO_CMD_PORT				GPIOD
#define SDIO_CMD_PIN				GPIO_Pin_2
#define SDIO_CMD_SOURCE			GPIO_PinSource2

//RESET引脚在8782模块上没用到，实际由PDN控制复位
//#define SDIO_RESET_PORT 				GPIOA
//#define SDIO_RESET_PIN				GPIO_Pin_11

#define SDIO_PDN_PORT 				GPIOG
#define SDIO_PDN_PIN					GPIO_Pin_9



//#define SDIO_RESET_HIGH				GPIO_SET(SDIO_RESET_PORT, SDIO_RESET_PIN)
//#define SDIO_RESET_LOW				GPIO_CLR(SDIO_RESET_PORT, SDIO_RESET_PIN)

#define SDIO_PDN_HIGH					GPIO_SET(SDIO_PDN_PORT, SDIO_PDN_PIN)
#define SDIO_PDN_LOW					GPIO_CLR(SDIO_PDN_PORT, SDIO_PDN_PIN)


//define flash PORT
#define FLASH_CS_PORT 				GPIOI
#define FLASH_CS_PIN 				GPIO_Pin_8

#define FLASH_CLK_PORT 			GPIOB
#define FLASH_CLK_PIN 				GPIO_Pin_3
#define FLASH_CLK_PIN_SOURCE 		GPIO_PinSource3

#define FLASH_MISO_PORT 			GPIOB
#define FLASH_MISO_PIN 				GPIO_Pin_4
#define FLASH_MISO_PIN_SOURCE  	GPIO_PinSource4

#define FLASH_MOSI_PORT 			GPIOB
#define FLASH_MOSI_PIN 				GPIO_Pin_5
#define FLASH_MOSI_PIN_SOURCE  	GPIO_PinSource5

//#define FLASH_WP_PORT 				GPIOA
//#define FLASH_WP_PIN 				GPIO_Pin_0



#define FLASH_CS_ENABLE	 	GPIO_CLR(FLASH_CS_PORT,FLASH_CS_PIN)
#define FLASH_CS_DISABLE		GPIO_SET(FLASH_CS_PORT,FLASH_CS_PIN)

//#define FLASH_WP_ENABLE		GPIO_CLR(FLASH_WP_PORT,FLASH_WP_PIN)
//#define FLASH_WP_DISABLE		GPIO_SET(FLASH_WP_PORT,FLASH_WP_PIN)


//define DCMI PORT
#define DCMI_HS_PORT_GROUP 	GPIOA
#define DCMI_HS_PIN 				GPIO_Pin_4
#define DCMI_HS_SOURCE 		GPIO_PinSource4

#define DCMI_VS_PORT_GROUP 	GPIOI
#define DCMI_VS_PIN 				GPIO_Pin_5
#define DCMI_VS_SOURCE 		GPIO_PinSource5

#define DCMI_PCLK_PORT_GROUP GPIOA
#define DCMI_PCLK_PIN 			GPIO_Pin_6
#define DCMI_PCLK_SOURCE 		GPIO_PinSource6

#define DCMI_D0_PORT_GROUP 	GPIOH
#define DCMI_D0_PIN 				GPIO_Pin_9
#define DCMI_D0_SOURCE 			GPIO_PinSource9

#define DCMI_D1_PORT_GROUP 	GPIOH
#define DCMI_D1_PIN 				GPIO_Pin_10
#define DCMI_D1_SOURCE 			GPIO_PinSource10

#define DCMI_D2_PORT_GROUP 	GPIOH
#define DCMI_D2_PIN 				GPIO_Pin_11
#define DCMI_D2_SOURCE 			GPIO_PinSource11

#define DCMI_D3_PORT_GROUP 	GPIOH
#define DCMI_D3_PIN 				GPIO_Pin_12
#define DCMI_D3_SOURCE 			GPIO_PinSource12

#define DCMI_D4_PORT_GROUP 	GPIOH
#define DCMI_D4_PIN 				GPIO_Pin_14
#define DCMI_D4_SOURCE 			GPIO_PinSource14

#define DCMI_D5_PORT_GROUP 	GPIOD
#define DCMI_D5_PIN 				GPIO_Pin_3
#define DCMI_D5_SOURCE 			GPIO_PinSource3

#define DCMI_D6_PORT_GROUP 	GPIOI
#define DCMI_D6_PIN 				GPIO_Pin_6
#define DCMI_D6_SOURCE 			GPIO_PinSource6

#define DCMI_D7_PORT_GROUP 	GPIOI
#define DCMI_D7_PIN 				GPIO_Pin_7
#define DCMI_D7_SOURCE 			GPIO_PinSource7

#define MCO_PORT_GROUP 		GPIOA
#define MCO_PIN 					GPIO_Pin_8
#define MCO_PIN_SOURCE 			GPIO_PinSource8

#define DCMI_RST_PORT_GROUP 	GPIOG
#define DCMI_RST_PIN 			GPIO_Pin_2

#define DCMI_PWD_PORT_GROUP 	GPIOG
#define DCMI_PWD_PIN 			GPIO_Pin_3

#define SIO_C_PORT_GROUP 		GPIOB
#define SIO_C_PIN 				GPIO_Pin_6

#define SIO_D_PORT_GROUP 		GPIOB
#define SIO_D_PIN 				GPIO_Pin_7

#define SIO_D_OUT		gpio_cfg((uint32_t)SIO_D_PORT_GROUP, SIO_D_PIN, GPIO_Mode_Out_PP);
#define SIO_D_IN			gpio_cfg((uint32_t)SIO_D_PORT_GROUP, SIO_D_PIN, GPIO_Mode_IPU);

//end define DCMI PORT
#define GPIO_SET(port,pin) (port->BSRRL = pin)
#define GPIO_CLR(port,pin) (port->BSRRH = pin)
#define GPIO_STAT(port,pin)   (!!(port->IDR & pin))
// 3个LED灯
#define IND1_GROUP	GPIOH
#define IND2_GROUP	GPIOH
#define IND3_GROUP	GPIOH

#define IND1_PIN	GPIO_Pin_10
#define IND2_PIN	GPIO_Pin_11
#define IND3_PIN	GPIO_Pin_12

#define IND1_ON 		do{GPIO_CLR(IND1_GROUP, IND1_PIN); }while(0)
#define IND1_OFF 	do{GPIO_SET(IND1_GROUP, IND1_PIN); }while(0)

#define IND2_ON 		do{GPIO_CLR(IND2_GROUP, IND2_PIN); }while(0)
#define IND2_OFF 	do{GPIO_SET(IND2_GROUP, IND2_PIN); }while(0)

#define IND3_ON 		do{GPIO_CLR(IND3_GROUP, IND3_PIN); }while(0)
#define IND3_OFF 	do{GPIO_SET(IND3_GROUP, IND3_PIN);}while(0)

#define IND1_SET IND1_ON
#define IND1_CLR IND1_OFF

#define IND2_SET IND2_ON
#define IND2_CLR IND2_OFF

#define IND3_SET IND3_ON
#define IND3_CLR IND3_OFF



#define LED1   IND1_GROUP,IND1_PIN
#define LED2   IND2_GROUP,IND2_PIN
#define LED3   IND3_GROUP,IND3_PIN


#define BUTTON_PORT						GPIOA
#define BUTTON_PIN							GPIO_Pin_0
#define BUTTON_SOURCE					GPIO_PinSource0
#define BUTTON_EXTI_LINE 			EXTI_Line0
#define BUTTON_STAT 	GPIO_STAT(BUTTON_PORT, BUTTON_PIN)




void gpio_cfg(uint32_t group, uint32_t pin, uint8_t mode);
void usr_gpio_init(void);
int check_rst_stat(void);
void driver_misc_init(void);
uint32_t get_random(void);
void driver_gpio_init(void);
void _mem_init(void);
uint32_t get_us_count(void);
#endif
