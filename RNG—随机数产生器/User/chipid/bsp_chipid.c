/**
  ******************************************************************************
  * @file    bsp_beep.c
  * @author  fire
  * @version V3.0
  * @date    2015-xx-xx
  * @brief   获取chipid 及片上FLASH大小的应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32  F429开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

//获取CPU的ID函数，每个芯片都有唯一的 96_bit unique ID         

#include "./chipid/bsp_chipid.h"



/*
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID
 * 输入  ：chip_id:存储芯片ID的结构体
 * 输出  ：无
 */
void Get_ChipID(CHIP_ID_TypeDef* chip_id)  
{
		
	chip_id->ID_H = CHIP_ID->ID_H;
	chip_id->ID_M = CHIP_ID->ID_M; 
	chip_id->ID_L = CHIP_ID->ID_L;	
	
//	printf("\r\nchipID=0x%08x%08x%08x", CHIP_ID->ID_H,CHIP_ID->ID_M,CHIP_ID->ID_L);	

}


/*
 * 函数名：Get_ChipFlashSize
 * 描述  ：获取芯片FLASH大小，单位为KB
 * 输入  ：无
 * 输出  ：芯片FLASH大小，单位为KB
 */
uint32_t Get_ChipFlashSize(void) 
{
	//低16位为FLASH 大小
	return (CHIP_FLASH_SIZE &0xFFFF); 
}

/*********************************************END OF FILE**********************/

