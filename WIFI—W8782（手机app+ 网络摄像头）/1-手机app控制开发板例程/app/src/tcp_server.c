/***********************************************************************
文件名称：TCP_Server.C
功    能：
编写时间：
编 写 人：北京智嵌物联网电子技术团队
注    意：
***********************************************************************/
#include "drivers.h"
#include "app.h"
#include "api.h"
#include "sha1.h"
#include "cfg80211.h"
#include "defs.h"
#include "type.h"
#include "types.h"


#include "lwip\sockets.h"
#include "lwip\netif.h"
#include "lwip\dns.h"
#include "lwip\api.h"
#include "lwip\tcp.h"
#include "lwip\dhcp.h"
#include "lwip\ip_addr.h"

#include "dhcpd.h"
#include "sys_misc.h"

#include "webserver.h"

#include "usr_cfg.h"
#include "test.h"

/***************开发板ip定义*************************/

char *BOARD_IP  	= "192.168.1.200";   		//开发板ip 
char *BOARD_NETMASK = "255.255.255.0";   		//开发板子网掩码
char *BOARD_WG		= "192.168.1.1";   		    //开发板子网关

#define TCP_LOCAL_PORT			1030			//即TCP服务器端口号

int tcp_server_sock_fd = -1;					//TCP服务器socket句柄
int a_new_client_sock_fd = -1;					//TCP客户端socket句柄

char tcp_recv_flag = 0;
OS_SEM  sem_tcp_rec_flag ;				//tcp接收完一桢数据信号量定义

char tcp_recv_buff[2048];						//tcp数据接收缓冲器
int tcp_recv_len = 0;							//tcp数据接收长度




/***********************************************************************
函数名称：void Task_Temperature_Send_Stats(void *pdata)
功    能：检测client_sock的状态，并向已经连接的客户端发送温湿度,每秒1次
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_Temperature_Send_Stats(void *pdata)
{
	DHT11_Data_TypeDef data;	
	OS_ERR  os_err;
	uint8_t ret;
	
	char cStr[100];
	
	while(1)
	{
	//OSSemPend(&sem_tcp_rec_flag,0,OS_OPT_PEND_BLOCKING,0,&os_err);				//持续等待sem_tcp_rec_flag信号量有效
    OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                  OS_OPT_TIME_DLY,
                  &os_err); 

		ret = Read_DHT11(&data);
		if(ret ==1 )
		{

			sprintf(cStr,"temp = %d.%d,hum=%d.%d",data.temp_int,data.temp_deci,data.humi_int,data.humi_deci);

			TCP_Send_Data(a_new_client_sock_fd,cStr,strlen(cStr),MSG_DONTWAIT);//将数据原样返回
		

		}
		else
		{

		}
	}
	
//	struct lwip_sock *sock;
//	float f_adc_value = 0;
//	DHT11_T tDHT;
//	uint8_t ret;
//	unsigned char str[] = "DHT11温度：35°C;湿度：30%;CPU温度：50.12°C\r\n<br>";//加上<br>是为了可以在网页上换行显示
//	int i = 0;
//	while(1)
//	{
//		f_adc_value = (ADC_RCVTab[0] * 3.3 / 4095 - 0.76) / 0.0025 + 25;//CPU温度转换
//		Float_To_Str(f_adc_value,&str[36]);			//将一个float型数据转成字符串	
//			
//		ret = DHT11_ReadData(&tDHT);
//		if (ret == 1)
//		{
//			str[11] = tDHT.Temp / 10 + '0';
//			str[12] = tDHT.Temp % 10 + '0';
//			str[23] = tDHT.Hum / 10 + '0';
//			str[24] = tDHT.Hum % 10 + '0';
//		}			
//		else
//		{
//			printf("\r\n未发现DHT11温湿度传感器\r\n");
//		}
//		for(i = 0;i < MAX_TEMPER_CLIENT_NUM;i ++)//最多连接的客户端数MAX_TEMPER_CLIENT_NUM
//		{		
//			sock = get_socket(client_sock[i]);
//			if(sock->conn->pcb.tcp->state == ESTABLISHED)//有连接
//			{	
//				TCP_Send_Data(client_sock[i],str,strlen(str),MSG_DONTWAIT);//向该客户端发送温度
//			}
//			else						//客户端断开了连接
//			{
//				client_sock[i] = -1;	//将该索引号置为无效
//			}
//		}
//		OSTimeDlyHMSM(0, 0, 1, 0);		//挂起1s
//	}
}

/***********************************************************************
函数名称：void ZQWL_W8782_IRQ_Handler(int socket, uint8_t *buff, int size)
功    能：网络数据处理函数，当有网络数据时，该函数被自动调用，网络的所有数据均在这里处理
输入参数：s为socket号，buff数据指针，size是数据长度 remote_addr 远程地址信息
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void ZQWL_W8782_IRQ_Handler(int s, uint8_t *buff, int size)
{
	struct lwip_sock *sock;
  OS_ERR os_err;
  
	sock = get_socket(s);
	if(sock->conn->pcb.tcp->local_port == TCP_LOCAL_PORT)//与开发板TCP端口号一致
	{
		if (sock->conn->type == NETCONN_TCP)//是TCP协议
		{
			memcpy(tcp_recv_buff,buff,size);//复制数据
			tcp_recv_flag = 1;
			tcp_recv_len = size;
			a_new_client_sock_fd = s;
			OSSemPost(&sem_tcp_rec_flag,OS_OPT_POST_ALL,&os_err);  //抛出一个信号量表示tcp已经接收完成一帧数据
		}
	}  

}
/***********************************************************************
函数名称：void Task_TCP_server(void *pdata)
功    能：TCP服务器的任务，在此可以实现具体功能，本例例程是将收到的数据原样返回。
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
extern void CMD_Ctrl( char *data);
void Task_TCP_Server(void *pdata)
{
	OS_ERR  os_err;
	
	tcp_server_sock_fd = test_tcp_server(TCP_LOCAL_PORT);//初始化一个TCP服务socket 端口为TCP_LOCAL_PORT并开始监听

	OSSemCreate(&sem_tcp_rec_flag,"tcp_rec_flag",0,&os_err); 						//创建一个信号量,
  
  while(1)
	{
		OSSemPend(&sem_tcp_rec_flag,0,OS_OPT_PEND_BLOCKING,0,&os_err);				//持续等待sem_tcp_rec_flag信号量有效

		CMD_Ctrl(tcp_recv_buff);//增加一个简单的手机控制LED的实例
    
	}
}
