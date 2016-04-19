#ifndef __CHIPID_H
#define	__CHIPID_H


#include "stm32f4xx.h"


typedef struct
{
  __IO uint32_t ID_H;   	//¸ß×Ö½Ú
  __IO uint32_t ID_M;  
  __IO uint32_t ID_L; 		//µÍ×Ö½Ú
} CHIP_ID_TypeDef;

#define CHIP_ID	((CHIP_ID_TypeDef *)((uint32_t)0x1FFF7A10))
#define CHIP_FLASH_SIZE 				*((uint32_t*)0x1FFF7A22)

void Get_ChipID(CHIP_ID_TypeDef* chip_id);
uint32_t Get_ChipFlashSize(void) ;

#endif /* __CHIPID_H */
