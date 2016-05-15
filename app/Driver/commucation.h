#ifndef _COMMUCATION_H_
#define _COMMUCATION_H_
#include "products.h"

#define HEAD_SIZE				(4)
#define IPADDR_SIZE				(4)
#define PORT_SIZE				(2)
#define TIME_SIZE				(4)
#define CMD_SIZE				(2)
#define CURRENT_PACKET_SIZE		(2)
#define TOTAL_PACKET_SIZE		(2)
#define LENGTH_SIZE				(2)
#define CRC_SIZE				(4)

#define SRC_IPADDR_OFFSET		(HEAD_SIZE)
#define SRC_PORT_OFFSET			(SRC_IPADDR_OFFSET		+IPADDR_SIZE)
#define DEST_IPADDR_OFFSET		(SRC_PORT_OFFSET		+PORT_SIZE)
#define DEST_PORT_OFFSET		(DEST_IPADDR_OFFSET		+IPADDR_SIZE)
#define TIME_OFFSET				(DEST_PORT_OFFSET		+PORT_SIZE)
#define CMD_OFFSET				(TIME_OFFSET			+TIME_SIZE)
#define CURRENT_PACKET_OFFSET	(CMD_OFFSET				+CMD_SIZE)
#define TOTAL_PACKET_OFFSET		(CURRENT_PACKET_OFFSET	+CURRENT_PACKET_SIZE)
#define LENGTH_OFFSET			(TOTAL_PACKET_OFFSET	+TOTAL_PACKET_SIZE)
#define CRC_OFFSET				(LENGTH_OFFSET			+LENGTH_SIZE)
#define DATA_OFFSET				(CRC_OFFSET				+CRC_SIZE)

#define PACKET_DATA_LENGTH				(512)

typedef enum
{
    NACK = 0,					//应答错误的ACK及Ack错误类型
	ACK ,						//应答正确的ACK及Ack内容
	GET_MCU_VERSION_CMD,		//获取远程单片机的版本
	GET_MCU_IP_CMD,				//获取远程单片机的IP地址
	GET_MCU_MAC_CMD,			//获取远程单片机的mac地址

	SET_MCU_IAP_CMD	,			//服务器发送升级包给单片机
	SET_MCU_RESET_CMD,			//设置远程单片机进行复位重启
	SET_SCANNER_CMD,			//设置远程扫描仪,功能保留
	
	SEND_SCANNER_DATA_CMD  =  0x0100,		//发送扫描仪的数据给服务器(1包目前规定为512byte)
	SEND_UPDATED_STATUS_CMD,				//发送升级是否成功的状态给服务器
	SEND_UPDATE_VERSION_CMD,				//服务器发送升级成功后的版本单板
	
	SEND_HEART_BEAT_CMD		= 0x1000,	//心跳包

	#if PRODUCTS_TEST_ENABLE
	TEST_SET_MCU_LINK_TCPIP_PORT_CMD = 0xA000,
	TSET_SET_MCU_LOCAL_TCPIP_PORT_CMD,
	TEST_SET_MCU_UART_CMD,
	TEST_SEND_DATA_CMD,

	TEST_GET_MCU_LINK_TCPIP_PORT_CMD = 0xB000,
	TSET_GET_MCU_LOCAL_TCPIP_PORT_CMD,
	TEST_GET_MCU_UART_MAP_CMD,
	TEST_GET_MCU_MAC_CMD,
	#endif

	SERVER_CMD_MAX
}protocol;

typedef enum
{
	TIME_OUT_ACK = 0,
	DATA_ERROR_ACK,
	NACK_MAX
}nAck;

typedef struct
{	
	const unsigned char head[HEAD_SIZE];
	unsigned char src_ipaddr[IPADDR_SIZE];
	unsigned char src_port[PORT_SIZE];
	unsigned char dest_ipaddr[IPADDR_SIZE];
	unsigned char dest_port[PORT_SIZE];
	unsigned char time_stamp[TIME_SIZE];
	unsigned char cmd[CMD_SIZE];
	unsigned char current_packet[CURRENT_PACKET_SIZE];
	unsigned char total_packet[TOTAL_PACKET_SIZE];
	unsigned char data_length[LENGTH_SIZE];
	unsigned char crc[CRC_SIZE];
	unsigned char *data_buf;
	
}commucation_struct;

void clientBufInit(void);

extern unsigned char clientSendBuffer[DATA_OFFSET+512];
extern unsigned int sendCount,ackCount,nAckCount;
extern unsigned int LinkCount;

extern commucation_struct clientCmca;

unsigned char receive_server_data(void);
void serverCmdProcess(void);

#endif

