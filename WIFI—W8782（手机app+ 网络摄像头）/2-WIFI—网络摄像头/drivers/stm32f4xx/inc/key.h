#ifndef __DEV_KEY_H
#define	__DEV_KEY_H

enum _KEY_TYPE{
	KEY_USR,
	KEY_MAX
};


struct usr_ctl_key{
	bool key_stat[KEY_MAX];
	int press_time_cnt[KEY_MAX];
	uint32_t last_detect_time[KEY_MAX];
};

void init_key_dev(void);

#endif
