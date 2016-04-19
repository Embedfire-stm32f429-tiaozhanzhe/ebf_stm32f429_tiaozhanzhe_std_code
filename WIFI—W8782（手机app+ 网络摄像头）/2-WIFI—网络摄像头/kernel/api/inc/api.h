#ifndef _API_H_
#define _API_H_

//kernel
#ifdef OS_UCOS
#ifdef UCOS_V3
#include "os.h"
#include "os_cfg_app.h"
#else
#include "ucos_ii.h"
#endif
#endif
#include "atomic.h"
#include "mbox.h"
#include "mutex.h"
#include "sem.h"
#include "wait.h"
#include "task_api.h"
#include "timer.h"
#include "msg_q.h"
#include "memory.h"

extern volatile uint32_t jiffies;

__inline void enter_interrupt(void)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OSIntNestingCtr++;
#else
	OSIntNesting++;
#endif
#endif

}

__inline void exit_interrupt(int need_sched)
{
#ifdef	OS_UCOS
	if (need_sched)
		OSIntExit();
	else{
#ifdef UCOS_V3
		OSIntNestingCtr--;
#else
		OSIntNesting--;
#endif
	}
#endif

}

#endif
