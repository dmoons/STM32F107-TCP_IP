#include "usr_task.h"
#include "ke_timer.h"
#include "usart.h"
#include "data_transfer.h"
#include "usr_design.h"
#include "iap_code.h"
#include "flash.h"
#include "relay.h"
#include "products_test.h"

void usr_test_timer(unsigned short const msgid)
{
	QPRINTF("usr timer0\r\n");
}

const ke_msg_handler usr_timer_state[] =
{
    {USR_TIMER0,     					(ke_msg_func_t) usr_test_timer},		
#if USART1_ENABLE
	{USR_UART1_RECEIVE_TIMER,     		(ke_msg_func_t) usr_uart1_receive_timer},
#endif
#if USART2_ENABLE
	{USR_UART2_RECEIVE_TIMER,     		(ke_msg_func_t) usr_uart2_receive_timer},
#endif
#if USART3_ENABLE
	{USR_UART3_RECEIVE_TIMER,     		(ke_msg_func_t) usr_uart3_receive_timer},
#endif
#if USART4_ENABLE
	{USR_UART4_RECEIVE_TIMER,     		(ke_msg_func_t) usr_uart4_receive_timer},
#endif
#if USART5_ENABLE
    {USR_UART5_RECEIVE_TIMER,     		(ke_msg_func_t) usr_uart5_receive_timer},
#endif
    {USR_RECEIVE_SERVER_TIMER,     		(ke_msg_func_t) usr_receive_server_timer},
    {USR_SYSTEM_RESET_TIMER,     		(ke_msg_func_t) usr_system_reset_timer},
    {USR_OTA_TIME_OUT_TIMER,     		(ke_msg_func_t) usr_ota_time_out_timer},
    {USR_RELAY1_TIME_OUT_TIMER,     	(ke_msg_func_t) usr_relay1_timeout_timer},
    {USR_RELAY2_TIME_OUT_TIMER,     	(ke_msg_func_t) usr_relay2_timeout_timer},
    {USR_RELAY3_TIME_OUT_TIMER,     	(ke_msg_func_t) usr_relay3_timeout_timer},
    {USR_RELAY4_TIME_OUT_TIMER,     	(ke_msg_func_t) usr_relay4_timeout_timer},
    {USR_TCP_IP_LINK_TIMER,     		(ke_msg_func_t) usr_tcp_ip_link_timer},
    {USR_TEST_SEND_TIMER,				(ke_msg_func_t) usr_test_sendData_timer},
    {USR_TCP_IP_DICONNECTTION_TIMER,	(ke_msg_func_t) usr_tcpip_timeout_timer},
};

unsigned short get_usr_using_timer_count(void)
{
	return (unsigned short)(sizeof(usr_timer_state) / sizeof(ke_msg_handler));
}


