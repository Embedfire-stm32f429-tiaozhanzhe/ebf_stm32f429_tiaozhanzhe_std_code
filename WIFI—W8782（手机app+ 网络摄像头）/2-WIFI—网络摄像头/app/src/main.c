/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   WIFI 摄像头例程
  ******************************************************************************
  * @attention
  *（实验前请先修改本文件的ssid及pwd变量，分别为要连接的路由名及密码）
	*	tcp连接端口号为：4800
	*
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */



#define DEBUG

#include "drivers.h"  
#include "app.h"
#include "api.h"

#include "test.h"
#include "dhcpd.h"
#include "webserver.h"
#include <cctype>

/*
* 版本说明在doc目录
*/
#define VIRSION		"V1.8"



//实验前请先修改本文件的ssid及pwd变量，分别为要连接的路由名及密码
//tcp 端口号默认为1030
char ssid[]="A305";
char pwd[]="wildfire";


void main_thread(void *pdata)
{
	int ret;
  OS_ERR   err;

	
	#ifdef DEBUG
	RCC_ClocksTypeDef RCC_ClocksStatus;
	#endif
	
	driver_misc_init(); //初始化一些杂项(驱动相关)
	usr_gpio_init(); //初始化GPIO

	OSStatTaskCPUUsageInit((OS_ERR*)&ret);

	Debug_USART_Config(); //初始化串口1
	
	//打印MCU总线时钟
#ifdef DEBUG
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	p_dbg("SYSCLK_Frequency:%d,HCLK_Frequency:%d,PCLK1_Frequency:%d,PCLK2_Frequency:%d,ADCCLK_Frequency:%d\n", 
		RCC_ClocksStatus.SYSCLK_Frequency, 
		RCC_ClocksStatus.HCLK_Frequency, 
		RCC_ClocksStatus.PCLK1_Frequency, 
		RCC_ClocksStatus.PCLK2_Frequency, 
		0);
#endif

	w25q128_init(); //初始化SPI-flash

	init_work_thread();	//初始化工作线程	
	load_web_cfg(&web_cfg);
		
	init_lwip(); //初始化lwip协议栈
	
	ret = init_wifi();//初始化WIFI芯片
	if(ret == 0)
	{
		init_monitor(); //初始化monitor模块,必须在init_wifi之后调用
		lwip_netif_init();
		init_udhcpd(); //初始化dhcp服务器
		enable_dhcp_server(); // 开启dhcp服务器,如果工作在sta模式,可以不开启dhcpserver

	}else
		p_err("init wifi faild!"); 


	misc_init(); //初始化一些杂项(app相关)

	init_key_dev();
	
	open_camera();

	p_dbg("code version:%s", VIRSION);
	p_dbg("startup time:%d.%d S", os_time_get() / 1000, os_time_get() % 1000);
	
#if	SUPPORT_WEBSERVER
	web_server_init();
#endif
//	test_power_save_enable();  //如果想降低wifi模块功耗，请执行这里
	
	//创建服务器，监听tcp端口连接。
	test_tcp_server();
	
	print_help();
	
	
	Wifi_Connect(ssid,pwd);//连接AP（路由），参数为帐号、密码

//	work_process();

	//专门用于处理各种事件的任务
  thread_create(Task_Event_Process, 0, TASK_MAIN_PRIO, 0, 512, "Task_Event_Process");

	
	while (1)//在此可以指定一个CPU运行指示灯闪烁，监视系统运行状态
	{

    OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                  OS_OPT_TIME_DLY,
                  &err); 
	}
}

int main(void)
{

	OS_ERR err;
	OSInit(&err);

	_mem_init(); //初始化内存分配
	msg_q_init();//
	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");

	OSStart(&err);


	return 0;
}
