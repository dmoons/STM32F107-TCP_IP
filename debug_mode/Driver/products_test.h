#ifndef _PRODUCTS_TEST_H_
#define _PRODUCTS_TEST_H_
#include "products.h"

#if PRODUCTS_TEST_ENABLE

#define IP_SIZE			(4)
#define PORT_SIZE		(2)
#define SERVER_TCP_IP_ADDR		(0)
#define SERVER_TCP_IP_SIZE		(IP_SIZE)
#define SERVER_PORT_ADDR		(SERVER_TCP_IP_ADDR+SERVER_TCP_IP_SIZE)
#define SERVER_PORT_SIZE		(PORT_SIZE)

#define LOCAL_TCP_IP_ADDR		(SERVER_PORT_ADDR+SERVER_PORT_SIZE)
#define LOCAL_TCP_IP_SIZE		(IP_SIZE)
#define LOCAL_NETMASK_ADDR		(LOCAL_TCP_IP_ADDR+LOCAL_TCP_IP_SIZE)
#define LOCAL_NETMASK_SIZE		(IP_SIZE)
#define LOCAL_GW_ADDR			(LOCAL_NETMASK_ADDR+LOCAL_NETMASK_SIZE)
#define LOCAL_GW_SIZE			(IP_SIZE)
#define LOCAL_PORT_ADDR			(LOCAL_GW_ADDR+LOCAL_GW_SIZE)
#define LOCAL_PORT_SIZE			(PORT_SIZE)

#define UART_MAP_GROUP			(4)
#define UART_MAP_GROUP_SIZE		(2)
#define UART_MAP_ADDR			(LOCAL_PORT_ADDR+LOCAL_PORT_SIZE)
#define UART_MAP_SIZE			(UART_MAP_GROUP*UART_MAP_GROUP_SIZE)

#define SETTING_BUFFER_SIZE		(UART_MAP_ADDR+UART_MAP_SIZE)


#define AT_HEAD_ADDR	(0)
#define AT_HEAD_SIZE	(3)
#define AT_DICT_ADDR	(AT_HEAD_ADDR+AT_HEAD_SIZE)
#define AT_DICT_SIZE	(1)
#define AT_CMD_ADDR		(AT_DICT_ADDR+AT_DICT_SIZE)
#define AT_CMD_MAX_SIZE	(16)

#define AT_DATA_MAC_COUNT	(5)
#define AT_DATA_MAX_SIZE	(16)


typedef enum
{
	AT_DICT_S = 1,
	AT_DICT_G,
	AT_DICT_MAX
}at_dict;

typedef enum
{
	LOCAL_IP = 1,
	SERVER_IP,
	UART_MAP,
	MCU_VERSION,
	MCU_MAC,
	AT_MAX
	
}at_cmd;


void mode_io_init(void);
void writeServerTcpIpToflash(unsigned char *data);
void writeLocalTcpIpToflash(unsigned char *data);
void writeUartMapToflash(unsigned char *data);
void UartATCmd(unsigned char *data,unsigned short length);
void usr_test_sendData_timer(unsigned short const msgid);
void UartCmdPhase(unsigned char *data,unsigned short length);


#endif


#endif



