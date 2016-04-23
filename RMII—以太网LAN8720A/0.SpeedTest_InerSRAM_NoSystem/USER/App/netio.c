#include "lwip/opt.h"
#include "netio.h"
//#include "includes.h"
#include "stm32f4xx.h"

#include "lwip/sockets.h"

#if LWIP_TCP
#include "lwip/tcp.h"

/*
 * This implements a netio server.
 *  The client sends a command word (4 bytes) then a data length word (4 bytes).
 *  If the command is "receive", the server is to consume "data length" bytes into
 *   a circular buffer until the first byte is non-zero, then it is to consume
 *   another command/data pair.
 *  If the command is "send", the server is to send "data length" bytes from a circular
 *   buffer with the first byte being zero, until "some time" (6 seconds in the
 *   current netio126.zip download) has passed and then send one final buffer with
 *   the first byte being non-zero. Then it is to consume another command/data pair.
 */

/* See http://www.nwlab.net/art/netio/netio.html to get the netio tool */


#define NETIO_BUF_SIZE              (4 * 1024)
#define NETIO_USE_STATIC_BUF        0

//NETIO状态定义
#define NETIO_STATE_WAIT_FOR_CMD    0
#define NETIO_STATE_RECV_DATA       1
#define NETIO_STATE_SEND_DATA       2
#define NETIO_STATE_SEND_DATA_LAST  3
#define NETIO_STATE_DONE            4

struct netio_state {
  u32_t  state;
  u32_t  cmd;
  u32_t  data_len;
  u32_t  cntr;
  u8_t * buf_ptr;
  u32_t  buf_pos;
  u32_t  first_byte;
  u32_t  time_stamp;
};

//NETIO命令协议定义
#define NETIO_CMD_QUIT              0
#define NETIO_CMD_C2S               1
#define NETIO_CMD_S2C               2
#define NETIO_CMD_RES               3

static err_t netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

extern __IO uint32_t LocalTime ; /* this variable is used to create a time reference incremented by 10ms */


//NETIO关闭
static void netio_close(void *arg, struct tcp_pcb *pcb)
{
  err_t err;

  struct netio_state *ns = arg;
  ns->state = NETIO_STATE_DONE;  	//标记NETIO不处于任何状态
  tcp_recv(pcb, NULL);
  err = tcp_close(pcb);				//关闭连接

  if (err != ERR_OK) {
    tcp_recv(pcb, netio_recv); 		//关闭失败，稍后重试
  } else {
	//关闭成功
#if NETIO_USE_STATIC_BUF != 1
    if(ns->buf_ptr != NULL){
      mem_free(ns->buf_ptr);
    }
#endif
    tcp_arg(pcb, NULL);		//注销掉参数
    tcp_poll(pcb, NULL, 0);	//注销掉轮训函数
    tcp_sent(pcb, NULL);	//注销掉发送函数	
    if (arg != NULL) {
      mem_free(arg);		//释放arg的内存
    }
  }
}

//接收数据回调函数
static err_t netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct netio_state *ns = arg;
  u8_t * data_ptr;
  u32_t data_cntr;
  struct pbuf *q = p;
  u16_t len;
	


  if (p != NULL) {
    tcp_recved(pcb, p->tot_len); //通知LWIP可以获取更多数据
  }

  if (err == ERR_OK && q != NULL) {

    while (q != NULL) {
      data_cntr = q->len;
      data_ptr = q->payload;
      while (data_cntr--) {
        if (ns->state == NETIO_STATE_DONE){
          netio_close(ns, pcb); //关闭连接
          break;
        } else if (ns->state == NETIO_STATE_WAIT_FOR_CMD) {
          if (ns->cntr < 4) {
            /* build up the CMD field */
            ns->cmd <<= 8;
            ns->cmd |= *data_ptr++;
            ns->cntr++;
          } else if (ns->cntr < 8) {
            /* build up the DATA field */
            ns->data_len <<= 8;
            ns->data_len |= *data_ptr++;
            ns->cntr++;

            if (ns->cntr == 8) {
              /* now we have full command and data words */
              ns->cntr = 0;
              ns->buf_pos = 0;
              ns->buf_ptr[0] = 0;
              if (ns->cmd == NETIO_CMD_C2S) {
                ns->state = NETIO_STATE_RECV_DATA;
              } else if (ns->cmd == NETIO_CMD_S2C) {
                ns->state = NETIO_STATE_SEND_DATA;
				 //开始时的时间
                ns->time_stamp = LocalTime;//OSTimeGet(&os_err);  //获取时间

                len = tcp_sndbuf(pcb);
                len = LWIP_MIN(len, ns->data_len - ns->cntr);
                len = LWIP_MIN(len, NETIO_BUF_SIZE - ns->buf_pos);

                do {
                  err = tcp_write(pcb, ns->buf_ptr + ns->buf_pos, len, TCP_WRITE_FLAG_COPY);
                  if (err == ERR_MEM) {
                    len /= 2;
                  }
                } while ((err == ERR_MEM) && (len > 1));

                ns->buf_pos += len;
                ns->cntr    += len;

              } else {
                //不能识别的命令
                ns->cntr = 0;
                ns->buf_pos = 0;
                ns->buf_ptr[0] = 0;
                netio_close(ns, pcb);
                break;
              }
            }
          } else {
            /* in trouble... shouldn't be in this state! */
          }

        } else if (ns->state == NETIO_STATE_RECV_DATA) {  //NETIO处于接收数据状态

          if(ns->cntr == 0){
            /* save the first byte of this new round of data
             * this will not match ns->buf_ptr[0] in the case that
             *   NETIO_BUF_SIZE is less than ns->data_len.
             */
            ns->first_byte = *data_ptr;
          }

          ns->buf_ptr[ns->buf_pos++] = *data_ptr++;
          ns->cntr++;

          if (ns->buf_pos == NETIO_BUF_SIZE) {
            /* circularize the buffer */
            ns->buf_pos = 0;
          }

          if(ns->cntr == ns->data_len){
            ns->cntr = 0;
            if (ns->first_byte != 0) {
              /* if this last round did not start with 0,
               *  go look for another command */
              ns->state = NETIO_STATE_WAIT_FOR_CMD;
              ns->data_len = 0;
              ns->cmd = 0;
              /* TODO LWIP_DEBUGF( print out some throughput calculation results... ); */
            } else {
              /* stay here and wait on more data */
            }
          }

        } else if (ns->state == NETIO_STATE_SEND_DATA
            || ns->state == NETIO_STATE_SEND_DATA_LAST) {
          /* I don't think this should happen... */
        } else {
          /* done / quit */
          netio_close(ns, pcb);
          break;
        } /* end of ns->state condition */
      } /* end of while data still in this pbuf */

      q = q->next;  //处理下pbuf中的数据
    }

    pbuf_free(p);  //释放内存
 
  } else {
	 //错误或者另一端关闭
    if (p != NULL) {
      pbuf_free(p); //释放内存
    }
    netio_close(ns, pcb); //关闭连接

  }
  return ERR_OK;

}

//发送数据回调函数
static err_t netio_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  struct netio_state *ns = arg;
  err_t err = ERR_OK;
//	OS_ERR os_err;
	
  if (ns->cntr >= ns->data_len && ns->state == NETIO_STATE_SEND_DATA) {
    /* done with this round of sending */
    ns->buf_pos = 0;
    ns->cntr = 0;

    /* check if timer expired */
    if (/*OSTimeGet(&os_err) */LocalTime- ns->time_stamp > 600) {
      ns->buf_ptr[0] = 1;
      ns->state = NETIO_STATE_SEND_DATA_LAST;
    } else {
      ns->buf_ptr[0] = 0;
    }
  }

  if(ns->state == NETIO_STATE_SEND_DATA_LAST || ns->state == NETIO_STATE_SEND_DATA){
    len = tcp_sndbuf(pcb);
    len = LWIP_MIN(len, ns->data_len - ns->cntr);
    len = LWIP_MIN(len, NETIO_BUF_SIZE - ns->buf_pos);

    if(ns->cntr < ns->data_len){
      do {
        err = tcp_write(pcb, ns->buf_ptr + ns->buf_pos, len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_MEM) {
          len /= 2;
        }
      } while ((err == ERR_MEM) && (len > 1));

      ns->buf_pos += len;
      if(ns->buf_pos >= NETIO_BUF_SIZE){
        ns->buf_pos = 0;
      }

      ns->cntr += len;
    }
  }

  if(ns->cntr >= ns->data_len && ns->state == NETIO_STATE_SEND_DATA_LAST){
    /* we have buffered up all our data to send this last round, go look for a command */
   ns->state = NETIO_STATE_WAIT_FOR_CMD;
   ns->cntr  = 0;
   /* TODO LWIP_DEBUGF( print out some throughput calculation results... ); */
  }

  return ERR_OK;
}

//轮训回调函数，每2秒调用一次
static err_t
netio_poll(void *arg, struct tcp_pcb *pcb)
{
  struct netio_state * ns = arg;
  if(ns->state == NETIO_STATE_SEND_DATA){

  } else if(ns->state == NETIO_STATE_DONE){
    netio_close(ns, pcb);
  }

  return ERR_OK;

}

#if NETIO_USE_STATIC_BUF == 1
static u8_t netio_buf[NETIO_BUF_SIZE];
#endif

//接受连接
static err_t netio_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct netio_state * ns;

  LWIP_UNUSED_ARG(err);

  ns = mem_malloc(sizeof(struct netio_state));

  if(ns == NULL){
    return ERR_MEM;
  }

  ns->state = NETIO_STATE_WAIT_FOR_CMD;
  ns->data_len = 0;
  ns->cmd = 0;
  ns->cntr = 0;
  ns->buf_pos = 0;
#if NETIO_USE_STATIC_BUF == 1
  ns->buf_ptr = netio_buf;
#else
  ns->buf_ptr = mem_malloc(NETIO_BUF_SIZE);

  if(ns->buf_ptr == NULL){
    mem_free(ns);
    return ERR_MEM;
  }
#endif

  ns->buf_ptr[0] = 0;

  tcp_arg(pcb, ns);				//注册参数
  tcp_sent(pcb, netio_sent);	//注册发送回调函数
  tcp_recv(pcb, netio_recv);	//注册接收回调函数
  tcp_poll(pcb, netio_poll, 4); //注册轮训函数，每2秒轮训一次
  return ERR_OK;
}

//NETIO初始化
void netio_init(void)
{
  struct tcp_pcb *pcb;

  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, 18767);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, netio_accept);
}


#endif


