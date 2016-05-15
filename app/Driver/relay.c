#include "relay.h"
#include "usr_task.h"
#include "usart.h"
#include "usr_design.h"
#include "usr_task.h"
#include "commucation.h"
#include "ke_timer.h"

extern void delay(unsigned int time);

unsigned char relay1Flg = 0;
unsigned char relay2Flg = 0;
unsigned char relay3Flg = 0;
unsigned char relay4Flg = 0;

void relay_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE );//PORTE时钟使能 

   GPIO_InitStructure.GPIO_Pin = RELAY_PIN_1 | RELAY_PIN_2 | RELAY_PIN_3 | RELAY_PIN_4;  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE, &GPIO_InitStructure);

   RELAY_PIN_1_LOW;
   RELAY_PIN_2_LOW;
   RELAY_PIN_3_LOW;
   RELAY_PIN_4_LOW;
}

void relaySelect(unsigned int id,unsigned int flg)
{
	if(id-0xfffe0001 > RELAY_MAX)
		return;
	
	switch(usrtMap[id-0xfffe0001][1])
	{
		case RELAY_1:
			if(flg == 0xAABBCC01)
			{
				if(relay1Flg == 1)
				{
					RELAY_PIN_1_LOW;
					delay(100);
				}
				else
					relay1Flg = 1;
				RELAY_PIN_1_HIGH;
				ke_timer_set(USR_RELAY1_TIME_OUT_TIMER,RELAY_TIME_OUT);
			}
			else
				RELAY_PIN_1_LOW;
			break;
			
		case RELAY_2:
			if(flg == 0xAABBCC01)
			{
				if(relay2Flg == 1)
				{
					RELAY_PIN_2_LOW;
					delay(100);
				}
				else
					relay2Flg = 1;
				RELAY_PIN_2_HIGH;
				ke_timer_set(USR_RELAY2_TIME_OUT_TIMER,RELAY_TIME_OUT);
			}
			else
				RELAY_PIN_2_LOW;
			break;

		case RELAY_3:
			if(flg == 0xAABBCC01)
			{
				if(relay3Flg == 1)
				{
					RELAY_PIN_3_LOW;
					delay(100);
				}
				else
					relay3Flg = 1;
				RELAY_PIN_3_HIGH;
				ke_timer_set(USR_RELAY3_TIME_OUT_TIMER,RELAY_TIME_OUT);
			}
			else
				RELAY_PIN_3_LOW;
			break;

		case RELAY_4:
			if(flg == 0xAABBCC01)
			{
				if(relay4Flg == 1)
				{
					RELAY_PIN_4_LOW;
					delay(100);
				}
				else
					relay4Flg = 1;
				RELAY_PIN_4_HIGH;
				ke_timer_set(USR_RELAY4_TIME_OUT_TIMER,RELAY_TIME_OUT);
			}
			else
				RELAY_PIN_4_LOW;
			break;
			
		default:break;
	}
}


void usr_relay1_timeout_timer(unsigned short const msgid)
{
	relay1Flg = 0;
	RELAY_PIN_1_LOW;
}

void usr_relay2_timeout_timer(unsigned short const msgid)
{
	relay2Flg = 0;
	RELAY_PIN_2_LOW;
}

void usr_relay3_timeout_timer(unsigned short const msgid)
{
	relay3Flg = 0;
	RELAY_PIN_3_LOW;
}

void usr_relay4_timeout_timer(unsigned short const msgid)
{
	relay4Flg = 0;
	RELAY_PIN_4_LOW;
}






