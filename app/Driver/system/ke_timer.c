#include "ke_timer.h"

struct ke_timer usr_timer[USR_TIMER_MAX] = {{0,0}};
static unsigned char gTimerTickFlg = 0;
static unsigned short gUsrUsingTimerCount = 0;

void ke_timer_set(unsigned short const timer_id,  unsigned short const delay)
{
	usr_timer[timer_id].flag = 1;
	usr_timer[timer_id].time = delay;
}

void ke_timer_clear(unsigned short const timer_id)
{
	usr_timer[timer_id].flag = 0;
}

void ke_schedule(void)
{	
	unsigned short timer_id = 0,fun_id = 0;
	if(gTimerTickFlg)
	{
		gTimerTickFlg = 0;
		for(timer_id=0;timer_id<gUsrUsingTimerCount;timer_id++)
		{
			if(usr_timer[timer_id].flag)
			{
				if(usr_timer[timer_id].time > 0)
					usr_timer[timer_id].time--;
			}
		}
	}
	
	for(timer_id=0;timer_id<gUsrUsingTimerCount;timer_id++)
	{
		if(usr_timer[timer_id].flag)
		{
			if(usr_timer[timer_id].time == 0)
			{
				usr_timer[timer_id].flag = 0;
				for(fun_id=0;fun_id<gUsrUsingTimerCount;fun_id++)
				{
					if(usr_timer_state[fun_id].id == timer_id)
					{
						usr_timer_state[fun_id].func(timer_id);
					}
				}
			}
		}
	}
}

void timer_tick_set(void)
{
	gTimerTickFlg = 1;
}

void ke_timer_init(void)
{
	gUsrUsingTimerCount = get_usr_using_timer_count();

}





