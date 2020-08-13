/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   基于OV5640摄像头二维码扫描例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32  F429开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./camera/bsp_ov5640.h"
#include "./systick/bsp_SysTick.h"
#include "./camera/ov5640_AF.h"
#include "qr_decoder_user.h"
#include "./beep/bsp_beep.h"  
/*简单任务管理*/
uint32_t Task_Delay[NumOfTask];

uint8_t dispBuf[100];
OV5640_IDTypeDef OV5640_Camera_ID;

uint8_t beep_on_flag = 0;//蜂鸣器状态，1表示蜂鸣器响，0表示蜂鸣器不响
/**
  * @brief  TIM2产生10ms时基初始化函数
  * @param  
  * @param  
  * @note 	
  */
void Time2_init()
{	 
  	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	
  	TIM_TimeBaseStructure.TIM_Period = 10000;	//10000us=10ms
  	TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//中断使能
  	TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}
/**
  * @brief  TIM4_IRQHandler:10ms时基中断函数
  * @param  
  * @param  
  * @note 	
  */
void Time2_IRQ()
{
    static u32 BeepTime=8;
         
    if(beep_on_flag)   
    {
	  BEEP_ON;
      if((--BeepTime) == 0)
      {
        BeepTime=8;
        beep_on_flag =0;
        BEEP_OFF;  
      }  
    }
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    char  qr_type_len=0;
    short qr_data_len=0;
    char qr_type_buf[10];
    char qr_data_buf[512];
    int addr=0;
    int i=0,j=0;
    char qr_num=0;
	/*摄像头与RGB LED灯共用引脚，不要同时使用LED和摄像头*/
	
  Debug_USART_Config();   
	
	/* 配置SysTick 为10us中断一次,时间到后触发定时中断，
	*进入stm32fxx_it.c文件的SysTick_Handler处理，通过数中断次数计时
	*/
	SysTick_Init();

    BEEP_GPIO_Config();
	/*初始化液晶屏*/
    LCD_Init();
    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
    LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
   LCD_SetTransparency(0xFF);
   LCD_Clear_ARGB8888(LCD_COLOR_BLACK_ARGB8888); 
   //绘制透明框
   LCD_SetTextColor_ARGB8888(TRANSPARENCY_ARGB8888);
   LCD_DrawFullRect_ARGB8888(Frame_width,Frame_width);
   //绘制扫描框
   LCD_View_Finder_ARGB8888(Frame_width,Frame_line_length,Frame_line_size,LCD_COLOR_GREEN_ARGB8888);

  CAMERA_DEBUG("STM32F429 二维码解码例程");
	

  /* 初始化摄像头GPIO及IIC */
  OV5640_HW_Init();   

  /* 读取摄像头芯片ID，确定摄像头正常连接 */
  OV5640_ReadID(&OV5640_Camera_ID);

   if(OV5640_Camera_ID.PIDH  == 0x56)
  {
//    sprintf((char*)dispBuf, "              OV5640 摄像头,ID:0x%x", OV5640_Camera_ID.PIDH);
//		LCD_DisplayStringLine_EN_CH(LINE(0),(uint8_t*)dispBuf);
    CAMERA_DEBUG("%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);

  }
  else
  {
		LCD_SetLayer(LCD_BACKGROUND_LAYER); 

    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t*) "         没有检测到OV5640，请重新检查连接。");
    CAMERA_DEBUG("没有检测到OV5640摄像头，请重新检查连接。");

    while(1);  
  }

  
  OV5640_Init();
  
  OV5640_RGB565Config();
  OV5640_AUTO_FOCUS();
  
  //使能DCMI采集数据
  //DMA直接传输摄像头数据到LCD屏幕显示
  DCMI_Cmd(ENABLE); 
  DCMI_CaptureCmd(ENABLE);
  
  Time2_init();

  
  while(1)
	{
      //二维码识别，返回识别条码的个数
      qr_num = QR_decoder();
       
       if(qr_num)
       {
           //识别成功，蜂鸣器响标志
           beep_on_flag =1;
           
           //解码的数据是按照识别条码的个数封装好的二维数组，这些数据需要
           //根据识别条码的个数，按组解包并通过串口发送到上位机串口终端
           for(i=0;i < qr_num;i++)
           {
               qr_type_len = decoded_buf[i][addr++];//获取解码类型长度
               
               for(j=0;j < qr_type_len;j++)
                   qr_type_buf[j]=decoded_buf[i][addr++];//获取解码类型名称
               
               qr_data_len  = decoded_buf[i][addr++]<<8; //获取解码数据长度高8位
               qr_data_len |= decoded_buf[i][addr++];    //获取解码数据长度低8位
               
               for(j=0;j < qr_data_len;j++)
                   qr_data_buf[j]=decoded_buf[i][addr++];//获取解码数据
               
               uart_send_buf((unsigned char *)qr_type_buf, qr_type_len);//串口发送解码类型
               while(get_send_sta()); //等待串口发送完毕
               uart_send_buf((unsigned char *)":", 1);    //串口发送分隔符
               while(get_send_sta()); //等待串口发送完毕
               uart_send_buf((unsigned char *)qr_data_buf, qr_data_len);//串口发送解码数据
               while(get_send_sta()); //等待串口发送完毕
               uart_send_buf((unsigned char *)"\r\n", 2); //串口发送分隔符
               while(get_send_sta());//等待串口发送完毕
               addr =0;//清零
           }
               
       } 
		
	}

}


/*********************************************END OF FILE**********************/

