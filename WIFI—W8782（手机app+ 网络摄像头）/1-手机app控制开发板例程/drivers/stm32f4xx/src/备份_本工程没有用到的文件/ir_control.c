#define DEBUG
#include "drivers.h"
#include "bsp.h"

#include "debug.h"

#define TEST_TIMER5

#define MAX_SEQ_NUM 256
uint32_t send_seq[MAX_SEQ_NUM], seq_pos = 0, send_pos = 0;
bool ir_send_pending = FALSE, ir_send_val = FALSE;

uint8_t ir_recv_data[16];

static void ir_timer_callback(void);

__inline void ir_pwm_enable()
{
	//TIM_Cmd(TIM1, ENABLE);
	TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
}

__inline void ir_pwm_disable()
{
	//TIM_Cmd(TIM1, DISABLE);
	//TIM_ForcedOC3Config(TIM1, TIM_ForcedAction_InActive);
	TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_Inactive);
}

void init_ir()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	uint32_t apbclock, scaler = 0, Counter;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
		
	TIM_DeInit(TIM1); 
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	Counter = apbclock*2/(scaler + 1)/38000;

	GPIO_PinAFConfig(GPIOB, IR_CON_PIN_SOURCE, GPIO_AF_TIM1); 

	gpio_cfg((uint32_t)IR_CON_PORT_GROUP, IR_CON_PIN, GPIO_Mode_AF_PP);
	
	TIM_TimeBaseStructure.TIM_Period = Counter;
 	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Counter/2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_Low;        //设置互补端输出极性
        TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;//使能互补端输出
        TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;        //死区后输出状态
        TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//死区后互补端输出状态
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	 TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//运行模式下输出选择 
        TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//空闲模式下输出选择 
        TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;         //锁定设置
        TIM_BDTRInitStructure.TIM_DeadTime = 0x90;                                         //死区时间设置
        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;                 //刹车功能使能
        TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;//刹车输入极性
        TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//自动输出使能 
        TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);

	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);       

	ir_pwm_disable();
	
	start_timer(1000, ir_timer_callback);
	pause_timer();
	
	ir_recv_enable();
}

__inline void ir_trig_send(void)
{
	if(send_pos < seq_pos){
		if(ir_send_val)
			ir_pwm_enable();
		else
			ir_pwm_disable();
		ir_send_val = !ir_send_val;
		//p_dbg("[%d]", send_seq[send_pos]);
		mode_timer_by_counter(send_seq[send_pos++]);
	}else{
		ir_send_pending = FALSE;
		ir_pwm_disable();
		pause_timer();
		p_dbg("ir send end");
	}
}

void ir_timer_callback()
{
	pause_timer();
	ir_trig_send();
#if 0  //for test
	if(ir_send_val)
		ir_pwm_enable();
	else
		ir_pwm_disable();
	ir_send_val = !ir_send_val;
#endif
}


//value us
void ir_fill_seq(uint32_t value)
{
	uint32_t timer_clk;
	if(seq_pos < MAX_SEQ_NUM){
		timer_clk = get_hard_timer_clk();
		send_seq[seq_pos++] = timer_clk*2/1000000*value;
	}else
		p_err("fill seq over");
}

void rst_seq_pos()
{
	seq_pos = 0;
}

void ir_fill_nec_preamble()
{
	rst_seq_pos();
	ir_fill_seq(4500);
	ir_fill_seq(4500);
}


//结束发送，需要一个下降沿
void ir_fill_nec_tail()
{
	ir_fill_seq(500);
}

void ir_fill_nec_data0()
{
	ir_fill_seq(560);
	ir_fill_seq(565);
}

void ir_fill_nec_data1()
{
	ir_fill_seq(560);
	ir_fill_seq(1685);
}

void ir_fill_nec_byte(uint8_t value)
{
	int i = 8;
	while(i--)
	{
		if(value & 0x80)
			ir_fill_nec_data1();
		else
			ir_fill_nec_data0();

		value = value << 1;
	}
}

void ir_start_send()
{
	p_dbg("ir send start");
	send_pos = 0;
	ir_send_pending = TRUE;
	ir_send_val = 1;
	ir_trig_send();
}

void ir_send_nec_data(uint8_t *data, int len)
{
	int i;
	if(ir_send_pending)
		return;
	
	ir_fill_nec_preamble();
	/*
	ir_fill_nec_byte(usr_code);
	ir_fill_nec_byte(~usr_code);
	ir_fill_nec_byte(data);
	ir_fill_nec_byte(~data);*/
	for(i = 0; i < len; i++)
		ir_fill_nec_byte(data[i]);
	
	ir_fill_nec_tail();
	ir_start_send();
}


/*
*红外接收功能初始化，这里用TIM5作为计数
*
*/
void ir_recv_enable()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
  	NVIC_InitTypeDef   NVIC_InitStructure;

	gpio_cfg((uint32_t)IR_IN_PORT_GROUP, IR_IN_PIN, GPIO_Mode_IPU);	

	
	SYSCFG_EXTILineConfig(IR_IN_SOURCE_GROUP, IR_IN_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = IR_IN_LINE;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  	//红外接收探头默认是高电平
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI15_10_IRQn_Priority;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void ir_recv_disable()
{
	NVIC_DisableIRQ(EXTI15_10_IRQn);
}


void handle_ir_recv(uint32_t time)
{
	static int cnt = 0, cnt_ex = 0;
	static uint8_t data, bit;

	printf("%d ", time/100);
	switch(cnt)
	{
		case 0:
		//if(time < 14000 && time > 13000)		// 13.5ms,
		if(time < 14000 && time > 5000)
		{
			cnt = 1;
			cnt_ex = 0;
			data = 0;
		}else
			;		// 第一个下降沿肯定是个非法值，忽略掉就行
		break;
		default:
		{
			if(time > 900 && time < 1300)		// 1.125ms
				bit = 0;
			else if(time > 2000 && time < 2500)	// 2.25ms
				bit = 1;
			else{
				cnt = 0;		// 第一个下降沿也可能出现在这里
				break;
			}
			if(cnt_ex++ < 8)
			{
				data <<= 1;
				data |=  bit;
			}

			if(cnt_ex == 8)
			{
				printf("%x ", data);
				ir_recv_data[cnt - 1] = data;
				cnt++;
				cnt_ex = 0;
				data = 0;

				// 接收的数据不能超过ir_recv_data的大小
				if(cnt >= 17)
				{
					p_dbg("ir recv over");
					cnt = 0;
					break;
				}
			}
		}
		break;
	}
	
}

void __EXTI14_IRQHandler(void)
{

	static uint32_t last_time = 0;
	uint32_t time;
	time = get_us_count();

	//转换成us
	handle_ir_recv(time - last_time);
	last_time = time;

	
}


