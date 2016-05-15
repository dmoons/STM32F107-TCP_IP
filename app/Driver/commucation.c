#include "commucation.h"
#include "common.h"
#include "data_transfer.h"
#include "usart.h"
#include "usr_design.h"
#include "tcp_client_demo.h" 
#include "ke_timer.h"
#include "crc.h"
#include "iap_code.h"
#include "relay.h"
#include "products_test.h"

#define CRC_INIT_VAL			(0x0000)
extern unsigned char gMode;
extern ota_struct gOTA;

unsigned char clientSendBuffer[DATA_OFFSET+PACKET_DATA_LENGTH];
commucation_struct clientCmca ={0x1a,0x2b,0x3c,0x4d};
unsigned char uartBuf[PACKET_DATA_LENGTH];

void clientBufInit(void)
{
	clientCmca.data_buf = &uartBuf[0];
}

#if LWIP_DHCP
static void getLocalIPAndPort(void)
{
	clientIPAddr[0] = tcp_client_pcb->local_ip.addr&0xff;
	clientIPAddr[1] = (tcp_client_pcb->local_ip.addr>>8)&0xff;
	clientIPAddr[2] = (tcp_client_pcb->local_ip.addr>>16)&0xff;
	clientIPAddr[3] = (tcp_client_pcb->local_ip.addr>>24)&0xff;

	clientPort[0] 	= (tcp_client_pcb->local_port>>8)&0xff;
	clientPort[1] 	= (tcp_client_pcb->local_port)&0xff;
}
#endif

static void clientSendDataToServer(void)
{
	unsigned short dataLen=0,i;
	
	dataLen = TwoCharGetShort(clientCmca.data_length);	
	
	for(i=0;i<DATA_OFFSET;i++)
		clientSendBuffer[i] = (&(clientCmca.head[0]))[i];

	for(i=0;i<dataLen;i++)
		clientSendBuffer[DATA_OFFSET+i] = clientCmca.data_buf[i];

	TCP_Client_Send_Data(clientSendBuffer,dataLen+DATA_OFFSET);
}

static void fullTcpIpInfo(void)
{
	unsigned short i;
	
	#if LWIP_DHCP
	getLocalIPAndPort();
	#endif
	
	for(i=0;i<IPADDR_SIZE;i++)
		clientCmca.src_ipaddr[i] 	= clientIPAddr[i];

	for(i=0;i<PORT_SIZE;i++)
		clientCmca.src_port[i] 		= clientPort[i];

	for(i=0;i<IPADDR_SIZE;i++)
		clientCmca.dest_ipaddr[i] 	= serverIPAddr[i];

	for(i=0;i<PORT_SIZE;i++)
		clientCmca.dest_port[i] 	= serverPort[i];
}

unsigned char receive_server_data(void)
{
	unsigned short i=0,dataLength=0;
	if(clientSendBuffer[0] == 0x1A && clientSendBuffer[1] == 0x2B && clientSendBuffer[2] == 0x3C && clientSendBuffer[3] == 0x4d)
	{
		for(i=0;i<IPADDR_SIZE;i++)
			clientCmca.src_ipaddr[i] = clientSendBuffer[SRC_IPADDR_OFFSET+i];

		for(i=0;i<PORT_SIZE;i++)
			clientCmca.src_port[i]	 = clientSendBuffer[SRC_PORT_OFFSET+i];

		for(i=0;i<IPADDR_SIZE;i++)
			clientCmca.dest_ipaddr[i]= clientSendBuffer[DEST_IPADDR_OFFSET+i];

		for(i=0;i<PORT_SIZE;i++)
			clientCmca.dest_port[i] = clientSendBuffer[DEST_PORT_OFFSET+i];

		for(i=0;i<TIME_SIZE;i++)
			clientCmca.time_stamp[i] = clientSendBuffer[TIME_OFFSET+i];
		
		for(i=0;i<CMD_SIZE;i++)
			clientCmca.cmd[i] 		= clientSendBuffer[CMD_OFFSET+i];

		
		for(i=0;i<CURRENT_PACKET_SIZE;i++)
			clientCmca.current_packet[i]	= clientSendBuffer[CURRENT_PACKET_OFFSET+i];

		for(i=0;i<TOTAL_PACKET_SIZE;i++)
			clientCmca.total_packet[i]	= clientSendBuffer[TOTAL_PACKET_OFFSET+i];

		for(i=0;i<LENGTH_SIZE;i++)
			clientCmca.data_length[i] 	= clientSendBuffer[LENGTH_OFFSET+i];

		for(i=0;i<CRC_SIZE;i++)
			clientCmca.crc[i] 	= clientSendBuffer[CRC_OFFSET+i];

		dataLength = TwoCharGetShort(clientCmca.data_length);
		for(i=0;i<dataLength;i++)
			clientCmca.data_buf[i] 	= clientSendBuffer[DATA_OFFSET+i];
		
		return 1;
	}
	return 0;
}

void serverCmdProcess(void)
{
	unsigned short i,j,cmd,sendFlg=0;
	unsigned short ackCmd;
	unsigned int uartCmd,openFlg;
    unsigned short currentPacket,totalPacket,packetLength,otaStatus;
	unsigned int crc=0;
	unsigned short dataLength;
	cmd = TwoCharGetShort(clientCmca.cmd);

	switch(cmd)
	{
		case NACK:	
			ackCmd = TwoCharGetShort(clientCmca.data_buf);
			QPRINTF("#############ackCmd=0x%04x,\r\n",ackCmd);
			switch(ackCmd)
			{
				case SEND_SCANNER_DATA_CMD:
					QPRINTF("************send scan data is error\r\n");
					
					break;

				#if PRODUCTS_TEST_ENABLE
				case TEST_SEND_DATA_CMD:
					nAckCount++;
					break;
				#endif
					
				default:
					break;
			}
			break;
			
		case ACK:
			ackCmd = TwoCharGetShort(clientCmca.data_buf);
			QPRINTF("#############ackCmd=0x%04x,\r\n",ackCmd);
			switch(ackCmd)
			{
				case SEND_SCANNER_DATA_CMD:
					uartCmd = FourCharGetInt(&(clientCmca.data_buf[2]));
					openFlg = FourCharGetInt(&(clientCmca.data_buf[6]));
					QPRINTF("uartCmd=0x%x ,openFlg=0x%x\r\n",uartCmd,openFlg);
					switch(uartCmd)
					{
						case 0xfffe0001:
						case 0xfffe0002:
						case 0xfffe0003:
						case 0xfffe0004:
							relaySelect(uartCmd,openFlg);
							break;
					
						default:break;
					}
					break;

				case SEND_HEART_BEAT_CMD:
					QPRINTF("TCP IP Linked status\r\n");
					LinkStatus = 1;
					LinkCount = 0;
					ke_timer_clear(USR_TCP_IP_DICONNECTTION_TIMER);
					break;
					
				#if PRODUCTS_TEST_ENABLE
				case TEST_SEND_DATA_CMD:
					ackCount++;
					break;
				#endif
				
				default:
					break;
			}
			break;
			
		case GET_MCU_VERSION_CMD:
			QPRINTF("GET_MCU_VERSION_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 26);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_VERSION_CMD);
				
				for(i=0;i<12;i++)
					clientCmca.data_buf[i+2] = softVersion[i];	
				for(i=0;i<12;i++)
					clientCmca.data_buf[i+14] = hardVersion[i];	
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 26));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_VERSION_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;
			
		case GET_MCU_IP_CMD:
			QPRINTF("GET_MCU_MAC_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 16);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_IP_CMD);
				
				j= 2;
				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = clientIPAddr[i];

				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = netmaskAddr[i];

				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = gwAddr[i];

				for(i=0;i<2;i++)
					clientCmca.data_buf[j++] = clientPort[i];	
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 16));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_IP_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;
			
		case GET_MCU_MAC_CMD:
			QPRINTF("GET_MCU_MAC_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 8);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_MAC_CMD);
				
				for(i=0;i<6;i++)
					clientCmca.data_buf[i+2] = macAddr[i];	
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 8));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],GET_MCU_MAC_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;
		case SET_MCU_IAP_CMD:
			currentPacket 	= TwoCharGetShort(clientCmca.current_packet);
			totalPacket 	= TwoCharGetShort(clientCmca.total_packet);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				if(currentPacket == 0 && totalPacket > 0)
				{
					OTA_start(totalPacket);
				}

				otaStatus = OTA_Process(&clientCmca.data_buf[0],packetLength);
				if((currentPacket+1) == totalPacket && totalPacket > 0)
				{
					OTA_end(1);
				}

				if(otaStatus == 0)
				{
					ShortGetTwoChar(clientCmca.cmd ,ACK);
					ShortGetTwoChar(clientCmca.data_length , 2);
					ShortGetTwoChar(&clientCmca.data_buf[0],SET_MCU_IAP_CMD);
					IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 2));
				}
				else
				{
					ShortGetTwoChar(clientCmca.cmd ,NACK);
					ShortGetTwoChar(clientCmca.data_length , 3);
					ShortGetTwoChar(&clientCmca.data_buf[0],SET_MCU_IAP_CMD);
					clientCmca.data_buf[2] = TIME_OUT_ACK;//time out
					IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
				}
			}
			else 
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],SET_MCU_IAP_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//time out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case SEND_UPDATE_VERSION_CMD:
			QPRINTF("SEND_UPDATE_VERSION_CMD\r\n");
			currentPacket 	= TwoCharGetShort(clientCmca.current_packet);
			totalPacket 	= TwoCharGetShort(clientCmca.total_packet);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				QPRINTF("if(crc == FourCharGetInt(clientCmca.crc)) gOTA.ota_end_flg=%d,\r\n",gOTA.ota_end_flg);
				if(gOTA.ota_end_flg)
					OTA_Version_Update(&clientCmca.data_buf[0]);
			}
			else 
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],SEND_UPDATE_VERSION_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//time out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
				sendFlg = 1;
			}		
			break;
				
		case SET_MCU_RESET_CMD:
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			ShortGetTwoChar(clientCmca.cmd ,ACK);
			ShortGetTwoChar(clientCmca.data_length , 2);
			ShortGetTwoChar(&clientCmca.data_buf[0],SET_MCU_RESET_CMD);
			IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 2));
			sendFlg = 1;
			ke_timer_set(USR_SYSTEM_RESET_TIMER,10);
			break;
			
		case SET_SCANNER_CMD:
			break;
			
		case SEND_SCANNER_DATA_CMD:
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			ShortGetTwoChar(clientCmca.cmd ,SEND_SCANNER_DATA_CMD);
			clientCmca.data_buf = &uartBuf[0];
			clientCmca.data_buf[0] = 0xFF;
			clientCmca.data_buf[1] = 0xFE;
			clientCmca.data_buf[2] = 0x00;
			QPRINTF("gUartFlg=%d,\r\n",gUartFlg);
			switch(gUartFlg)
			{
				case UART_1:
					#if USART1_ENABLE
					dataLength = gUart2_length;
					clientCmca.data_buf[3] = UART_1;
					for(i=0;i<gUart2_length;i++)
						clientCmca.data_buf[4+i] = uart2_buffer[i];
					#endif
					break;
					
				case UART_2:
					#if USART4_ENABLE
					dataLength = gUart1_length;
					clientCmca.data_buf[3] = UART_2;
					for(i=0;i<gUart1_length;i++)
						clientCmca.data_buf[4+i] = uart1_buffer[i];
					#endif
					break;
					
				case UART_3:
					#if USART4_ENABLE
					dataLength = gUart4_length;
					clientCmca.data_buf[3] = UART_3;
					for(i=0;i<gUart4_length;i++)
						clientCmca.data_buf[4+i] = uart4_buffer[i];
					#endif
					break;
					
				case UART_4:
					#if USART5_ENABLE
					dataLength = gUart5_length;
					clientCmca.data_buf[3] = UART_4;
					for(i=0;i<gUart5_length;i++)
						clientCmca.data_buf[4+i] = uart5_buffer[i];
					#endif
					break;

				default:break;
			}
			QPRINTF("dataLength=%d,\r\n",dataLength);
			ShortGetTwoChar(clientCmca.data_length , dataLength+4);
			IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], dataLength+4));
			sendFlg = 1;
			break;
			
		case SEND_UPDATED_STATUS_CMD:
			break;

		case SEND_HEART_BEAT_CMD:
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			ShortGetTwoChar(clientCmca.cmd ,SEND_HEART_BEAT_CMD);
			ShortGetTwoChar(clientCmca.data_length , 31);
			clientCmca.data_buf[0] = gMode;
			
			for(i=0;i<12;i++)
				clientCmca.data_buf[i+1] = softVersion[i];	
			for(i=0;i<12;i++)
				clientCmca.data_buf[i+13] = hardVersion[i];

			for(i=0;i<6;i++)
				clientCmca.data_buf[i+25] = macAddr[i];
			
			IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 31));
			sendFlg = 1;
			break;

		#if PRODUCTS_TEST_ENABLE
		case TEST_SET_MCU_LINK_TCPIP_PORT_CMD:
			if(!gMode)
				return;
			
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			if(crc == FourCharGetInt(clientCmca.crc) && packetLength == 6)
			{
				writeServerTcpIpToflash(&clientCmca.data_buf[0]);
				
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 2);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_SET_MCU_LINK_TCPIP_PORT_CMD);
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 2));
			}
			else 
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_SET_MCU_LINK_TCPIP_PORT_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//time out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case TSET_SET_MCU_LOCAL_TCPIP_PORT_CMD:
			if(!gMode)
				return;
			
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			//ip addr 4byte  netmask 4byte  gw 4byte   port 2byte
			if(crc == FourCharGetInt(clientCmca.crc) && packetLength == 14)
			{
				writeLocalTcpIpToflash(&clientCmca.data_buf[0]);
				
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 2);
				ShortGetTwoChar(&clientCmca.data_buf[0],TSET_SET_MCU_LOCAL_TCPIP_PORT_CMD);
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 2));
			}
			else 
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TSET_SET_MCU_LOCAL_TCPIP_PORT_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//time out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case TEST_SET_MCU_UART_CMD:
			if(!gMode)
				return;
			
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			if(packetLength == (RELAY_MAX-1)*2)
			{
				for(i=0;i<(RELAY_MAX-1)*2;i++)
				{
					if(clientCmca.data_buf[i] > (RELAY_MAX-1))
						break;
				}

				if(i == (RELAY_MAX-1)*2)
				{
					if(crc == FourCharGetInt(clientCmca.crc))
					{
						writeUartMapToflash(&clientCmca.data_buf[0]);
						
						ShortGetTwoChar(clientCmca.cmd ,ACK);
						ShortGetTwoChar(clientCmca.data_length , 2);
						ShortGetTwoChar(&clientCmca.data_buf[0],TEST_SET_MCU_UART_CMD);
						IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 2));
					}
					else 
					{
						ShortGetTwoChar(clientCmca.cmd ,NACK);
						ShortGetTwoChar(clientCmca.data_length , 3);
						ShortGetTwoChar(&clientCmca.data_buf[0],TEST_SET_MCU_UART_CMD);
						clientCmca.data_buf[2] = DATA_ERROR_ACK;//time out
						IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
					}
				}
			}
			sendFlg = 1;
			break;

		case TEST_SEND_DATA_CMD:
			if(!gMode)
				return;
			
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			ShortGetTwoChar(clientCmca.cmd ,TEST_SEND_DATA_CMD);
			clientCmca.data_buf = &uartBuf[0];
			dataLength = PACKET_DATA_LENGTH;
			for(i=0;i<dataLength;i++)
				clientCmca.data_buf[i] = i%100;
			ShortGetTwoChar(clientCmca.data_length , dataLength);
			IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], dataLength));
			sendFlg = 1;
			break;

		case TEST_GET_MCU_LINK_TCPIP_PORT_CMD:
			QPRINTF("TEST_GET_MCU_LINK_TCPIP_PORT_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 8);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_LINK_TCPIP_PORT_CMD);

				j= 2;
				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = serverIPAddr[i];

				for(i=0;i<2;i++)
					clientCmca.data_buf[j++] = serverPort[i];
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 8));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_LINK_TCPIP_PORT_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case TSET_GET_MCU_LOCAL_TCPIP_PORT_CMD:
			QPRINTF("TSET_GET_MCU_LOCAL_TCPIP_PORT_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 16);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_LINK_TCPIP_PORT_CMD);

				j= 2;
				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = clientIPAddr[i];

				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = netmaskAddr[i];

				for(i=0;i<4;i++)
					clientCmca.data_buf[j++] = gwAddr[i];

				for(i=0;i<2;i++)
					clientCmca.data_buf[j++] = clientPort[i];
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 8));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_LINK_TCPIP_PORT_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case TEST_GET_MCU_UART_MAP_CMD:
			QPRINTF("TEST_GET_MCU_UART_MAP_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 10);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_UART_MAP_CMD);

				j= 2;
				for(i=0;i<8;i++)
					clientCmca.data_buf[j++] = usrtMap[i/2][i%2];

				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 10));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_UART_MAP_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;

		case TEST_GET_MCU_MAC_CMD:
			QPRINTF("TEST_GET_MCU_MAC_CMD\r\n");
			fullTcpIpInfo();
			IntGetFourChar(clientCmca.time_stamp,0);
			packetLength	= TwoCharGetShort(clientCmca.data_length);
			crc = get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], packetLength);
			if(crc == FourCharGetInt(clientCmca.crc))
			{
				ShortGetTwoChar(clientCmca.cmd ,ACK);
				ShortGetTwoChar(clientCmca.data_length , 8);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_MAC_CMD);

				j= 2;
				for(i=0;i<6;i++)
					clientCmca.data_buf[j++] = macAddr[i];

				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 8));
			}
			else
			{
				ShortGetTwoChar(clientCmca.cmd ,NACK);
				ShortGetTwoChar(clientCmca.data_length , 3);
				ShortGetTwoChar(&clientCmca.data_buf[0],TEST_GET_MCU_MAC_CMD);
				clientCmca.data_buf[2] = DATA_ERROR_ACK;//crc error out
				IntGetFourChar(&clientCmca.crc[0],get_crc32(CRC_INIT_VAL, &clientCmca.data_buf[0], 3));
			}
			sendFlg = 1;
			break;
		#endif

		default:break;
	}
	
	if(sendFlg)
		clientSendDataToServer();
}

