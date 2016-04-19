#include "api.h"
#include "debug.h"
#include "mutex.h"

mutex_t mutex_init(const char *name)
{
	mutex_t ret;
#ifdef OS_UCOS
	
#ifdef UCOS_V3
	INT16U err;
	ret = (mutex_t)mem_calloc(sizeof(OS_MUTEX), 1);
	if(!ret)
		return 0;
	OSMutexCreate((OS_MUTEX  *)ret,
								(CPU_CHAR  *)name,
								(OS_ERR    *)&err);
#else
	INT8U perr;
	ret =  (mutex_t)OSSemCreate(1);
	if(ret){
		OSEventNameSet(ret, (INT8U*)name, &perr);
	}
#endif
	
#endif

	return ret;
}

int mutex_lock(mutex_t mutex)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
	OSMutexPend((OS_MUTEX	*)mutex,
							(OS_TICK	 )0,
							(OS_OPT    )OS_OPT_PEND_BLOCKING,
							(CPU_TS 	*)0,
							(OS_ERR 	*)&perr);
#else
	INT8U perr;
	OSSemPend((OS_EVENT*)mutex, 0, &perr);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
#endif

	p_err("mutex_lock err %d", perr);
	return  - 1;
}

int mutex_unlock(mutex_t mutex)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
	mutex->OwnerTCBPtr = OSTCBCurPtr;
	OSMutexPost((OS_MUTEX	*)mutex,
							(OS_OPT		 )OS_OPT_POST_NONE,
							(OS_ERR		*)&perr);
#else
	INT8U perr;
	perr = OSSemPost((OS_EVENT*)mutex);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
#endif

	p_err("mutex_unlock err %d", perr);
	return  - 1;
}

void mutex_destory(mutex_t mutex)
{
#ifdef OS_UCOS	
#ifdef UCOS_V3
	INT16U perr = 0;
	OSMutexDel((OS_MUTEX	*)mutex,
						 (OS_OPT		 )OS_OPT_DEL_ALWAYS,
						 (OS_ERR		*)&perr);	
		mem_free(mutex);
#else
	INT8U perr;
	OSSemDel((OS_EVENT*)mutex, OS_DEL_ALWAYS, &perr);
#endif
#endif

	if (perr != 0)
		p_err("mutex_destory err %d", perr);
}
