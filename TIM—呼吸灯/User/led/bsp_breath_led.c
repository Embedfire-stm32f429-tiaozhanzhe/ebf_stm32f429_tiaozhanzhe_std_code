/**
  ******************************************************************************
  * @file    bsp_breath_led.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   呼吸灯驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./led/bsp_breath_led.h"


/* LED亮度等级 PWM表 */
uint8_t indexWave[] = {1,1,2,2,3,4,6,8,10,14,19,25,33,44,59,80,
	107,143,191,255,255,191,143,107,80,59,44,33,25,19,14,10,8,6,4,3,2,2,1,1};



 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( BREATH_LED1_GPIO_CLK|BREATH_LED2_GPIO_CLK|BREATH_LED3_GPIO_CLK|LED4_GPIO_CLK, ENABLE); 

		GPIO_PinAFConfig(BREATH_LED1_GPIO_PORT,BREATH_LED1_PINSOURCE,BREATH_LED1_AF); 
		GPIO_PinAFConfig(BREATH_LED2_GPIO_PORT,BREATH_LED2_PINSOURCE,BREATH_LED2_AF); 
		GPIO_PinAFConfig(BREATH_LED3_GPIO_PORT,BREATH_LED3_PINSOURCE,BREATH_LED3_AF);  
	
		/*BREATH_LED1*/															   
		GPIO_InitStructure.GPIO_Pin = BREATH_LED1_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
		GPIO_Init(BREATH_LED1_GPIO_PORT, &GPIO_InitStructure);	
    
    /*BREATH_LED2*/															   
		GPIO_InitStructure.GPIO_Pin = BREATH_LED2_PIN;	
    GPIO_Init(BREATH_LED2_GPIO_PORT, &GPIO_InitStructure);	
    
    /*BREATH_LED3*/															   
		GPIO_InitStructure.GPIO_Pin = BREATH_LED3_PIN;	
    GPIO_Init(BREATH_LED3_GPIO_PORT, &GPIO_InitStructure);	
		
		/*LED4*/	
		/*设置引脚模式为输出模式 LED4不是定时器的通道，不能使用pwm控制，所以不能用作呼吸灯*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  		
		GPIO_InitStructure.GPIO_Pin = LED4_PIN;	
    GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);	
		
//		/*关闭RGB灯*/
//		LED_RGBOFF;
		
		/*指示灯默认开启*/
		LED4(ON);

}


 /**
  * @brief  通用定时器 TIMx,x[2,3,4,5]中断优先级配置
  * @param  无
  * @retval 无
  */ 
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BREATH_TIM_IRQn; 
	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



/*
 * TIM_Period / Auto Reload Register(ARR) = 256-1   TIM_Prescaler=((SystemCoreClock/2)/1000000)*30-1 
 *  *	
 * 定时器时钟源TIMxCLK = 2 * PCLK1  
 *				PCLK1 = HCLK / 4 
 *				=> TIMxCLK = HCLK / 2 = SystemCoreClock /2
 * 定时器频率为 = TIMxCLK/(TIM_Prescaler+1) = (SystemCoreClock /2)/((SystemCoreClock/2)/1000000)*30 = 1000000/30 = 1/30MHz
 * 中断周期为 = 1/(1/30MHz) * 256 = x ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8,15,16,17]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;


	// 开启TIMx_CLK,x[2,3,4,5] 
  RCC_APB1PeriphClockCmd(BREATH_TIM_CLK, ENABLE); 
	
  /* 累计 TIM_Period个后产生一个更新或者中断*/		 
  TIM_TimeBaseStructure.TIM_Period = 256-1;       //当定时器从0计数到255，即为256次，为一个定时周期
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK = HCLK / 2 = SystemCoreClock /2
	// 定时器频率为 = TIMxCLK/(TIM_Prescaler+1) = (SystemCoreClock /2)/((SystemCoreClock/2)/1000000)*30 = 1000000/30 = 1/30MHz
  /*基本定时器配置TIM_Prescaler根据效果来设置即可，中断周期小 灯闪烁快，大则闪烁缓慢*/
	TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/2)/1000000)*30-1;	    
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BREATH_TIM, &TIM_TimeBaseStructure);	
	
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;	  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR1_Val时为低电平 LED灯亮
  TIM_OC1Init(BREATH_TIM, &TIM_OCInitStructure);	 //使能通道1
  
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(BREATH_TIM, TIM_OCPreload_Enable);
	
	
	TIM_OC2Init(BREATH_TIM, &TIM_OCInitStructure);	 //使能通道2  
	/*使能通道2重载*/
	TIM_OC2PreloadConfig(BREATH_TIM, TIM_OCPreload_Enable);


  TIM_OC3Init(BREATH_TIM, &TIM_OCInitStructure);	 //使能通道3  
	/*使能通道3重载*/
	TIM_OC3PreloadConfig(BREATH_TIM, TIM_OCPreload_Enable);


	TIM_ARRPreloadConfig(BREATH_TIM, ENABLE);			//使能TIM重载寄存器ARR

	// 开启计数器中断
	TIM_ITConfig(BREATH_TIM,TIM_IT_Update,ENABLE);
	
	// 使能计数器
	TIM_Cmd(BREATH_TIM, ENABLE);																		


}

/**
  * @brief  初始化呼吸灯
  * @param  无
  * @retval 无
  */
void BreathLED_Config(void)
{

	TIMx_GPIO_Config();
	
	TIM_Mode_Config();

	TIMx_NVIC_Configuration();	
}

/*********************************************END OF FILE**********************/
