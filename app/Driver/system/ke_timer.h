#ifndef _KE_TIMER_H_
#define _KE_TIMER_H_
#include "usr_task.h"

#define TIMER_TICK_10MS_VALUE	(1000)

struct ke_timer
{
	unsigned char flag;
	unsigned short time;
};

extern void ke_timer_set(unsigned short const timer_id,  unsigned short const delay);
extern void ke_timer_clear(unsigned short const timer_id);
extern void ke_schedule(void);
extern void ke_timer_init(void);
extern void timer_tick_set(void);

#endif

