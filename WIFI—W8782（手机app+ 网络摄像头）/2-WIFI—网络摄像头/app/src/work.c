#define DEBUG

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

struct iot_work_struct  iot_work;

int local_udp_server = -1, local_udp_client = -1;


extern char command;
extern mutex_t socket_mutex;
extern unsigned char debug_s;

extern void handle_cmd(char cmd);
extern int set_ipaddr(struct netif *p_netif, struct ip_addr *ipaddr, struct ip_addr *netmask, struct ip_addr *gw, struct ip_addr *dns) ;

/*
 *发送事件到主线程
 *
*/
void send_work_event(uint32_t event)
{
	iot_work.event_flag |= event;
	if(iot_work.event)
		wake_up(iot_work.event);
}

/*
	将本地ip设为默认值，ap模式下
*/
void set_default_ip()
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct ip_addr dns;

	gw.addr = web_cfg.ip.gw;
	ipaddr.addr = web_cfg.ip.ip;
	netmask.addr = web_cfg.ip.msk;
	dns.addr = web_cfg.ip.dns;

	if(p_netif)
	set_ipaddr(p_netif, &ipaddr, &netmask, &gw, &dns);
}


int udp_broadcast(uint16_t port, uint8_t *p_data, int len)
{
	struct sockaddr_in addr;
	//p_dbg_enter;
	//p_dbg("enter %s\n", __FUNCTION__);
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(struct sockaddr_in);
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr("255.255.255.255");

	mutex_lock(socket_mutex);
	len = sendto(local_udp_client, p_data, len, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
	mutex_unlock(socket_mutex);
	//p_dbg("ret:%d", len);
	//p_dbg_exit;

	return len;
}

int udp_local_send(void *buff, int len)
{
	return udp_broadcast(12531, (uint8_t *)buff, len);
}

/*
 *
 *
*/
void tcp_recv_thread(void *arg);
void init_iot_work()
{
	int n = 1;
	uint8_t tmp[16];
	char *cpu_id = (char *)p_netif->hwaddr;
	int id_len = 6;
	struct bond_req req;

	memset(&iot_work, 0, sizeof(struct iot_work_struct));

	iot_work.event = init_event();
	
	local_udp_server = udp_create_server(12531);
	if(local_udp_server == -1)
		p_err_fun;
	local_udp_client = socket(AF_INET, SOCK_DGRAM, 0);
	if(local_udp_client == -1)
		p_err_fun;
	
	add_select_array(local_udp_server);
	setsockopt(local_udp_client, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(n));

	//暂时用mac地址生成sn号
	memcpy(tmp, p_netif->hwaddr, 6);
	snprintf(iot_work.self_sn.mac, 16, "%02x%02x%02x%02x%02x%02x", 
				tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);

	md5_vector(1, (const u8**)&cpu_id, (const size_t *)&id_len, tmp);
	snprintf(iot_work.self_sn.sn, 33, "%02x%02x%02x%02x%02x%02x%02x%02x"	\
			"%02x%02x%02x%02x%02x%02x%02x%02x", tmp[0], tmp[1], tmp[2], tmp[3]
			                                  , tmp[4], tmp[5], tmp[6], tmp[7]
			                                  , tmp[8], tmp[9], tmp[10], tmp[11]
			                  			       , tmp[12], tmp[13], tmp[14], tmp[15]);
	p_dbg("mac:%s,sn:%s", iot_work.self_sn.mac, iot_work.self_sn.sn);

	memset(&req, 0, sizeof(struct bond_req));
	memcpy(req.mac, iot_work.self_sn.mac, 12);
	memcpy(req.sn, iot_work.self_sn.sn, 32);

	req.sn[31] += 1; //sub tcopic
	bond_host(&req);

//	init_mqtt(&web_cfg, &iot_work);

	thread_create(tcp_recv_thread, 0, TASK_TCP_RECV_PRIO, 0, TASK_TCP_RECV_STACK_SIZE, "tcp_recv_thread");

}


void send_ctl_msg(uint8_t msg_type, uint8_t time, uint8_t fre, uint16_t card_type,  uint32_t card_id)
{
	//mqtt_pub(&ctl_pkg, sizeof(struct _ctl_pkg));
}

int bond_host(struct bond_req *req)
{
	int i;
	uint8_t empty_mac[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	for(i = 0;i < MAX_BOND_NUM; i++)
	{
		if(memcmp(web_cfg.work_cfg.bond[i].mac, req->mac, 12) == 0)
			break;
	}

	if(i != MAX_BOND_NUM) //already bond
	{
		goto sucess; //over write
	};

	for(i = 0;i < MAX_BOND_NUM; i++)
	{
		if(memcmp(web_cfg.work_cfg.bond[i].mac, empty_mac, 12) == 0)
			break;
	}

	if(i == MAX_BOND_NUM) //full
		return -2;

sucess:
	if(i < MAX_BOND_NUM)
	{
		memcpy(&web_cfg.work_cfg.bond[i], req, sizeof(struct bond_req));
		return 0;
	}

	return -1;
}

void handle_local_udp_recv(uint8_t *data, int len)
{
	struct local_pkg_head *head = (struct local_pkg_head*)data;

	if(head->dir != MSG_DIR_APP2DEV)
	{
		p_err_fun;
		return;
	}
	switch(head->msg_id)
	{
		case LOCAL_MSG_DISC_REQ:
		{
			struct disc_res *res;
			struct local_pkg_head *res_head = (struct local_pkg_head *)mem_calloc(sizeof(struct local_pkg_head) + sizeof(struct disc_res), 1);
			if(res_head)
			{
			
				res_head->dir = MSG_DIR_DEV2APP;
				res_head->msg_id = LOCAL_MSG_DISC_RES;
				res_head->data_len = sizeof(struct disc_res);
				res = (struct disc_res *)(res_head + 1);

				memcpy(res->mac, iot_work.self_sn.mac, 12);
				memcpy(res->sn, iot_work.self_sn.sn, 32);
				
				udp_local_send((void*)res_head, sizeof(struct local_pkg_head) + sizeof(struct disc_res));
			}
		}
		break;
		case LOCAL_MSG_BOND_REQ:
		if(head->data_len == sizeof(struct bond_req))
		{
			int ret;
			uint8_t res_buff[sizeof(struct local_pkg_head) + 4];
			struct local_pkg_head *res_head = (struct local_pkg_head *)res_buff;
			
			struct bond_req *req= (struct bond_req*)(head + 1);
			ret = bond_host(req);
			if(ret)
			{
				ret = 1;
			}

			res_head->dir = MSG_DIR_DEV2APP;
			res_head->msg_id = LOCAL_MSG_BOND_RES;
			res_head->data_len = 4;
			udp_local_send((void*)res_head, sizeof(struct local_pkg_head) + 4);

		}
		break;
		default:
		break;
	}
}
		

void Task_Event_Process(void *pdata)
{
	uint32_t pending_event;
  
  memset(&iot_work, 0, sizeof(struct iot_work_struct));

	iot_work.event = init_event();

	

	//send_work_event(MODE_DETECT_CHANGE_STA);	//默认sta模式	
  
	while(1)
	{
		
		//等待事件通知，1s超时
		wait_event_timeout(iot_work.event, 1000);	
		pending_event = iot_work.event_flag;
		iot_work.event_flag = 0;

		if(pending_event & MODE_DETECT_CHANGE_STA)
		{
			p_dbg("连接到:%s, 密码:%s", web_cfg.sta.essid, web_cfg.sta.key);

			IND3_OFF;
			wifi_set_mode(MODE_STATION);
			indicate_led_twink_start(1000);
			wifi_connect(web_cfg.sta.essid, web_cfg.sta.key);

			if (is_wifi_connected())
			{
				p_dbg("wifi connect ok");
				if(web_cfg.ip.auto_get)
					auto_get_ip(p_netif);
			}
		}

		if(pending_event & MODE_DETECT_CHANGE_AP)
		{
			struct ip_addr ipaddr;
			struct ip_addr netmask;
			struct ip_addr gw;
			struct ip_addr dns;
			p_dbg("建立ap:%s, 密码:%s", web_cfg.ap.essid, web_cfg.ap.key);

			IND3_ON;
			indicate_led_twink_start(200);
			wifi_set_mode(MODE_AP);
			wifi_ap_cfg(web_cfg.ap.essid, web_cfg.ap.key, KEY_WPA2, web_cfg.ap.channel, 4);

			gw.addr = web_cfg.ip.gw;
			ipaddr.addr = web_cfg.ip.ip;
			netmask.addr = web_cfg.ip.msk;
			dns.addr = web_cfg.ip.dns;
			if(p_netif)
				set_ipaddr(p_netif, &ipaddr, &netmask, &gw, &dns);
		}
		
		if(pending_event & SEND_SNESERS_EVENT)
			{
//			mqtt_pub(&sensers_pkg, sizeof(struct _sensers_pkg));
		}

		if(pending_event & TEST_EVENT)
		{
			handle_cmd(command);
		}
	}

}
		



int Wifi_Connect(char *essid, char *key)
{
    //p_dbg("连接到:%s, 密码:%s", web_cfg.sta.essid, web_cfg.sta.key);

    p_dbg("连接到:%s, 密码:%s",essid, key);

    IND3_OFF;
    wifi_set_mode(MODE_STATION);
    indicate_led_twink_start(1000);
   // wifi_connect(web_cfg.sta.essid, web_cfg.sta.key);
    wifi_connect(essid, key);

    if (is_wifi_connected())
    {
      p_dbg("wifi connect ok");
      if(web_cfg.ip.auto_get)
        auto_get_ip(p_netif);
      
      thread_create(tcp_recv_thread, 0, TASK_TCP_RECV_PRIO, 0, TASK_TCP_RECV_STACK_SIZE, "tcp_recv_thread");

      return 0;
    }
    else
      return -1;

}


/*
 *主工作线程
 *
*/
int auto_get_ip(struct netif *p_netif);
void work_process()
{
//	int ret;
	uint32_t pending_event;
	
	init_iot_work();

	send_work_event(MODE_DETECT_CHANGE_STA);	//默认sta模式
	
	while(1)
	{
		//等待事件通知，1s超时
		wait_event_timeout(iot_work.event, 1000);
		pending_event = iot_work.event_flag;
		iot_work.event_flag = 0;

		if(pending_event & MODE_DETECT_CHANGE_STA)
		{
			p_dbg("连接到:%s, 密码:%s", web_cfg.sta.essid, web_cfg.sta.key);

			IND3_OFF;
			wifi_set_mode(MODE_STATION);
			indicate_led_twink_start(1000);
			wifi_connect(web_cfg.sta.essid, web_cfg.sta.key);

			if (is_wifi_connected())
			{
				p_dbg("wifi connect ok");
				if(web_cfg.ip.auto_get)
					auto_get_ip(p_netif);
			}
		}

		if(pending_event & MODE_DETECT_CHANGE_AP)
		{
			struct ip_addr ipaddr;
			struct ip_addr netmask;
			struct ip_addr gw;
			struct ip_addr dns;
			p_dbg("建立ap:%s, 密码:%s", web_cfg.ap.essid, web_cfg.ap.key);

			IND3_ON;
			indicate_led_twink_start(200);
			wifi_set_mode(MODE_AP);
			wifi_ap_cfg(web_cfg.ap.essid, web_cfg.ap.key, KEY_WPA2, web_cfg.ap.channel, 4);

			gw.addr = web_cfg.ip.gw;
			ipaddr.addr = web_cfg.ip.ip;
			netmask.addr = web_cfg.ip.msk;
			dns.addr = web_cfg.ip.dns;
			if(p_netif)
				set_ipaddr(p_netif, &ipaddr, &netmask, &gw, &dns);
		}
		
		if(pending_event & SEND_SNESERS_EVENT)
			{
//			mqtt_pub(&sensers_pkg, sizeof(struct _sensers_pkg));
		}

		if(pending_event & TEST_EVENT)
		{
			handle_cmd(command);
		}
	}
}



int is_led_ctrl_cmd(char *buff)
{
	if ((buff[0] == 0xaa) && (buff[1] == 0x55) && (buff[2] == 1 || buff[2] == 2))
		return 1;

	return 0;
}

int is_udp_socket(int num)
{
	struct lwip_sock *sock;

	sock = get_socket(num);
	if (!sock || !sock->conn)
	{
		return 0;
	}

	if (sock->conn->type == NETCONN_UDP)
	{
		return 1;
	}

	return 0;

}

int is_dhcp_socket(int num)
{
	uint16_t tmp_port = 0;
	struct lwip_sock *sock;

	sock = get_socket(num);
	if (!sock || !sock->conn)
	{
		return 0;
	}

	if (sock->conn->type == NETCONN_UDP)
	{
		tmp_port = sock->conn->pcb.udp->local_port; //server

		if (tmp_port == DHCP_SERVER_PORT)
			return 1;
	}
	return 0;
}

int is_web_socket(int num)
{
	uint16_t tmp_port = 0;
	struct lwip_sock *sock;

	sock = get_socket(num);
	if (!sock || !sock->conn)
	{
		return 0;
	}

	if (sock->conn->type == NETCONN_TCP)
	{
		tmp_port = sock->conn->pcb.tcp->local_port; //server

		if (tmp_port == 80)
			return 1;
	}
	return 0;
}
bool select_fd[MEMP_NUM_NETCONN] = {FALSE};

int add_select_array(int s)
{
	if(s >= MEMP_NUM_NETCONN)
	{
		p_err_fun;
		return -1;
	}

	select_fd[s] = TRUE;
	
	return 0;
}

int del_select_array(int s)
{
	if(select_fd[s] == 0)
	{
		p_err_fun;
	}
	
	select_fd[s] = FALSE;

	return 0;
}

/*
 *selcet模式接收数据的例子，可以监控多个socket的数据接收
 *所有发送到开发板的udp和tcp数据都从这里读取
 */
DECLARE_MONITOR_ITEM("tcp totol recv", tcp_totol_recv);
#define TCP_RCV_SIZE 1024
extern int client_socket_fd;
extern int server_socket_fd;
extern int remote_socket_fd;

void tcp_recv_thread(void *arg)
{
	int i, size, retval, select_size;
	struct lwip_sock *sock;
	char *tcp_rcv_buff;
	struct timeval tv;
	fd_set rfds;
	
	tcp_rcv_buff = (char*)mem_malloc(TCP_RCV_SIZE + 4);
	while (1)
	{
		mutex_lock(socket_mutex);
		FD_ZERO(&rfds);

		for (i = 0; i < MEMP_NUM_NETCONN; i++)
		{
			if(select_fd[i])
			{
				sock = get_socket(i);
				if (sock && sock->conn && sock->conn->recvmbox)
				//socket关闭和接收关闭则不能接收
				{
					FD_SET(i, &rfds);
				}
			}
		}

		select_size = MEMP_NUM_NETCONN;

		mutex_unlock(socket_mutex);

		tv.tv_sec = 5;
		tv.tv_usec = 0;
		retval = select(select_size, &rfds, NULL, NULL, &tv);
		if ((retval ==  - 1) || (retval == 0))
		{
			sleep(50); //内存错误
		}
		else
		{
			if (retval)
			{
				for (i = 0; i < select_size; i++)
				{
					if (FD_ISSET(i, &rfds))
					{
						struct sockaddr remote_addr;
						mutex_lock(socket_mutex);

						#if 1	//使用recvfrom接收
						retval = sizeof(struct sockaddr);
						size = recvfrom(i, tcp_rcv_buff, TCP_RCV_SIZE, MSG_DONTWAIT, &remote_addr, (socklen_t*) &retval);
						/*	
						p_dbg("rcv from:%d,%d,%d,%d,%d,%d\n",udp_remote_addr.sa_data[0],remote_addr.sa_data[1],
						remote_addr.sa_data[2],
						remote_addr.sa_data[3],
						remote_addr.sa_data[4],
						remote_addr.sa_data[5]	);*/
						#else //使用recv接收
						size = recv(i, tcp_rcv_buff, TCP_RCV_SIZE, MSG_DONTWAIT);
						#endif
						mutex_unlock(socket_mutex);
						if (size ==  - 1)
						{
							if (errno == EWOULDBLOCK || errno == ENOMEM) 
							{
								//出现这两种情况不代表socket断开,所以不能关闭socket,不过很少出现这种情况:)
								p_err("tcp_recv err:%d,%d\n", i, errno);
								sleep(10);
							}
							else
							{
								p_err("tcp_recv fatal err:%d,%d\n", i, errno);
								close_socket(i);
								FD_CLR(i, &rfds);
							}
							continue;
						}
						if (size == 0)
						//0代表连接已经关闭
						{
							if (errno != 0)
							{
								p_err("tcp_client_recv %d err1:%d\n", i, errno);
								close_socket(i);

								if(i == client_socket_fd)
									client_socket_fd =  -1;

								if(i == remote_socket_fd)
									remote_socket_fd =  -1;
							}
							else
								p_err("rcv 0 byte?\n");
							continue;
						}

						//p_dbg("socket:%d rcv:%d\n", i, size);
						ADD_MONITOR_VALUE(tcp_totol_recv, size);
#if	SUPPORT_WEBSERVER
						if(is_web_socket(i))
						{
							handle_web_sm(i, (PUCHAR)tcp_rcv_buff, size);
						}
						else 
#endif
						if (is_dhcp_socket(i))
						{
							handle_dhcp_rcv((uint8_t*)tcp_rcv_buff, size);
						}
						else 
						if(local_udp_server == i)
						{
							handle_local_udp_recv((unsigned char*)tcp_rcv_buff, size);
						}
						else
							handle_test_recv(i, (unsigned char*)tcp_rcv_buff, size);
					} //end of if(FD_ISSET(i,&rfds))
				} //end of for(i = 0; i < select_size; i++)
			}
		}
	}
}


