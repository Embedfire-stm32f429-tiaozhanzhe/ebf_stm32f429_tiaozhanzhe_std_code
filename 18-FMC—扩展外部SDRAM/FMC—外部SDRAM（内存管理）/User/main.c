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
#include "./malloc/malloc.h"
#include <stdlib.h>



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	u8 *p=0;
	u8 sramx=0;				//默认为内部sram
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
    
  /* 初始化串口 */
  Debug_USART_Config();
  
  printf("\r\n 野火STM32F429 SDRAM 读写测试例程\r\n");
  
  /*初始化SDRAM模块*/
  SDRAM_Init();
  
 	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池

	 
	printf ( "\r\n野火内存管理\r\n" );

	 
	printf ( "\r\n分配内存\r\n" );
	 
  p = mymalloc ( sramx, 1024 *2 );//申请2K字节
	
	sprintf((char*)p,"Memory Malloc");//使用申请空间存储内容
	
	printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//显示内部内存使用率
	printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//显示外部内存使用率

	myfree(sramx,p);//释放内存
	p=0;		      	//指向空地址

  printf ( "\r\n释放内存\r\n" );
	printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//显示内部内存使用率
	printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//显示外部内存使用率
	
	
  while(1);
	

}


/*********************************************END OF FILE**********************/

