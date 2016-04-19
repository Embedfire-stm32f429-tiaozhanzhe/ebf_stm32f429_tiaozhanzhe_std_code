#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

void RTC_Config(void);
void RTC_TimeAndDate_Init(void);
void RTC_TimeAndDate_Show(void);


/*ÐÅÏ¢Êä³ö*/
#define RTC_DEBUG_ON         0

#define RTC_INFO(fmt,arg...)           printf("<<-RTC-INFO->> "fmt"\n",##arg)
#define RTC_ERROR(fmt,arg...)          printf("<<-RTC-ERROR->> "fmt"\n",##arg)
#define RTC_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-RTC-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#endif // __RTC_H__
