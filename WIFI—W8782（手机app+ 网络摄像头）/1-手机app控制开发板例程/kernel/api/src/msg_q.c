#include "api.h"
#include "debug.h"
#include "msg_q.h"

#ifdef OS_UCOS
struct _msg_q_manage
{
	msg_q_t msg;
	void **q_start;
};

struct _msg_q_manage q_man[MSG_QUEUE_MAX];
#endif 

int msgget(msg_q_t *msg_q, int q_size)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR err;
	OS_Q *msg;
	
	void **q_start = NULL;
	int i;
	
	msg = (OS_Q*)mem_calloc(sizeof(OS_Q), 1);
	if(!msg)
		return -1;
	for (i = 0; i < MSG_QUEUE_MAX; i++)
	{
		if (q_man[i].msg == 0)
			break;
	} if (i >= MSG_QUEUE_MAX)
	{
		p_err("msgget: no more msg queen");
		return  - 1;
	}
	
	
#else
	OS_EVENT *msg;
	void **q_start = NULL;
	int i;

	for (i = 0; i < OS_MAX_QS; i++)
	{
		if (q_man[i].msg == 0)
			break;
	} 
	if (i >= OS_MAX_QS)
	{
		p_err("msgget: no more msg queen");
		return  - 1;
	}
#endif
	q_start = (void **)mem_malloc(q_size *(sizeof(void*)));
	if (q_start == 0)
	{
		p_err("msgget: malloc queen err.no enough malloc");
		return  - 1;
	}
#ifdef UCOS_V3
	OSQCreate((OS_Q				*)msg, 
						(CPU_CHAR		*)"msg_q", 
						(OS_MSG_QTY	 )q_size, 
						(OS_ERR			*)&err );
#else
	msg = OSQCreate(q_start, (INT16U)q_size);
#endif
	if (msg == 0)
	{
		*msg_q = 0;
		mem_free(q_start);
		return  - 1;
	}
	*msg_q = (msg_q_t)msg;
	q_man[i].msg = (msg_q_t)msg;
	q_man[i].q_start = q_start;
#endif

	
	return 0;
}

int msgsnd(msg_q_t msgid, void *msgbuf)
{
	INT16U perr;

	assert(msgid);
#ifdef OS_UCOS
#ifdef UCOS_V3


	OSQPost(msgid,
        msgbuf,
        0,
        0,
        (OS_ERR*)&perr);
#else
	perr = OSQPost((OS_EVENT*)msgid, (void*)msgbuf);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
	else if (perr == OS_ERR_Q_FULL)
		return 1;
#endif

	p_err("msgsnd: err %d", perr);
	return  - 1;
}

int msgrcv(msg_q_t msgid, void **msgbuf, unsigned int timeout)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_MSG_SIZE p_msg_size;
	CPU_TS p_ts;
	INT16U perr;
#else
	INT8U perr;
#endif
	
#endif


	assert(msgid);
	timeout = ms2ticks(timeout);
#ifdef OS_UCOS
#ifdef UCOS_V3
	*msgbuf = OSQPend(msgid,
                timeout,
                OS_OPT_PEND_BLOCKING,
                &p_msg_size,
                &p_ts,
                (OS_ERR*)&perr);
#else
	*msgbuf = OSQPend((OS_EVENT*)msgid, (INT32U)timeout, &perr);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
	else if (perr == OS_ERR_TIMEOUT)
		return 1;
#endif

	p_err("msgrcv: err %d", perr);
	return  - 1;
}

int msgfree(msg_q_t msgid)
{
#ifdef OS_UCOS
	int i;
	
#ifdef UCOS_V3
	INT16U perr;
	for (i = 0; i < MSG_QUEUE_MAX; i++)
	{
		if (q_man[i].msg == msgid)
			break;
	}
	if (i >= MSG_QUEUE_MAX)
	{
		p_err("msgfree: err no match msg_q %x", msgid);
		return  - 1;
	}
#else
	INT8U perr;
	for (i = 0; i < OS_MAX_QS; i++)
	{
		if (q_man[i].msg == msgid)
			break;
	}
	if (i >= OS_MAX_QS)
	{
		p_err("msgfree: err no match msg_q %x", msgid);
		return  - 1;
	}
#endif
#ifdef UCOS_V3
	OSQDel((OS_Q*)msgid, OS_OPT_DEL_ALWAYS, (OS_ERR*)&perr);
#else
	OSQDel((OS_EVENT*)msgid, OS_DEL_ALWAYS, &perr);
#endif
	if (OS_ERR_NONE != perr)
	{
		p_err("msgfree: free err %d", perr);
		return  - 1;
	}

	if (!q_man[i].q_start)
	{
		assert(0);
	}

	mem_free(q_man[i].q_start);
	q_man[i].q_start = 0;
	q_man[i].msg = 0;
#endif

	return 0;
}

void msg_q_init(void)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	int i;
	for (i = 0; i < MSG_QUEUE_MAX; i++)
	{
		q_man[i].msg = 0;
		q_man[i].q_start = 0;
	}
#else
	int i;
	for (i = 0; i < OS_MAX_QS; i++)
	{
		q_man[i].msg = 0;
		q_man[i].q_start = 0;
	}
#endif
#endif
}
