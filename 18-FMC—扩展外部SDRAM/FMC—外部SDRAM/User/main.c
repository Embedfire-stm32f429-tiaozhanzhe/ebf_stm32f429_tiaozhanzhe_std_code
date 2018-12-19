/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   读写SDRAM
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include <stdlib.h>


void Delay(__IO u32 nCount); 

/*绝对定位方式访问SDRAM,这种方式必须定义成全局变量*/
uint8_t testValue __attribute__((at(SDRAM_BANK_ADDR)));

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
    
  /* 初始化串口 */
  Debug_USART_Config();
  
  printf("\r\n 野火STM32F429 SDRAM 读写测试例程\r\n");
  
  /*初始化SDRAM模块*/
  SDRAM_Init();
  
  /*蓝灯亮，表示正在读写SDRAM测试*/
  LED_BLUE;
  
  /*对SDRAM进行读写测试，检测SDRAM是否正常*/
  if(SDRAM_Test()==1)
  {
		//测试正常 绿灯亮
    LED_GREEN;			  
  }
	else
	{
		//测试失败 红灯亮
		LED_RED;
	}
	
	/*指针方式访问SDRAM*/
	{	
	 uint32_t temp;
	
	 printf("\r\n指针方式访问SDRAM\r\n");
	/*向SDRAM写入8位数据*/
	 *( uint8_t*) (SDRAM_BANK_ADDR ) = (uint8_t)0xAA;
	 printf("\r\n指针访问SDRAM，写入数据0xAA \r\n");

	 /*从SDRAM读取数据*/
	 temp =  *( uint8_t*) (SDRAM_BANK_ADDR );
	 printf("读取数据：0x%X \r\n",temp);

	 /*写/读 16位数据*/
	 *( uint16_t*) (SDRAM_BANK_ADDR+10 ) = (uint16_t)0xBBBB;
	 printf("指针访问SDRAM，写入数据0xBBBB \r\n");
	 
	 temp =  *( uint16_t*) (SDRAM_BANK_ADDR+10 );
	 printf("读取数据：0x%X \r\n",temp);


	 /*写/读 32位数据*/
	 *( uint32_t*) (SDRAM_BANK_ADDR+20 ) = (uint32_t)0xCCCCCCCC;
	 printf("指针访问SDRAM，写入数据0xCCCCCCCC \r\n");	 
	 temp =  *( uint32_t*) (SDRAM_BANK_ADDR+20 );
	 printf("读取数据：0x%X \r\n",temp);

	}
	
	/*绝对定位方式访问SDRAM,这种方式必须定义成全局变量*/
	{
		testValue = 0xDD;
		printf("\r\n绝对定位访问SDRAM，写入数据0xDD,读出数据0x%X,变量地址为%X\r\n",testValue,(uint32_t )&testValue);	 
	}
	
	

  while(1); 
 

}


void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

