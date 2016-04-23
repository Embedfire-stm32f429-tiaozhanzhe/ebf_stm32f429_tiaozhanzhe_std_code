/**
  ******************************************************************************
  * @file    httpserver-socket.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013  
  * @brief   Basic http server implementation using LwIP socket API   
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
  
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "string.h"
#include "speedtest.h"
#include "./sdram/bsp_sdram.h"
#include <stdlib.h>
#define TCPCLIENT_SPEEDTEST_PRIO 6

static char tcp_remote_ip[16] = "192.168.1.106"; /*remote ip address*/
static int tcp_remote_port = 5000;               /*remote port*/
char test[1024] __EXRAM;
/*-----------------------------------------------------------------------------------*/
static void tcpClientSpeedTest_thread(void *arg)
{
  int sock;
  struct sockaddr_in address;
	char *buf =NULL;
	int optval = 1; 

//	SDRAM_Test();
 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
  {
    printf("can not create socket");
    return;
  }
	printf("\r\nsocket success");
  
//	lwip_setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval)); 
	
  /* bind to port  */
  address.sin_family = AF_INET;
  address.sin_port = htons(tcp_remote_port);
  address.sin_addr.s_addr = inet_addr(tcp_remote_ip);
	
  /* listen for incoming connections (TCP listen backlog = 5) */
  if(connect(sock, (struct sockaddr*)&address,sizeof(address)) == -1)
	{
		printf("\r\ncan not connect to Server");
		close(sock);
	}
	  	printf("\r\nconnect success");

  
	buf = test;//(char *)malloc(sizeof(char) * 1024);//test;//
	if(buf ==NULL)
		printf("\r\n buf null");
	memset(buf, 't', 1024);
  
  while (1) 
  {
		if( send(sock, (const uint8_t *)buf, 1024,MSG_WAITALL) == -1)
		{
				printf("\r\n send error");
       break;
		}
		
//		  ret = read(conn, recv_buffer, buflen); 

  }
	
	while(1)
	{
		printf("\r\nwhile error");
	}
}


/**
  * @brief  Initialize the HTTP server (start its thread) 
  * @param  none
  * @retval None
  */
void speedTest_init()
{
  sys_thread_new("HTTP", tcpClientSpeedTest_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, TCPCLIENT_SPEEDTEST_PRIO);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
