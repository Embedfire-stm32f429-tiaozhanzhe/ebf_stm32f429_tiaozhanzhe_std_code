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
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/RTC/bsp_rtc.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/beep/bsp_beep.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{  
	/* 初始化LED */
	LED_GPIO_Config();
	BEEP_GPIO_Config();

	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	printf("\n\r这是一个RTC闹钟实验 \r\n");
	
	/*=========================液晶初始化开始===============================*/
	LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/		
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
  /*=========================液晶初始化结束===============================*/
	
	/*
	 * 当我们配置过RTC时间之后就往备份寄存器0写入一个数据做标记
	 * 所以每次程序重新运行的时候就通过检测备份寄存器0的值来判断
	 * RTC 是否已经配置过，如果配置过那就继续运行，如果没有配置过
	 * 就初始化RTC，配置RTC的时间。
	 */
  if (RTC_ReadBackupRegister(RTC_BKP_DRX) != 0X32F3)
  {
    /* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
    RTC_CLK_Config();
		
				/* 闹钟设置 */
		RTC_AlarmSet();
		
    /* 设置时间和日期 */
		RTC_TimeAndDate_Set();
		

  }
  else
  {
    /* 检查是否电源复位 */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n 发生电源复位....\r\n");
    }
    /* 检查是否外部复位 */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n 发生外部复位....\r\n");
    }

    printf("\r\n 不需要重新配置RTC....\r\n");
    
    /* 使能 PWR 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();

    /* 清除RTC中断标志位 */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    /* 清除 EXTI Line 17 悬起位 (内部连接到RTC Alarm) */
    EXTI_ClearITPendingBit(EXTI_Line17);		
  } 
	
	/* 显示时间和日期 */
	RTC_TimeAndDate_Show();
}


/**********************************END OF FILE*************************************/
