/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RTC例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/RTC/bsp_rtc.h"
#include "./Bsp/usart/bsp_debug_usart.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	// 初始化调试串口
	Debug_USART_Config();
	printf("\r\n RTC 实时时钟实验\r\n");
  
  // 判断是否发生上电/外部复位引脚 复位
//  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
//  {
//		printf("\r\n RTC 初始化\r\n");
//		RTC_Config();
//		RTC_TimeAndDate_Init();
//  }
//  else
//  {
//    // 判断是不是上电复位
//    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//    {
//      printf("\r\n 发生上电复位\r\n");
//    }
//    // 判断是不是 外部复位引脚 复位
//    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//    {
//      printf("\r\n 发生外部pin复位\r\n");
//    }

//    printf("\r\n 不用重新配置RTC....\r\n");
//    
//    // 使能PWR时钟
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    // PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问
//    PWR_BackupAccessCmd(ENABLE);
//    // 等待 RTC APB 寄存器同步
//    RTC_WaitForSynchro();		
//		
//  }
		RTC_Config();
		RTC_TimeAndDate_Init();
	// 显示时间和日期
	RTC_TimeAndDate_Show();
}




