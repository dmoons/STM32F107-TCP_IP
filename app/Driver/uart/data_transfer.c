#include "data_transfer.h"
#include "usart.h"
#include <string.h>
#include "stmflash.h"
#include "tcp_client_demo.h" 
#include "commucation.h"
#include "common.h"
#include "iap_code.h"
#include "products_test.h"
#include "relay.h"

extern unsigned char gMode;

#if USART1_ENABLE
unsigned short gUart1_length = 0;
unsigned char uart1_buffer[UART1_DATA_LENGTH] = {'\0'};
#endif
#if USART2_ENABLE
unsigned short gUart2_length = 0;
unsigned char uart2_buffer[UART2_DATA_LENGTH] = {'\0'};
#endif
#if USART3_ENABLE
unsigned short gUart3_length = 0;
unsigned char uart3_buffer[UART3_DATA_LENGTH] = {'\0'};
#endif
#if USART4_ENABLE
unsigned short gUart4_length = 0;
unsigned char uart4_buffer[UART4_DATA_LENGTH] = {'\0'};
#endif
#if USART5_ENABLE
unsigned short gUart5_length = 0;
unsigned char uart5_buffer[UART5_DATA_LENGTH] = {'\0'};
#endif

unsigned char gUartFlg = 0;

#if USART1_ENABLE
void usr_uart1_receive_timer(unsigned short const msgid)
{
	uart1_buffer[gUart1_length] = '\0';
	QPRINTF("gIndex=%d,receive uart1 data:%s\r\n",gUart1_length,uart1_buffer);

	if(gMode)
	{
		UartATCmd(uart1_buffer,gUart1_length);
		UartCmdPhase(uart1_buffer,gUart1_length);
	}
	else if(!gOtaFlg)
	{	
		gUartFlg = UART_2;
		ShortGetTwoChar(clientCmca.cmd,SEND_SCANNER_DATA_CMD);
		serverCmdProcess();
	}

	memset(uart1_buffer,'\0',gUart1_length);
	gUart1_length = 0;
}
#endif

#if USART2_ENABLE
void usr_uart2_receive_timer(unsigned short const msgid)
{
	uart2_buffer[gUart2_length] = '\0';
	QPRINTF("gIndex=%d,receive uart2 data:%s\r\n",gUart2_length,uart2_buffer);

	if(gMode)
	{
		UartATCmd(uart2_buffer,gUart2_length);
		UartCmdPhase(uart2_buffer,gUart2_length);
	}
	else
	{
		if(!gOtaFlg)
		{
			gUartFlg = UART_1;
			ShortGetTwoChar(clientCmca.cmd,SEND_SCANNER_DATA_CMD);
			serverCmdProcess();
		}
	}

	memset(uart2_buffer,'\0',gUart2_length);
	gUart2_length = 0;
}
#endif

#if USART3_ENABLE
void usr_uart3_receive_timer(unsigned short const msgid)
{
	uart3_buffer[gUart3_length] = '\0';

	memset(uart3_buffer,'\0',gUart3_length);
	gUart3_length = 0;
}
#endif

#if USART4_ENABLE
void usr_uart4_receive_timer(unsigned short const msgid)
{
	uart4_buffer[gUart4_length] = '\0';
	QPRINTF("gIndex=%d,receive uart4 data:%s\r\n",gUart4_length,uart4_buffer);

	if(gMode)
	{
		UartATCmd(uart4_buffer,gUart4_length);
		UartCmdPhase(uart4_buffer,gUart4_length);
	}
	else if(!gOtaFlg)
	{
		gUartFlg = UART_3;
		ShortGetTwoChar(clientCmca.cmd,SEND_SCANNER_DATA_CMD);
		serverCmdProcess();
	}

	memset(uart4_buffer,'\0',gUart4_length);
	gUart4_length = 0;
}
#endif

#if USART5_ENABLE
void usr_uart5_receive_timer(unsigned short const msgid)
{
	uart5_buffer[gUart5_length] = '\0';
	QPRINTF("gIndex=%d,receive uart5 data:%s\r\n",gUart5_length,uart5_buffer);	

	if(gMode)
	{
		UartATCmd(uart5_buffer,gUart5_length);
		UartCmdPhase(uart5_buffer,gUart5_length);
	}
	else if(!gOtaFlg)
	{
		gUartFlg = UART_4;
		ShortGetTwoChar(clientCmca.cmd,SEND_SCANNER_DATA_CMD);
		serverCmdProcess();
	}
	
	memset(uart5_buffer,'\0',gUart5_length);
	gUart5_length = 0;
}
#endif


