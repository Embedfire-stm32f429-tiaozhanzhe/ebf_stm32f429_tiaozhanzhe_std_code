#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE       /* LSE used as RTC source clock */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI used as RTC source clock. The RTC Clock   */
                                   /*   may varies due to LSI frequency dispersion. */ 
typedef struct  
{
  RTC_TimeTypeDef RTC_Time;
  RTC_DateTypeDef RTC_Date;  
}RTC_TIME;                                   

uint8_t RTC_CheckAndConfig(RTC_TIME *rtc_time);

#endif // __RTC_H__
