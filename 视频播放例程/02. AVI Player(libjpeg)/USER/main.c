/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/**
  ******************************************************************************
  *                              头文件
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "ff.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/wm8978/bsp_wm8978.h" 
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT result; 
/**
  ******************************************************************************
  *                                任务函数
  ******************************************************************************
  */
extern void AVI_play(char *filename);

/*
*********************************************************************************************************
* Description : 初始化BL8782 wifi模块使能引脚，并关闭使能
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOB,GPIO_Pin_13);  //禁用WiFi模块
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 初始化LED */
	LED_GPIO_Config();
	
  BL8782_PDN_INIT();
  
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();
	
  printf("\r\n this is a fatfs test demo \r\n");
/*************************   flash 文件系统   *********************************************/
	//在外部Flash挂载文件系统
	result = f_mount(&fs,"0:",1);
	printf("\r\n f_mount res_flash=%d \r\n",result);
	
	/*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);

	LCD_SetLayer(LCD_BACKGROUND_LAYER);//LCD_BACKGROUND_LAYER LCD_FOREGROUND_LAYER
  
	/* 整屏清为白色 */
  LCD_Clear(LCD_COLOR_BLACK);
  LCD_SetBackColor(LCD_COLOR_BLACK);
  LCD_SetTextColor(LCD_COLOR_WHITE); 

	/* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
	if (wm8978_Init()==0)
	{
		LCD_DisplayStringLine(0,(uint8_t *)"Don't Find WM8978!!!");
		while (1);	/* 停机 */
	}
	while(1)
	{
    AVI_play("0:/Thank you.avi");
    LCD_Clear(LCD_COLOR_BLACK);
    AVI_play("0:/xiao cheng da shi.avi");
    LCD_Clear(LCD_COLOR_BLACK);    
	}

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
