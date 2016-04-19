#define DEBUG
#include "drivers.h"
#include "app.h"
#include "api.h"

#include "usr_cfg.h"

struct usr_ctl_key ctl_key;

timer_t key_detect_timer;

void handle_key(int key_type, int key_level, int long_press);

#define LONG_PRESS_HOLD_TIME	20	// 2S
void key_detect_timer_callback(void *arg)
{

	if(ctl_key.key_stat[KEY_USR] !=  BUTTON_STAT)
	{
		if(!BUTTON_STAT)
			ctl_key.press_time_cnt[KEY_USR] = 0;
		else{

			if(ctl_key.press_time_cnt[KEY_USR] < LONG_PRESS_HOLD_TIME)
				handle_key(KEY_USR, BUTTON_STAT, 0);
		}
		ctl_key.key_stat[KEY_USR] = BUTTON_STAT;
	}else{
		if(!BUTTON_STAT){
			if(ctl_key.press_time_cnt[KEY_USR]++ == LONG_PRESS_HOLD_TIME)
				handle_key(KEY_USR, BUTTON_STAT, 1);
		}
	}


}

void init_key_dev()
{
	memset(&ctl_key, 0, sizeof(struct usr_ctl_key));

	gpio_cfg((uint32_t)BUTTON_PORT, BUTTON_PIN, GPIO_Mode_IPU);

	ctl_key.key_stat[KEY_USR] = 1;
	key_detect_timer = timer_setup(100, 1, key_detect_timer_callback, NULL);
	add_timer(key_detect_timer);
}

void handle_key(int key_type, int key_level, int long_press)
{
	uint32_t now_time, time_period;
	if(key_type >= KEY_MAX)
		return;

	now_time = TIM_GetCounter(TIM5);

	time_period = now_time - ctl_key.last_detect_time[key_type];
	if(time_period < 1000){
		//p_err("time_period too short:%d", time_period);
		return;
	}

	ctl_key.last_detect_time[key_type] = now_time;

	p_dbg("key:%d, type:%d", key_type, long_press);
	ctl_key.key_stat[key_type] = key_level;

	switch(key_type)
	{
		case KEY_USR:
		if(!long_press)
		{
//			audio_cfg.recording = !audio_cfg.recording;
//			if(audio_cfg.recording){
//				IND2_ON;
//				p_dbg("start record");
//			}else{
//				IND2_OFF;
//				p_dbg("stop record");
//			}

		}else{
			
			if(wifi_get_mode() == MODE_AP)
			{
//				send_work_event(MODE_DETECT_CHANGE_STA);
			}else{
//				send_work_event(MODE_DETECT_CHANGE_AP);
			}
		}
		break;
		default:
		break;
	}
}



