#ifndef _DATA_TRANSFER_H_
#define _DATA_TRANSFER_H_
#include "products.h"

#define UART1_DATA_LENGTH	(256)
#define UART2_DATA_LENGTH	(256)
#define UART3_DATA_LENGTH	(256)
#define UART4_DATA_LENGTH	(256)
#define UART5_DATA_LENGTH	(256)



extern unsigned char gUartFlg;
#if USART1_ENABLE
extern unsigned short gUart1_length;
extern unsigned char uart1_buffer[UART1_DATA_LENGTH];
extern void usr_uart1_receive_timer(unsigned short const msgid);
#endif
#if USART2_ENABLE
extern unsigned short gUart2_length;
extern unsigned char uart2_buffer[UART2_DATA_LENGTH];
extern void usr_uart2_receive_timer(unsigned short const msgid);
#endif
#if USART3_ENABLE
extern unsigned short gUart3_length;
extern unsigned char uart3_buffer[UART3_DATA_LENGTH];
extern void usr_uart3_receive_timer(unsigned short const msgid);
#endif
#if USART4_ENABLE
extern unsigned short gUart4_length;
extern unsigned char uart4_buffer[UART4_DATA_LENGTH];
extern void usr_uart4_receive_timer(unsigned short const msgid);
#endif
#if USART5_ENABLE
extern unsigned short gUart5_length;
extern unsigned char uart5_buffer[UART5_DATA_LENGTH];
extern void usr_uart5_receive_timer(unsigned short const msgid);
#endif

typedef enum
{
	UART_1 = 1,
	UART_2,
	UART_3,
	UART_4,
	UART_MAX
}uart_enu;
#endif

