#include "api.h"
#include "debug.h"
#include "timer.h"
#include "sys_misc.h"

#ifdef OS_UCOS
void _comm_timer_callback(timer_t ptmr, void *parg)
{
	assert(ptmr);
	timer_tasklet_callback(&ptmr->work);
}
#endif



timer_t timer_setup(int time_val, int type, timer_callback_func callback, void *callback_arg)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
#else
	INT8U perr;	
#endif
#endif
	timer_t tmr;


	time_val = ms2ticks(time_val);

#ifdef OS_UCOS
	if ((callback == 0))
	{
		p_err("setup_timer err arg\n");
		return 0;
	}
	if (type)
	//== 1 repeat
	{
		p_dbg("repeat:%d\n", time_val);
#ifdef UCOS_V3
		tmr=(OS_TMR *)mem_calloc(1, sizeof(OS_TMR));
		if(tmr)
			OSTmrCreate(tmr,"TIMER",time_val, time_val, OS_OPT_TMR_PERIODIC, (OS_TMR_CALLBACK_PTR)_comm_timer_callback, 0, (OS_ERR*)&perr);
#else
		tmr = OSTmrCreate(time_val, time_val, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)_comm_timer_callback, 0, "", &perr);
#endif
	}
	else
	{
		p_dbg("one short:%d\n", time_val);
#ifdef UCOS_V3
		tmr=(OS_TMR *)mem_calloc(1, sizeof(OS_TMR));
		if(tmr)
			OSTmrCreate(tmr,"TIMER",time_val, time_val, OS_OPT_TMR_ONE_SHOT, (OS_TMR_CALLBACK_PTR)_comm_timer_callback, 0, (OS_ERR*)&perr);
#else
		tmr = OSTmrCreate(time_val, time_val, OS_TMR_OPT_ONE_SHOT, (OS_TMR_CALLBACK)_comm_timer_callback, 0, "", &perr);
#endif
	}

	if (perr != OS_ERR_NONE)
	{
		tmr = 0;
		p_err("setup_timer err\n");
	}

	init_work(&tmr->work, callback, callback_arg);
#endif

	return tmr;
}

int timer_pending(timer_t tmr)
{
	timer_t tmr_t = tmr;

	if (tmr_t == 0)
	{
		p_err("timer_pending tmr err");
		return  0;
	}
#ifdef OS_UCOS
#ifdef UCOS_V3
	if (tmr_t->State == OS_TMR_STATE_RUNNING)
	{
		return 1;
	}
#else
	if (tmr_t->OSTmrState == OS_TMR_STATE_RUNNING)
	{
		return 1;
	}
#endif
#endif

	return 0;
}

//expires ----n*ms
int mod_timer(timer_t tmr, unsigned int expires)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr, ret;
#else
	INT8U perr, ret;
#endif
#endif

	timer_t tmr_t = tmr;

	if (tmr_t == 0)
	{
		p_err("mod_timer tmr err");
		return  0;
	}

	expires = ms2ticks(expires);

	if(timer_pending(tmr)/* && (expires == tmr_t->OSTmrPeriod)*/)
		return 1;
#ifdef OS_UCOS
#ifdef UCOS_V3
	tmr_t->Dly = expires;
	tmr_t->Period = expires;
	ret = OSTmrStart(tmr_t, (OS_ERR*)&perr);
	if (ret == TRUE && perr == OS_ERR_NONE)
	{
		return 0;
	}
#else
	tmr_t->OSTmrDly = expires;
	tmr_t->OSTmrPeriod = expires;
	ret = OSTmrStart(tmr_t, &perr);
	if (ret == OS_TRUE && perr == OS_ERR_NONE)
	{
		return 0;
	}
#endif
#endif

	return  0;
}

int add_timer(timer_t tmr_t)
{
#ifdef OS_UCOS
	INT16U perr;//, ret
#endif

	if (tmr_t == 0)
	{
		p_err("add_timer tmr err");
		return  - 1;
	}
#ifdef OS_UCOS
	 OSTmrStart(tmr_t, (void*)&perr);//ret =
	if (perr == OS_ERR_NONE)
	{
		return OS_ERR_NONE;
	}
#endif

	return  - 1;
}

//停止定时器
int del_timer(timer_t tmr)
{
#ifdef OS_UCOS	
#ifdef UCOS_V3
	INT16U perr;
#else
	INT8U perr;
#endif
#endif

	timer_t tmr_t = tmr;
	
	if (tmr_t == 0)
	{
		p_err("del_timer tmr err");
		return 0;
	}
#ifdef OS_UCOS
#ifdef UCOS_V3
	if(timer_pending(tmr))
	{
		OSTmrStop(tmr_t, OS_OPT_TMR_NONE, 0, (OS_ERR*)&perr);
		return 1;
	}
	OSTmrStop(tmr_t, OS_OPT_TMR_NONE, 0, (OS_ERR*)&perr);
#else
	if(timer_pending(tmr))
	{
		OSTmrStop(tmr_t, OS_TMR_OPT_NONE, 0, &perr);
		return 1;
	}
	OSTmrStop(tmr_t, OS_TMR_OPT_NONE, 0, &perr);
#endif
#endif

	return  0;
}

//释放删除定时器
int timer_free(timer_t tmr)
{
#ifdef OS_UCOS
	INT16U ret;
#endif

	timer_t tmr_t = tmr;
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
	ret = OSTmrDel(tmr_t, (OS_ERR*)&perr);

	if ((ret == TRUE) && (perr == OS_ERR_NONE))
	{
		mem_free(tmr_t);
		return OS_ERR_NONE;
	}
#else
	INT8U perr;
	ret = OSTmrDel(tmr_t, &perr);

	if ((ret == OS_TRUE) && (perr == OS_ERR_NONE))
		return OS_ERR_NONE;
#endif
#endif

	p_err("timer_free err %d", perr);
	return  - 1;
}

void sleep(uint32_t ms)
{
	u32 s = 0;
	
	if(in_interrupt()){
	
		delay_us(ms*1000);
		return;
	}

#ifdef OS_UCOS
#ifdef UCOS_V3
	ms = ms2ticks(ms);
	OSTimeDly(ms,OS_OPT_TIME_DLY,(OS_ERR*)&s);
#else
	if(ms < 10)
		ms = 10;
	if(ms > 1000)
	{
		s = ms/1000;
		ms = ms%1000;
	}
	OSTimeDlyHMSM (0,0,s,ms);
#endif
#endif
	
}

//ms
unsigned int os_time_get(void)
{
	return (unsigned int)tick_ms;
}

unsigned int ms2ticks(unsigned int ms)
{
	if(ms == 0)
		return 0;
	else{
		if(ms < 10)
			ms = 10;
			ms = ms * OS_TICK_RATE_HZ / 1000uL;
	}
	return ms;
}

