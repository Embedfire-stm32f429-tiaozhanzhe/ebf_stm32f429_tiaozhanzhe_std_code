#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

#include "test.h"
#include "dhcpd.h"
#include "webserver.h"
#include <cctype>


char command = 0;

uint8_t test_ir_data[6] = {0xb2, 0x4d, 0x3f, 0xc0, 0x40, 0xbf};

#define help_string   \
"测试功能:\r\n"\
"	0:基本信息\r\n"\
"	1:扫描\r\n"\
"	2:连接WIFI\r\n"\
"	3:关闭WIFI连接\r\n"\
"	4:创建AP\r\n"\
"	5:连接TCP\r\n"\
"	6:关闭TCP连接\r\n"\
"	7:send发送\r\n"\
"	8:建立TCP服务器\r\n"\
"	9:关闭TCP服务器\r\n"\
"	a:启用音频功能\r\n"\
"	b:关闭音频功能\r\n"\
"	c:dns测试\r\n"\
"	d:自动获取IP地址\r\n"\
"	e:打印内存信息\r\n"\
"	f:获取wifi连接信息\r\n"\
"	g:连接UDP\r\n"\
"	h:加入多播组\r\n"\
"	i:建立UDP服务器\r\n"\
"	j:sendto发送\r\n"\
"	k:切换音频模式(mp3/pcm)\r\n"\
"	l:调节音量\r\n"\
"	m:DEBUG输出\r\n"\
"	n:关闭AP\r\n"\
"	o:获取sta信息\r\n"\
"	p:加入adhoc网络\r\n"\
"	q:退出adhoc网络\r\n"\
"	r:使能省电模式\r\n"\
"	s:关闭省电模式\r\n"\
"	t:monitor输出\r\n"\
"	u:打开摄像头\r\n"\
"	v:关闭摄像头\r\n"\
"	w:切换摄像头分辨率\r\n"\
"	x:重启\r\n"\
"	y:测试machtalk远程控制\r\n"\
"	z:将88W8782固件烧至SPI flash\r\n"\
"	?:帮助\r\n"\
"请输入上面的命令选择测试的功能:"


void print_help()
{
	p_dbg(help_string);
}

extern int test_create_adhoc(void);
void handle_cmd(char cmd)
{
	cmd = tolower(cmd);
	switch (cmd)
	{
		case '0':
			show_sys_info(p_netif);
			break;
		case '1':
			test_scan();
			break;
		case '2':
			test_wifi_connect();
			break;
		case '3':
			test_wifi_disconnect();
			break;
		case '4':
			test_create_ap();
			break;
		case '5':
			test_tcp_link();
			break;
		case '6':
			test_tcp_unlink();
			break;
		case '7':
			test_send("this is test data");
			break;
		case '8':
			test_tcp_server();
			break;
		case '9':
			test_close_tcp_server();
			break;
		case 'a':
			test_auto_get_ip(p_netif);
			break;
		case 'b':
			test_auto_get_ip(p_eth_netif);
			break;
		case 'c':
			test_dns("www.baidu.com");
			break;
		case 'd':
			show_sys_info(p_eth_netif);
			break;
		case 'e':
#if USE_MEM_DEBUG	
			mem_slide_check(1);
#endif
			break;
		case 'f':
			test_wifi_get_stats();
			break;
		case 'g':
			test_udp_link();
			break;
		case 'h':
			test_multicast_join();
			break;
		case 'i':
			test_udp_server();
			break;
		case 'j':
			test_sendto("this is test data");
			break;
#if SUPPORT_AUDIO
		case 'k':
			//switch_audio_mode();
			break;
		case 'l':
//			if(audio_cfg.volume > 0)
//				audio_cfg.volume -= 10;
//			else
//				audio_cfg.volume = 100;
//			p_dbg("当前音量:%d", audio_cfg.volume);
			break;
#endif
		case 'm':
			switch_dbg();
			break;
		case 'n':
			test_stop_ap();
			break;
		case 'o':
			test_get_station_list();
			break;
		case 'p':
			test_wifi_join_adhoc();
			break;
		case 'q':
			test_wifi_leave_adhoc();
			break;
		case 'r':
			test_power_save_enable();
			break;
		case 's':
			test_power_save_disable();
			break;
		case 't':
			monitor_switch();
			break;
		case 'u':
			open_camera();
			break;
		case 'v':
			close_camera();
			break;
		case 'w':
			switch_jpeg_size();
			break;
		case 'x':
			soft_reset();
			//test_mqtt_pub();
			break;
		case 'y':
			//st_connect_to_machtalk_server();
			//test_pub_mqtt_connect();
			test_write_firmware_to_spi_flash();
			break;
		case 'z':
//			RTC_Time_Set(15, 10, 1, 4, 22, 03, 00);
			//test_sub_mqtt_connect();
			
			break;
		case '?':
			print_help();
			break;
		default:
			p_err("unkown cmd");
			break;
	}
}
