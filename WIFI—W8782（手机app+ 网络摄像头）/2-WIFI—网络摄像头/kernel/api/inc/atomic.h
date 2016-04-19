#ifndef _ATOMIC_H_
#define _ATOMIC_H_
#include "api.h"

#ifdef OS_UCOS
#ifdef UCOS_V3
#define in_interrupt() (OSIntNestingCtr)
#else
#define in_interrupt() (OSIntNesting)
#endif
#endif

#define atomic_t atomic


typedef struct _atomic
{
	volatile unsigned char val;
} atomic;

__inline unsigned int local_irq_save(void)
{
	unsigned int cpu_sr;
#ifdef OS_UCOS
#ifdef UCOS_V3
	cpu_sr = CPU_SR_Save();
#else
	cpu_sr = OS_CPU_SR_Save();
#endif
#endif

	return (unsigned int)cpu_sr;
}

__inline void local_irq_restore(unsigned int cpu_sr)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	CPU_SR_Restore(cpu_sr);
#else
	OS_CPU_SR_Restore(cpu_sr);
#endif
#endif

}

__inline void enter_critical()
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR	err;
	OSSchedLock(&err);
#else
	OSSchedLock();
#endif
#endif

}

__inline void exit_critical()
{
#ifdef OS_UCOS	
#ifdef UCOS_V3
	OS_ERR	err;
	OSSchedUnlock(&err);
#else
	OSSchedUnlock();
#endif
#endif

}

int atomic_test_set(atomic *at, int val);
void atomic_set(atomic *at, int val);
int atomic_read(volatile atomic_t *v);
void atomic_add(int i, volatile atomic_t *v);
void atomic_sub(int i, volatile atomic_t *v);
int atomic_add_return(int i, atomic_t *v);
int  atomic_sub_return(int i, atomic_t *v);

#endif
