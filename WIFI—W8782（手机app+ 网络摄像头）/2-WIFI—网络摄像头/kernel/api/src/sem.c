#include "api.h"
#include "debug.h"
#include "sem.h"

void sem_init(sem_t *sem, int value)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR err;
	sem_t _sem;
	_sem = (sem_t)mem_calloc(sizeof(OS_SEM), 1);
	if(!_sem)
	{
		*sem = 0;
		return;
	}
	OSSemCreate((OS_SEM     *)_sem, 
							(CPU_CHAR   *)"sem", 
							(OS_SEM_CTR  )value, 
							(OS_ERR     *)&err);
	*sem = _sem;				
#else
	*sem = (sem_t)OSSemCreate(value);
#endif
	if (*sem == 0)

		p_err("sem_init err");
#endif

}

int sem_wait(sem_t *sem, unsigned int timeout)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
#else
	INT8U perr;
#endif
#endif

	timeout = ms2ticks(timeout);
#ifdef OS_UCOS
#ifdef UCOS_V3
	OSSemPend((OS_SEM    *)*sem, 
						(OS_TICK    )timeout, 
						(OS_OPT     )OS_OPT_PEND_BLOCKING, 
						(CPU_TS    *)0, 
						(OS_ERR    *)&perr);
#else
	OSSemPend((OS_EVENT*) * sem, timeout, &perr);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
	else if (perr == OS_ERR_TIMEOUT)
		return 1;
#endif

	p_err("sem_wait err %d", perr);
	return  - 1;
}

int sem_post(sem_t *sem)
{

#ifdef OS_UCOS
#ifdef UCOS_V3
	INT16U perr;
	OSSemPost((OS_SEM    *)*sem, 
						(OS_OPT     )OS_OPT_POST_ALL, 
						(OS_ERR    *)&perr);
#else
	INT8U perr;
	perr = OSSemPost((OS_EVENT*) *sem);
#endif
	if (perr == OS_ERR_NONE)
		return 0;
#endif

	p_err("sem_post err %d", perr);
	return  - 1;

}

//sem 为空，说明可能有其他任务在等待
int is_sem_empty(sem_t *sem)
{
#ifdef OS_UCOS	
#ifdef UCOS_V3
	return ((*sem)->Ctr == 0);
#else
	return ((*sem)->OSEventCnt == 0);
#endif
#endif

}

void sem_destory(sem_t *sem)
{

#ifdef OS_UCOS	
#ifdef UCOS_V3
	INT16U perr = 0;
	OSSemDel((OS_SEM    *)*sem, 
					 (OS_OPT     )OS_OPT_DEL_ALWAYS, 
					 (OS_ERR    *)&perr);
	mem_free(*sem);
#else
	INT8U perr = 0;
	OSSemDel((OS_EVENT*) * sem, OS_DEL_ALWAYS, &perr);
#endif
#endif

	if (perr != 0)
		p_err("sem_destory err %d", perr);
}
