#define DEBUG

#include "drivers.h"
#include "app.h"
#include "api.h"

timer_t senser_poll_timer;
mutex_t senser_poll_mutex;
//struct _sensers_stat sensers_stat;
struct _sensers_pkg sensers_pkg;

void senser_poll_callback(void *arg)
{
	int ret;
	uint8_t temp[5];
//	uint32_t card_id;
//	uint16_t card_type;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	mutex_lock(senser_poll_mutex);

  	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	
	sensers_pkg.sensers_stat.year = RTC_DateStructure.RTC_Year;
	sensers_pkg.sensers_stat.mon = RTC_DateStructure.RTC_Month;
	sensers_pkg.sensers_stat.date = RTC_DateStructure.RTC_Date;
	sensers_pkg.sensers_stat.week= RTC_DateStructure.RTC_WeekDay;

	sensers_pkg.sensers_stat.hour = RTC_TimeStructure.RTC_Hours;
	sensers_pkg.sensers_stat.min = RTC_TimeStructure.RTC_Minutes;
	sensers_pkg.sensers_stat.sec = RTC_TimeStructure.RTC_Seconds;

	ret = dth11_read_data(temp);
	if(ret == 0){
		sensers_pkg.sensers_stat.temperature = temp[2];
		sensers_pkg.sensers_stat.humidity= temp[0];	
	}
	
	sensers_pkg.pkg_head.msg_id = MQTT_MSG_SENSERS;
	sensers_pkg.pkg_head.dir = MSG_DIR_DEV2APP;
	sensers_pkg.pkg_head.reserved = 0;
	
	send_work_event(SEND_SNESERS_EVENT);

	mutex_unlock(senser_poll_mutex);

	dth11_start_read();
}

void init_sensers(void)
{
//	int ret;

//	init_rc522();
	init_dht11();

	senser_poll_mutex = mutex_init("");

	senser_poll_timer = timer_setup(3000, 1, senser_poll_callback, 0);
	mod_timer(senser_poll_timer, 3000);
	
}

