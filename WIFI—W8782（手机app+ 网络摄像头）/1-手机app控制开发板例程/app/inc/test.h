#if 0

#ifndef _TEST_H
#define _TEST_H

void test_scan(void);
void test_wifi_connect(void);
void test_start_ap(void);
void test_wifi_disconnect(void);
void test_tcpudp_recv(void);
void test_tcp_link(void);
void test_tcp_unlink(void);
void test_send(char *pstr);
void test_sendto(char *pstr);
void test_close_tcp_server(void);
void test_dns(char *hostname);
void test_tcp_server(void);
void test_auto_get_ip(struct netif *p_netif);
void test_open_audio(void);
void test_close_audio(void);
void test_wifi_get_stats(void);
void show_tcpip_info(struct netif *p_netif);
void show_sys_info(struct netif *p_netif);
void test_udp_link(void);
void test_multicast_join(void);
void test_udp_server(void);
void test_create_ap(void);
void test_stop_ap(void);
void test_get_station_list(void);
void test_power_save_enable(void);
void test_power_save_disable(void);
void test_wifi_join_adhoc(void);
void test_wifi_leave_adhoc(void);
void monitor_switch(void);
void test_full_speed_send(void);
void test_full_speed_send_stop(void);
void switch_loopback_test(void);
void print_help(void);
void handle_test_recv(int socket, uint8_t *data, int len);
void handle_cmd(char cmd);
#endif


#else



#ifndef _TEST_H
#define _TEST_H

#include "stm32f4xx.h"

void test_scan(void);
void test_wifi_connect(void);
void test_start_ap(void);
void test_wifi_disconnect(void);
void test_tcpudp_recv(void);
void test_tcp_link(void);
void test_tcp_unlink(void);
void test_send(char *pstr);
void test_sendto(char *pstr);
void test_close_tcp_server(void);
void test_dns(char *hostname);
int test_tcp_server(uint16_t port);
void test_auto_get_ip(struct netif *p_netif);
void test_open_audio(void);
void test_close_audio(void);
void test_wifi_get_stats(void);
void show_tcpip_info(struct netif *p_netif);
void show_sys_info(struct netif *p_netif);
void test_udp_link(void);
void test_multicast_join(void);
void test_udp_server(void);
void test_create_ap(void);
void test_stop_ap(void);
void test_get_station_list(void);
void test_power_save_enable(void);
void test_power_save_disable(void);
void test_wifi_join_adhoc(void);
void test_wifi_leave_adhoc(void);
void monitor_switch(void);
void test_full_speed_send(void);
void test_full_speed_send_stop(void);
void switch_loopback_test(void);
void print_help(void);
void handle_test_recv(int socket, uint8_t *data, int len);
void handle_cmd(char cmd);




/*********/
int TCP_Send_Data(int s,char *data,int len,int flags);





#endif
#endif
