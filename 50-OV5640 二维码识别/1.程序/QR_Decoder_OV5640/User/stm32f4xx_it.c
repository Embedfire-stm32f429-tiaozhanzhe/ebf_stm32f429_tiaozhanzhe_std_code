/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "bsp_ov5640.h"
#include "bsp_SysTick.h"
#include "bsp_debug_usart.h"
#include "qr_decoder_user.h"
extern unsigned int Task_Delay[];

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
//	printf("\r\n hardfault!");
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	uint8_t i;
	
  TimingDelay_Decrement();
	
	for(i=0;i<NumOfTask;i++)
	{
		if(Task_Delay[i])
		{
			Task_Delay[i]--;
		}
	}
}



/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

extern uint8_t fps;

//记录传输了多少行
static uint16_t line_num =0;


void DMA2_Stream1_IRQHandler(void)
{
  if(  DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) == SET )    
  {
		/*行计数*/
		line_num++;

    if(line_num==LCD_PIXEL_HEIGHT)
		{
			/*传输完一帧,计数复位*/
			line_num=0;
		}		
		/*DMA 一行一行传输*/
    OV5640_DMA_Config(FSMC_LCD_ADDRESS+(LCD_PIXEL_WIDTH*2*(line_num)),LCD_PIXEL_WIDTH*2/4);
//    dcmi_irq_hsyn();
    DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
	}
}



//使用帧中断重置line_num,可防止有时掉数据的时候DMA传送行数出现偏移
void DCMI_IRQHandler(void)
{
     /*判断帧中断标志位是否被置位*/
	if(  DCMI_GetITStatus (DCMI_IT_FRAME) == SET )    
	{
		/*传输完一帧，计数复位*/
		line_num=0;
        /*停止采集*/
        DCMI_CaptureCmd(DISABLE);
        /*获取一帧图片，FSMC_LCD_ADDRESS为存放图片的首地址*/
        /*LCD_PIXEL_WIDTH为图片宽度，LCD_PIXEL_HEIGHT为图片高度*/
		get_image(FSMC_LCD_ADDRESS,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT);
        /*绘制扫描窗口里边的扫描线，放在这里主要是避免屏幕闪烁*/
        LCD_Line_Scan_ARGB8888();
        /*重新开始采集*/
        DCMI_CaptureCmd(ENABLE);
        /*清除帧中断标志位*/
		DCMI_ClearITPendingBit(DCMI_IT_FRAME); 
	}

}

void USART1_IRQHandler(void)
{
	USART1_IRQ();
}

extern void Time2_IRQ();
void TIM2_IRQHandler(void)
{
  	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  	{
		Time2_IRQ();
    	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  	}
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
