#include "api.h"
#include "debug.h"
#include "mbox.h"

#ifdef OS_UCOS
void mbox_new(mbox_t *mbox, void *pmsg)
{
#ifdef UCOS_V3
	OS_ERR err;
	OS_Q *event;
	OSQCreate((OS_Q	*)event, 
			(CPU_CHAR*)"mbox event", 
			(OS_MSG_QTY)10, 
			(OS_ERR*)&err );
#else
	OS_EVENT *event;

	event = OSMboxCreate(pmsg);
#endif
	if (event == 0)
		p_err("mbox_new err");

	*mbox = (mbox_t)event;
}

void *mbox_get(mbox_t *mbox, unsigned int timeout)
{
	void *msg;
#ifdef UCOS_V3
	INT16U err;
	OS_MSG_SIZE	size;
#else
	INT8U err;
#endif
	timeout = ms2ticks(timeout);
#ifdef UCOS_V3
	msg = OSQPend((OS_Q				 *)mbox, 
								(OS_TICK			)timeout, 
								(OS_OPT				)OS_OPT_PEND_BLOCKING, 
								(OS_MSG_SIZE *)&size, 
								(CPU_TS			 *)0, 
								(OS_ERR			 *)&err);
#else
	msg = OSMboxPend((OS_EVENT*) * mbox, timeout, &err);
#endif
	if (err != OS_ERR_NONE)
	{
		p_err("mbox_get err %d", err);
		return 0;
	}

	return msg;
}

int mbox_post(mbox_t *mbox, void *pmsg)
{
	INT16U perr;
#ifdef UCOS_V3
	OSQPost((OS_Q			*)mbox,
					(void			*)pmsg,
					(OS_MSG_SIZE )1,
					(OS_OPT		 )OS_OPT_POST_FIFO,					
					(OS_ERR		*)&perr);
#else
	perr = OSMboxPost((OS_EVENT*) * mbox, pmsg);
#endif
	if (perr == OS_ERR_NONE)
		return 0;

	p_err("mbox_post err %d", perr);
	return  - 1;

}

int mbox_destory(mbox_t *mbox)
{
#ifdef UCOS_V3
	INT16U perr;
	OSQDel((OS_Q			*)mbox,(OS_OPT		 )OS_OPT_POST_FIFO,(OS_ERR		*)&perr);
#else
	INT8U perr;
	OSMboxDel((OS_EVENT*) * mbox, OS_DEL_ALWAYS, &perr);
#endif
	if (perr == OS_ERR_NONE)
		return 0;

	p_err("mbox_destory err %d", perr);
	return  - 1;

}
#endif
