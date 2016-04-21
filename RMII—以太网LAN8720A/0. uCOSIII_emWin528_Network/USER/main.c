/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "includes.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/* --------------- APPLICATION GLOBALS ---------------- */
static  OS_TCB       AppTaskStartTCB;
static  CPU_STK      AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB       AppTaskObj0TCB;
static  CPU_STK      AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE];

static  OS_TCB       AppTaskGUITCB;
__align(8) static  CPU_STK      AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  AppTaskStart   (void  *p_arg);
static  void  AppTaskCreate  (void);
static  void  AppObjCreate   (void);

static  void  AppTaskObj0    (void  *p_arg);
static  void  AppTaskGUI     (void  *p_arg);

KEY Key1,Key2;
uint8_t   tpad_flag=0;
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
*********************************************************************************************************
*/

int main(void)
{
	OS_ERR   err;
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_ERR  cpu_err;
#endif                 

	Mem_Init();                                                 /* Initialize Memory Managment Module                   */
	Math_Init();                                                /* Initialize Mathematical Module                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_NameSet((CPU_CHAR *)"STM32F429II",
							(CPU_ERR  *)&cpu_err);
#endif

	BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

	OSInit(&err);                                               /* Init uC/OS-III.                                      */

	App_OS_SetAllHooks();

	OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
								"App Task Start",
								AppTaskStart,
								0u,
								APP_CFG_TASK_START_PRIO,
								&AppTaskStartStk[0u],
								AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 &err);

  if(err!=OS_ERR_NONE)  bsp_result|=BSP_OS;
	OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
	
	while (DEF_ON) {                                            /* Should Never Get Here.                               */
			;
	}
}

//按键定时扫描的定时器
static void inputscan(void)
{
  OS_ERR  err;
  
  OSSchedLock(&err);
	GT9xx_GetOnePiont();//触摸屏定时扫描
  OSSchedUnlock(&err); 
  
	Key_RefreshState(&Key1);//刷新按键状态
	Key_RefreshState(&Key2);//刷新按键状态
	if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//按键被按下过
	{
		LED2_TOGGLE;
		Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
	}
	if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//按键被按下过
	{
		LED3_TOGGLE;
		Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
	}
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{

	OS_ERR      err;
 (void)p_arg;
 
	BSP_Init();                                                 /* Initialize BSP functions                             */
	CPU_Init();                                                 /* Initialize the uC/CPU services                       */

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
	CPU_IntDisMeasMaxCurReset();
#endif

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
#endif	
	AppObjCreate(); 
/* Create Applicaiton kernel objects                    */
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application Tasks\n\r"));
#endif	
	AppTaskCreate();                                            /* Create Application tasks                             */

	
	while (DEF_TRUE)
	{                                          /* Task body, always written as an infinite loop.       */
    inputscan();
		OSTimeDlyHMSM(0u, 0u, 0u, 20u,
									OS_OPT_TIME_HMSM_STRICT,
									&err);
	}
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create Application Tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	OS_ERR  os_err;

	/* ---------- CREATE KERNEL OBJECTS TEST TASK --------- */
	OSTaskCreate((OS_TCB       *)&AppTaskObj0TCB,
               (CPU_CHAR     *)"Kernel Objects Task 0",
               (OS_TASK_PTR   )AppTaskObj0,
               (void         *)0,
               (OS_PRIO       )APP_CFG_TASK_OBJ0_PRIO,
               (CPU_STK      *)&AppTaskObj0Stk[0],
               (CPU_STK_SIZE  )AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE / 10u],
               (CPU_STK_SIZE  )APP_CFG_TASK_OBJ0_STK_SIZE,
               (OS_MSG_QTY    )0u,
               (OS_TICK       )0u,
               (void         *)0,
               (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR       *)&os_err);
  OSTaskCreate((OS_TCB       *)&AppTaskGUITCB,
               (CPU_CHAR     *)"Kernel Objects Task 0",
               (OS_TASK_PTR   )AppTaskGUI,
               (void         *)0,
               (OS_PRIO       )APP_CFG_TASK_GUI_PRIO,
               (CPU_STK      *)&AppTaskGUIStk[0],
               (CPU_STK_SIZE  )AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE / 10u],
               (CPU_STK_SIZE  )APP_CFG_TASK_GUI_STK_SIZE,
               (OS_MSG_QTY    )0u,
               (OS_TICK       )0u,
               (void         *)0,
               (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR       *)&os_err);
}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create Application Kernel Objects.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
  KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
}
/*
*********************************************************************************************************
*                                          AppTaskObj0()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj0' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskObj0 (void  *p_arg)
{
  OS_ERR      err;
	static uint8_t tpad_count=0;
	(void)p_arg;
	
	while(TPAD_Init())
	{
		tpad_count++;
		if(tpad_count>=10)
    {
      bsp_result |= BSP_TPAD;
      break;
		}
     bsp_DelayUS(1000);
	}	
	
	while(1)
	{	
		if((tpad_count<10)&&TPAD_Scan(0))
		{
			BEEP_ON;
      if(UserApp_Flag)tpad_flag=1;
		}
		OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
		BEEP_OFF;
	}   
}

/**************  板载错误   ***************/
void hardwareerr(void)
{
	OS_ERR  err;
  printf("bsp_result->(0x%04X)\n",bsp_result);
  if((bsp_result&(~BSP_NETWORK))==0) return;
  if(!((bsp_result&BSP_SDRAM)||(bsp_result&BSP_GUI)))
  {
    char tempstr[20];
    sprintf(tempstr,"error->(0x%04X)",bsp_result);
    GUI_Clear();
    GUI_SetColor(GUI_RED);
    GUI_SetFont(GUI_FONT_24_ASCII);
		GUI_DispStringHCenterAt(tempstr, 400,120);
    
    LCD_BL_ON();
  }
  while(1)
  {
    printf("bsp_result->(0x%04X)\n",bsp_result);
    LED1_ON;//BEEP_ON;
    OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    LED1_OFF;//BEEP_OFF;	
    OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

/*
*********************************************************************************************************
*                                          AppTaskObj1()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj1' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppTaskGUI (void  *p_arg)
{
  (void)p_arg;
  if(bsp_result!=BSP_OK)
	{
		hardwareerr();
	}
  while(1)
  {
    emWinMainApp();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
