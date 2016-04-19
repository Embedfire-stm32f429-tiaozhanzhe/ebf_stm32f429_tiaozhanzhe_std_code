/***********************************************************************
文件名称：LED_Ctrl.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：

***********************************************************************/
#include "drivers.h"
#include "app.h"

#include "lwip\sockets.h"

/***********************************************************************
函数名称：void LED_Ctrl(unsigned char *data)
功    能：根据data的数据命令控制led的亮灭
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
			命令：	led1_on				LED1灯亮
					led2_on				LED2灯亮
					led3_on				LED3灯亮
					led4_on				LED4灯亮
									
					led1_off			LED1灯灭
					led2_off			LED2灯灭
					led3_off			LED3灯灭
					led4_off			LED4灯灭
***********************************************************************/
extern int a_new_client_sock_fd ;					//TCP客户端socket句柄
extern int TCP_Send_Data(int s,char *data,int len,int flags);

void CMD_Ctrl(char *data)
{
     
 char cStr [ 100 ] = { 0 }, cCh;
 	static uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;
	static DHT11_Data_TypeDef dht11_data;	


  char * pCh, * pCh1;
 	uint8_t ucLen;

  if ( ( pCh = strstr ( (const char*)data, "CMD_LED_" ) ) != 0 ) //LED灯命令 CMD_LED_2_1 控制LED2亮
  {
    cCh = * ( pCh + 8 );
    
    switch ( cCh )
    {
      case '1':
        cCh = * ( pCh + 10 );
        switch ( cCh )
        {
          case '0':
            IND1_OFF;
            ucLed1Status = 0;
            break;
          case '1':
            IND1_ON ;
            ucLed1Status = 1;
            break;
          default :
            break;
        }
        break;
        
      case '2':
        cCh = * ( pCh + 10 );
        switch ( cCh )
        {
          case '0':
            IND2_OFF ;
            ucLed2Status = 0;
            break;
          case '1':
            IND2_ON ;
            ucLed2Status = 1;
            break;
          default :
            break;
        }
        break;

      case '3':
        cCh = * ( pCh + 10 );
        switch ( cCh )
        {
          case '0':
            IND3_OFF;
            ucLed3Status = 0;
            break;
          case '1':
            IND3_ON;
            ucLed3Status = 1;
            break;
          default :
            break;
        }
        break;
        
      default :
        break;					
        
    }
		
		//执行命令后需要返回状态
		sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_END", ucLed1Status, ucLed2Status, ucLed3Status );

		
	 }
	
	else if ( ( pCh = strstr (  (const char*)data, "CMD_BUZZER_" ) ) != 0 ) //控制蜂鸣器命令 CMD_BUZZER_1 蜂鸣器响
	{
		cCh = * ( pCh + 11 );
		
		switch ( cCh )
		{
			case '0':
				BEEP_OFF ();
				ucBuzzerStatus = 0;
				break;
			case '1':
				BEEP_ON ();
				ucBuzzerStatus = 1;
				break;
			default:
				break;
		}
		
		//执行命令后需要返回状态
		sprintf ( cStr, "CMD_BUZZER_%d_ENDBUZZER_END", ucBuzzerStatus );
		
	}
	else if ( ( ( pCh  = strstr ( (const char*)data, "CMD_UART_" ) ) != 0 ) && 
					( ( pCh1 = strstr ( (const char*)data, "_ENDUART_END" ) )  != 0 ) ) 
	{
		if ( pCh < pCh1)
		{
			ucLen = pCh1 - pCh + 12;
			memcpy ( cStr, pCh, ucLen );
			cStr [ ucLen ] = '\0';
		}
	}
	else if ( strstr ( (const char*)data,  "CMD_READ_ALL_END" ) != 0 ) //APP请求所有传感器状态
	{
		Read_DHT11 ( & dht11_data );
		sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_DHT11_%d.%d_%d.%d_ENDDHT11_BUZZER_%d_ENDBUZZER_END", 
						ucLed1Status, ucLed2Status, ucLed3Status, dht11_data .temp_int, 
						dht11_data .temp_deci, dht11_data .humi_int, dht11_data .humi_deci,
						ucBuzzerStatus );
	}
	
	
	TCP_Send_Data(a_new_client_sock_fd,cStr,strlen(cStr),MSG_DONTWAIT);//将数据原样返回



}


