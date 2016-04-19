#include "api.h"
#include "debug.h"
#include "wait.h"

wait_event_t init_event()
{
	wait_event_t event;
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR err;
	event = (wait_event_t)mem_calloc(sizeof(OS_SEM), 1);
	if(!event)
		return 0;
	OSSemCreate((OS_SEM     *)event, 
							(CPU_CHAR   *)"", 
							(OS_SEM_CTR  )0, 
							(OS_ERR     *)&err);
#else
	event = OSSemCreate(0);
#endif
	if (!event)
		p_err("init_waitqueue_head err");
#endif

	return (wait_event_t)event;
}

int wait_event(wait_event_t wq)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
#else
	INT8U perr;
#endif
#endif

	if (wq == 0)
	{
		return  - 1;
	}
#ifdef OS_UCOS
#ifdef UCOS_V3
	OSSemPend((OS_SEM    *)wq, 
						(OS_TICK    )0, 
						(OS_OPT     )OS_OPT_PEND_BLOCKING, 
						(CPU_TS    *)0, 
						(OS_ERR    *)&perr);
#else
	OSSemPend(wq, 0, &perr);
#endif
	if (perr == OS_ERR_NONE)
	{
		return 0;
	}
#endif

	p_err("add_wait_queue err:%d\n", perr);
	return  - 1;
}

int wait_event_timeout(wait_event_t wq, unsigned int timeout)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
#else
	INT8U perr;
#endif
#endif

	uint32_t ticks;
	if (wq == 0)
	{
		p_err("wait_event_timeout wq err");
		return  - 1;
	}
	ticks = ms2ticks(timeout);
#ifdef OS_UCOS	
#ifdef UCOS_V3
	OSSemPend((OS_SEM    *)wq, 
						(OS_TICK    )ticks, 
						(OS_OPT     )OS_OPT_PEND_BLOCKING, 
						(CPU_TS    *)0, 
						(OS_ERR    *)&perr);
#else
	OSSemPend(wq, ticks, &perr);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
	else if (perr == OS_ERR_TIMEOUT){
		return WAIT_EVENT_TIMEOUT;
	}
#endif


	return  - 1;

}

void wake_up(wait_event_t wq)
{
#ifdef OS_UCOS
	INT16U perr;
#endif

#ifdef OS_UCOS	
#ifdef UCOS_V3
	OSSemPost((OS_SEM    *)wq, 
						(OS_OPT     )OS_OPT_POST_ALL, 
						(OS_ERR    *)&perr);
#else
	perr = OSSemPost(wq);
#endif
	if (perr != OS_ERR_NONE)
		p_err("wake_up err %d", perr);
#endif

		
}


void del_event(wait_event_t wq)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR err;
	OSSemDel((OS_SEM    *)wq, 
					 (OS_OPT     )OS_OPT_DEL_ALWAYS, 
					 (OS_ERR    *)&err);
	mem_free(wq);
#else
	INT8U perr;
	OSSemDel (wq, OS_DEL_ALWAYS, &perr);
#endif
#endif

}


void clear_wait_event(wait_event_t wq)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR perr;
#else
	INT8U perr;
#endif
	if (wq == 0)
	{
		p_err("clear_wait_event wq err");
		return ;
	}
	OSSemSet(wq, 0, (void*)&perr);
	if (perr != OS_ERR_NONE)
		p_err("clear_wait_event err %d", perr);
#endif

	return ;
}
