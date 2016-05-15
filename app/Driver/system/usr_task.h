#ifndef _USR_TASK_H_
#define _USR_TASK_H_
#include "products.h"

//usr timer msg
enum
{
	USR_TIMER0 = 0,
#if USART1_ENABLE
	USR_UART1_RECEIVE_TIMER,
#endif
#if USART2_ENABLE
	USR_UART2_RECEIVE_TIMER,
#endif
#if USART3_ENABLE
	USR_UART3_RECEIVE_TIMER,
#endif
#if USART4_ENABLE
	USR_UART4_RECEIVE_TIMER,
#endif
#if USART5_ENABLE
	USR_UART5_RECEIVE_TIMER,
#endif
	USR_RECEIVE_SERVER_TIMER,
	USR_SYSTEM_RESET_TIMER,
	USR_OTA_TIME_OUT_TIMER,
	USR_RELAY1_TIME_OUT_TIMER,
	USR_RELAY2_TIME_OUT_TIMER,
	USR_RELAY3_TIME_OUT_TIMER,
	USR_RELAY4_TIME_OUT_TIMER,
	USR_TCP_IP_LINK_TIMER,
	USR_TEST_SEND_TIMER,
	USR_TCP_IP_DICONNECTTION_TIMER,
	USR_TIMER_MAX
};

typedef void (*ke_msg_func_t)(unsigned short const msgid);
typedef struct
{
    unsigned short id;
    ke_msg_func_t func;
}ke_msg_handler;

extern const ke_msg_handler usr_timer_state[];
extern unsigned short get_usr_using_timer_count(void);

#endif

