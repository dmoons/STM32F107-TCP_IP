#ifndef _RELAY_H_
#define _RELAY_H_
#include "stm32f10x.h"

#define RELAY_PIN_1	GPIO_Pin_15
#define RELAY_PIN_2	GPIO_Pin_14
#define RELAY_PIN_3	GPIO_Pin_12
#define RELAY_PIN_4	GPIO_Pin_10

#define RELAY_PIN_1_HIGH	GPIO_SetBits(GPIOE,RELAY_PIN_1)
#define RELAY_PIN_1_LOW		GPIO_ResetBits(GPIOE,RELAY_PIN_1)

#define RELAY_PIN_2_HIGH	GPIO_SetBits(GPIOE,RELAY_PIN_2)
#define RELAY_PIN_2_LOW		GPIO_ResetBits(GPIOE,RELAY_PIN_2)

#define RELAY_PIN_3_HIGH	GPIO_SetBits(GPIOE,RELAY_PIN_3)
#define RELAY_PIN_3_LOW		GPIO_ResetBits(GPIOE,RELAY_PIN_3)

#define RELAY_PIN_4_HIGH	GPIO_SetBits(GPIOE,RELAY_PIN_4)
#define RELAY_PIN_4_LOW		GPIO_ResetBits(GPIOE,RELAY_PIN_4)

#define RELAY_TIME_OUT		(5*100) // 10 secs

typedef enum
{
	RELAY_1 =1,
	RELAY_2,
	RELAY_3,
	RELAY_4,
	RELAY_MAX
}relay_enum;
void relay_gpio_init(void);
void relaySelect(unsigned int id,unsigned int flg);
void usr_relay1_timeout_timer(unsigned short const msgid);
void usr_relay2_timeout_timer(unsigned short const msgid);
void usr_relay3_timeout_timer(unsigned short const msgid);
void usr_relay4_timeout_timer(unsigned short const msgid);

#endif


