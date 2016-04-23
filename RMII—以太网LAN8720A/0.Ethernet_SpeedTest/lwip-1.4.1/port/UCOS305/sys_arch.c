#if 1

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "includes.h"
#include "arch/sys_arch.h"


/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/
#define LWIP_ARCH_TICK_PER_MS       (1000/OS_CFG_TICK_RATE_HZ)


/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/
static OS_MEM StackMem;

const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;

__align(4) CPU_STK       LwIP_Task_Stk[LWIP_TASK_MAX*LWIP_STK_SIZE];
__align(4) CPU_INT08U    LwIP_task_priopity_stask[LWIP_TASK_MAX];
OS_TCB        LwIP_task_TCB[LWIP_TASK_MAX];


u32_t sys_now()
{
	OS_TICK os_tick_ctr;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	os_tick_ctr = OSTickCtr;
	CPU_CRITICAL_EXIT();

	return os_tick_ctr;
}

/*-----------------------------------------------------------------------------------*/
//  Creates an empty mailbox.
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  OS_ERR       ucErr;
      
  OSQCreate(mbox,"LWIP quiue", size, &ucErr); 
  LWIP_ASSERT( "OSQCreate ", ucErr == OS_ERR_NONE );
  
  if( ucErr == OS_ERR_NONE){ 
    return 0; 
  }
  return -1;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
	OS_ERR     ucErr;
	LWIP_ASSERT( "sys_mbox_free ", mbox != SYS_MBOX_NULL );      
			
	OSQFlush(mbox,& ucErr);
	
	OSQDel(mbox, OS_OPT_DEL_ALWAYS, &ucErr);
	LWIP_ASSERT( "OSQDel ", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------*/
//   Posts the "msg" to the mailbox.
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
	OS_ERR     ucErr;
  CPU_INT08U  i=0; 
  if( data == NULL ) data = (void*)&pvNullPointer;
  /* try 10 times */
  while(i<10){
    OSQPost(mbox, data,0,OS_OPT_POST_ALL,&ucErr);
    if(ucErr == OS_ERR_NONE)
      break;
    i++;
    OSTimeDly(5,OS_OPT_TIME_DLY,&ucErr);
  }
  LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  
}


/*-----------------------------------------------------------------------------------*/
//   Try to post the "msg" to the mailbox.
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  OS_ERR     ucErr;
  if(msg == NULL ) msg = (void*)&pvNullPointer;  
  OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);    
  if(ucErr != OS_ERR_NONE){
    return ERR_MEM;
  }
  return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  OS_ERR	ucErr;
  OS_MSG_SIZE   msg_size;
  CPU_TS        ucos_timeout;  
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
  if(timeout && in_timeout == 0)
    in_timeout = 1;
  *msg  = OSQPend (mbox,in_timeout,OS_OPT_PEND_BLOCKING,&msg_size, 
                          &ucos_timeout,&ucErr);

  if ( ucErr == OS_ERR_TIMEOUT ) 
      ucos_timeout = SYS_ARCH_TIMEOUT;  
  return ucos_timeout; 
}


/*----------------------------------------------------------------------------------*/
int sys_mbox_valid(sys_mbox_t *mbox)          
{      
  if(mbox->NamePtr)  
    return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
  else
    return 0;
}                                             
/*-----------------------------------------------------------------------------------*/                                              
void sys_mbox_set_invalid(sys_mbox_t *mbox)   
{                                             
  if(sys_mbox_valid(mbox))
    sys_mbox_free(mbox);                  
}                                             

/*-----------------------------------------------------------------------------------*/
//  Creates a new semaphore. The "count" argument specifies
//  the initial state of the semaphore.
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	OS_ERR	ucErr;
  OSSemCreate (sem,"LWIP Sem",count,&ucErr);
  if(ucErr != OS_ERR_NONE ){
    LWIP_ASSERT("OSSemCreate ",ucErr == OS_ERR_NONE );
    return -1;    
  }
  return 0;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  OS_ERR	ucErr;
  CPU_TS        ucos_timeout;
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
  if(timeout && in_timeout == 0)
    in_timeout = 1;  
  OSSemPend (sem,in_timeout,OS_OPT_PEND_BLOCKING,&ucos_timeout,&ucErr);
    /*  only when timeout! */
  if(ucErr == OS_ERR_TIMEOUT)
      ucos_timeout = SYS_ARCH_TIMEOUT;	
  return ucos_timeout;
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR	ucErr;  
  OSSemPost(sem,OS_OPT_POST_ALL,&ucErr);
  LWIP_ASSERT("OSSemPost ",ucErr == OS_ERR_NONE ); 
}

/*-----------------------------------------------------------------------------------*/
// Deallocates a semaphore
void sys_sem_free(sys_sem_t *sem)
{
  OS_ERR     ucErr;
	OSSemDel(sem, OS_OPT_DEL_ALWAYS, &ucErr );
	LWIP_ASSERT( "OSSemDel ", ucErr == OS_ERR_NONE );
}
/*-----------------------------------------------------------------------------------*/
int sys_sem_valid(sys_sem_t *sem)                                               
{
  if(sem->NamePtr)
    return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
  else
    return 0;                                    
}

/*-----------------------------------------------------------------------------------*/                                                                                                                                                                
void sys_sem_set_invalid(sys_sem_t *sem)                                        
{                                                                               
  if(sys_sem_valid(sem))
    sys_sem_free(sem);                                                       
} 

/*-----------------------------------------------------------------------------------*/
// Initialize sys arch
void sys_init(void)
{
  OS_ERR ucErr;
  memset(LwIP_task_priopity_stask,0,sizeof(LwIP_task_priopity_stask));
  /* init mem used by sys_mbox_t, use ucosII functions */
  OSMemCreate(&StackMem,"LWIP TASK STK",(void*)LwIP_Task_Stk,LWIP_TASK_MAX,LWIP_STK_SIZE*sizeof(CPU_STK),&ucErr);
  LWIP_ASSERT( "sys_init: failed OSMemCreate STK", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------*/
// TODO
/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread , void *arg, int stacksize, int prio)
{
  CPU_INT08U  ubPrio = LWIP_TASK_START_PRIO;
	OS_ERR      ucErr;
	int i; 
	int tsk_prio;
	CPU_STK * task_stk;
	if(prio){
		ubPrio +=(prio-1);
		for(i=0; i<LWIP_TASK_MAX; ++i)
			if(LwIP_task_priopity_stask[i] == ubPrio)
				break;
		if(i == LWIP_TASK_MAX){
			for(i=0; i<LWIP_TASK_MAX; ++i)
				if(LwIP_task_priopity_stask[i]==0){
					LwIP_task_priopity_stask[i] = ubPrio;
					break;
				}
			if(i == LWIP_TASK_MAX){
				LWIP_ASSERT( "sys_thread_new: there is no space for priority", 0 );
				return (-1);
			}        
		}else
			prio = 0;
	}
/* Search for a suitable priority */     
	if(!prio){
		ubPrio = LWIP_TASK_START_PRIO;
		while(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX)){ 
			for(i=0; i<LWIP_TASK_MAX; ++i)
				if(LwIP_task_priopity_stask[i] == ubPrio){
					++ubPrio;
					break;
				}
			if(i == LWIP_TASK_MAX)
				break;
		}
		if(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX))
			for(i=0; i<LWIP_TASK_MAX; ++i)
				if(LwIP_task_priopity_stask[i]==0){
					LwIP_task_priopity_stask[i] = ubPrio;
					break;
				}
		if(ubPrio >= (LWIP_TASK_START_PRIO+LWIP_TASK_MAX) || i == LWIP_TASK_MAX){
			LWIP_ASSERT( "sys_thread_new: there is no free priority", 0 );
			return (-1);
		}
	}
	if(stacksize > LWIP_STK_SIZE || !stacksize)   
			stacksize = LWIP_STK_SIZE;
  /* get Stack from pool */
	task_stk = OSMemGet( &StackMem, &ucErr );
	if(ucErr != OS_ERR_NONE){
		LWIP_ASSERT( "sys_thread_new: impossible to get a stack", 0 );
		return (-1);
	} 
	tsk_prio = ubPrio-LWIP_TASK_START_PRIO;
	OSTaskCreate(&LwIP_task_TCB[tsk_prio],
							 (CPU_CHAR  *)name,
							 (OS_TASK_PTR)thread, 
							 (void      *)0,
							 (OS_PRIO    )ubPrio,
							 (CPU_STK   *)&task_stk[0],
							 (CPU_STK_SIZE)stacksize/10,
							 (CPU_STK_SIZE)stacksize,
							 (OS_MSG_QTY )0,
							 (OS_TICK    )0,
							 (void      *)0,
							 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR    *)&ucErr);    
	
	return ubPrio;
}

/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	return 1;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
	CPU_SR_ALLOC();

	LWIP_UNUSED_ARG(pval);
	CPU_CRITICAL_EXIT();
}

#else

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*  Porting by Michael Vysotsky <michaelvy@hotmail.com> August 2011   */

#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "includes.h"
//#include "delay.h"
#include "arch/sys_arch.h"
//#include "malloc.h"
#include "os_cfg_app.h"
#include <string.h>

//当消息指针为空时,指向一个常量pvNullPointer所指向的值.
//在UCOS中如果OSQPost()中的msg==NULL会返回一条OS_ERR_POST_NULL
//错误,而在lwip中会调用sys_mbox_post(mbox,NULL)发送一条空消息,我们
//在本函数中把NULL变成一个常量指针0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 

//创建一个消息邮箱
//*mbox:消息邮箱
//size:邮箱大小
//返回值:ERR_OK,创建成功
//         其他,创建失败
err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
	
	OS_ERR err;
	if(size>MAX_QUEUE_ENTRIES)size=MAX_QUEUE_ENTRIES;		//消息队列最多容纳MAX_QUEUE_ENTRIES消息数目 
	OSQCreate((OS_Q*		)mbox,				//消息队列
              (CPU_CHAR*	)"LWIP Quiue",		//消息队列名称
              (OS_MSG_QTY	)size,				//消息队列长度
              (OS_ERR*		)&err);				//错误码
	if(err==OS_ERR_NONE) return ERR_OK;
	return ERR_MEM;
} 
//释放并删除一个消息邮箱
//*mbox:要删除的消息邮箱
void sys_mbox_free(sys_mbox_t * mbox)
{
	OS_ERR err; 
	OSQFlush(mbox,&err);
	OSQDel((OS_Q*	)mbox,
           (OS_OPT	)OS_OPT_DEL_ALWAYS,
           (OS_ERR*	)&err);
	LWIP_ASSERT( "OSQDel ",err == OS_ERR_NONE ); 
}
//向消息邮箱中发送一条消息(必须发送成功)
//*mbox:消息邮箱
//*msg:要发送的消息
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{    
	OS_ERR err;
	CPU_INT08U i=0;
	if(msg==NULL)msg=(void*)&pvNullPointer;	//当msg为空时 msg等于pvNullPointer指向的值 
	//发送消息
    while(i<10)	//试10次
	{
		OSQPost((OS_Q*		)mbox,		
			    (void*		)msg,
			    (OS_MSG_SIZE)strlen(msg),
			    (OS_OPT		)OS_OPT_POST_ALL,
			    (OS_ERR*	)&err);
		if(err==OS_ERR_NONE) break;
		i++;
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err); //延时5ms
	}
	LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  	
}
//尝试向一个消息邮箱发送消息
//此函数相对于sys_mbox_post函数只发送一次消息，
//发送失败后不会尝试第二次发送
//*mbox:消息邮箱
//*msg:要发送的消息
//返回值:ERR_OK,发送OK
// 	     ERR_MEM,发送失败
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
	OS_ERR err;
	if(msg==NULL)msg=(void*)&pvNullPointer;//当msg为空时 msg等于pvNullPointer指向的值 
	OSQPost((OS_Q*		)mbox,		
			(void*		)msg,
			(OS_MSG_SIZE)strlen(msg),
			(OS_OPT		)OS_OPT_POST_ALL,
			(OS_ERR*	)&err);
	if(err!=OS_ERR_NONE) return ERR_MEM;
	return ERR_OK;
}

//等待邮箱中的消息
//*mbox:消息邮箱
//*msg:消息
//timeout:超时时间，如果timeout为0的话,就一直等待
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
	OS_ERR err;
	OS_MSG_SIZE size;
	u32_t ucos_timeout,timeout_new;
	void *temp;
	if(timeout!=0)
	{
		ucos_timeout=(timeout*OS_CFG_TICK_RATE_HZ)/1000; //转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout<1)
		{
			ucos_timeout=1;//至少1个节拍
		}
	}
	else
	{
	        ucos_timeout = 0; 
            timeout = OSTimeGet(&err); //获取系统时间		
	} 
	//请求消息
	temp=OSQPend((OS_Q*			)mbox,   
				(OS_TICK		)ucos_timeout,
                (OS_OPT			)OS_OPT_PEND_BLOCKING,
                (OS_MSG_SIZE*	)&size,		
                (CPU_TS*		)0,
                (OS_ERR*		)&err);
	if(msg!=NULL)
	{	
		if(temp==(void*)&pvNullPointer)
		{
			*msg = NULL;   	//因为lwip发送空消息的时候我们使用了pvNullPointer指针,所以判断pvNullPointer指向的值
		}
 		else 
		{
			*msg=temp;									//就可知道请求到的消息是否有效
		}
	}    
	if(err==OS_ERR_TIMEOUT)
	{
		timeout=SYS_ARCH_TIMEOUT;  //请求超时
	}
	else
	{
		LWIP_ASSERT("OSQPend ",err==OS_ERR_NONE); 
		timeout_new=OSTimeGet(&err);
		if (timeout_new>timeout)
		{			
			timeout_new = timeout_new - timeout;//算出请求消息或使用的时间
		}
		else 
	    {
			timeout_new = 0xffffffff - timeout + timeout_new; 
		}
		timeout=timeout_new*1000/OS_CFG_TICK_RATE_HZ + 1; 
	}
	return timeout; 
}
//尝试获取消息
//*mbox:消息邮箱
//*msg:消息
//返回值:等待消息所用的时间/SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return sys_arch_mbox_fetch(mbox,msg,1);//尝试获取一个消息
}
//检查一个消息邮箱是否有效
//*mbox:消息邮箱
//返回值:1,有效.
//      0,无效
int sys_mbox_valid(sys_mbox_t *mbox)
{  
	if(mbox->NamePtr)  
		return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
	else
		return 0;; 
} 
//设置一个消息邮箱为无效
//*mbox:消息邮箱
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	if(sys_mbox_valid(mbox))
    sys_mbox_free(mbox);
} 
//创建一个信号量
//*sem:创建的信号量
//count:信号量值
//返回值:ERR_OK,创建OK
// 	     ERR_MEM,创建失败
err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
	OS_ERR err;
	OSSemCreate ((OS_SEM*	)sem,
                 (CPU_CHAR*	)"LWIP Sem",
                 (OS_SEM_CTR)count,		
                 (OS_ERR*	)&err);
	if(sem==NULL)return ERR_MEM; 
	LWIP_ASSERT("OSSemCreate ",sem != NULL );
	return ERR_OK;
} 
//等待一个信号量
//*sem:要等待的信号量
//timeout:超时时间
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
	OS_ERR err;
	u32_t ucos_timeout, timeout_new; 
	if(	timeout!=0) 
	{
		ucos_timeout = (timeout * OS_CFG_TICK_RATE_HZ) / 1000;//转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout < 1)
		ucos_timeout = 1;
	}else ucos_timeout = 0; 
	timeout = OSTimeGet(&err);  
	OSSemPend(sem,timeout,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
 	if(err == OS_ERR_TIMEOUT)timeout=SYS_ARCH_TIMEOUT;//请求超时	
	else
	{     
 		timeout_new = OSTimeGet(&err); 
		if (timeout_new>=timeout) timeout_new = timeout_new - timeout;
		else timeout_new = 0xffffffff - timeout + timeout_new;
 		timeout = (timeout_new*1000/OS_CFG_TICK_RATE_HZ + 1);//算出请求消息或使用的时间(ms)
	}
	return timeout;
}
//发送一个信号量
//sem:信号量指针
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR err;
	OSSemPost(sem,OS_OPT_POST_ALL,&err);//发送信号量
	LWIP_ASSERT("OSSemPost ",err == OS_ERR_NONE ); 
}
//释放并删除一个信号量
//sem:信号量指针
void sys_sem_free(sys_sem_t *sem)
{
	OS_ERR err;
	OSSemDel(sem,OS_OPT_DEL_ALWAYS,&err);
    LWIP_ASSERT("OSSemDel ",err==OS_ERR_NONE);
	sem = NULL;
} 
//查询一个信号量的状态,无效或有效
//sem:信号量指针
//返回值:1,有效.
//      0,无效
int sys_sem_valid(sys_sem_t *sem)
{
	if(sem->NamePtr)
		return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
	else
		return 0;           
} 
//设置一个信号量无效
//sem:信号量指针
void sys_sem_set_invalid(sys_sem_t *sem)
{
	if(sys_sem_valid(sem))
     sys_sem_free(sem);
} 
//arch初始化
void sys_init(void)
{ 
    //这里,我们在该函数,不做任何事情
} 
extern CPU_STK * TCPIP_THREAD_TASK_STK;//TCP IP内核任务堆栈,在lwip_comm函数定义
//LWIP内核任务的任务控制块
OS_TCB TcpipthreadTaskTCB;
//创建一个新进程
//*name:进程名称
//thred:进程任务函数
//*arg:进程任务函数的参数
//stacksize:进程任务的堆栈大小
//prio:进程任务的优先级
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	if(strcmp(name,TCPIP_THREAD_NAME)==0)//创建TCP IP内核任务
	{
		OS_CRITICAL_ENTER();	//进入临界区			 
		//创建开始任务
		OSTaskCreate((OS_TCB 	* )&TcpipthreadTaskTCB,			//任务控制块
					 (CPU_CHAR	* )"TCPIPThread task", 			//任务名字
                     (OS_TASK_PTR )thread, 						//任务函数
                     (void		* )0,							//传递给任务函数的参数
                     (OS_PRIO	  )prio,     					//任务优先级
                     (CPU_STK   * )&TCPIP_THREAD_TASK_STK[0],	//任务堆栈基地址
                     (CPU_STK_SIZE)stacksize/10,				//任务堆栈深度限位
                     (CPU_STK_SIZE)stacksize,					//任务堆栈大小
                     (OS_MSG_QTY  )0,							//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                     (OS_TICK	  )0,							//当使能时间片轮转时的时间片长度，为0时为默认长度，
                     (void   	* )0,							//用户补充的存储区
                     (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                     (OS_ERR 	* )&err);					//存放该函数错误时的返回值
		OS_CRITICAL_EXIT();	//退出临界区
	} 
	return 0;
} 
//lwip延时函数
//ms:要延时的ms数
void sys_msleep(u32_t ms)
{
	delay_ms(ms);
}
//获取系统时间,LWIP1.4.1增加的函数
//返回值:当前系统时间(单位:毫秒)
u32_t sys_now(void)
{
	OS_ERR err;
	u32_t ucos_time, lwip_time;
	ucos_time=OSTimeGet(&err);	//获取当前系统时间 得到的是UCSO的节拍数
	lwip_time=(ucos_time*1000/OS_CFG_TICK_RATE_HZ+1);//将节拍数转换为LWIP的时间MS
	return lwip_time; 		//返回lwip_time;
}















































#endif


