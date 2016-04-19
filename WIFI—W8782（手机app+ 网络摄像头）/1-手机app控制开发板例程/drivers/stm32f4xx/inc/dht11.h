#if 0

#ifndef _DHT11_H
#define _DHT11_H

struct _dht11_data{
	uint8_t data[5];
	uint8_t valid;
};

void init_dht11(void);
void dth11_start_read(void);
int dth11_read_data(uint8_t buff[5]);

#endif

#else

#ifndef __HUMITURE_H_
#define __HUMITURE_H_

#include  "stm32f4xx.h"

#define HUMITURE_HIGH  1
#define HUMITURE_LOW   0


/*---------------------------------------*/
#define TEMP_HUM_CLK     RCC_AHB1Periph_GPIOE
#define TEMP_HUM_PIN     GPIO_Pin_2               
#define TEMP_HUM_PORT    GPIOE



#define BEEP_GPIO_CLK     RCC_AHB1Periph_GPIOI
#define BEEP_GPIO_PIN     GPIO_Pin_11               
#define BEEP_GPIO_PORT    GPIOI

#define BEEP_OFF()					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON()					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)



#define TEMP_HUM_DATA_OUT(a)	if (a)	\
                                   GPIO_SetBits(TEMP_HUM_PORT,TEMP_HUM_PIN);\
                                   else		\
                                   GPIO_ResetBits(TEMP_HUM_PORT,TEMP_HUM_PIN)

#define  TEMP_HUM_DATA_IN()	  GPIO_ReadInputDataBit(TEMP_HUM_PORT,TEMP_HUM_PIN)

typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
}DHT11_Data_TypeDef;

void TEMP_HUM_GPIO_Config(void);
uint8_t DS18B20_Init(void);
float DS18B20_Get_Temp(void);

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);

void Beep_Init ( void );

#endif //__HUMITURE_H_
#endif
