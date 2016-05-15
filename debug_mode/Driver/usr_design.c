#include "usr_design.h"
#include "usart.h"
#include "lwip/tcp.h"
#include "tcp_client_demo.h" 
#include "usr_task.h"
#include "commucation.h"
#include "ke_timer.h"
#include "stm32f10x.h"
#include "iap_code.h"
#include "stmflash.h"
#include "common.h"
#include "products_test.h"
#include "led.h"

#define LINK_TIME_OUT_MAX_COUNT		(3)
unsigned char macAddr[6]		= {0x00,0x11,0x22,0x33,0x44,0x55};
unsigned char softVersion[12]	= {0x00};		
unsigned char hardVersion[12]	= {0x00};		

unsigned char serverIPAddr[4] 	= {TCP_SERVER_IP};	
unsigned char serverPort[2]		= {(TCP_SERVER_PORT>>8)&0xff,TCP_SERVER_PORT&0xff};			//client port 8080

unsigned char clientIPAddr[4] 	= {TCP_CLIENT_IP};	//client ip 192.168.0.1
unsigned char netmaskAddr[4]	= {TCP_CLIENT_NETMASK};
unsigned char gwAddr[4]			= {TCP_CLIENT_GW};
unsigned char clientPort[2]		= {(TCP_CLIENT_PORT>>8)&0xff,TCP_CLIENT_PORT&0xff};			//client port 8080

unsigned char usrtMap[4][2]	={{1,1},{2,2},{3,3},{4,4}};

unsigned short serverLength;
unsigned char LinkStatus = 0;
unsigned char ipconnectFlag = 0;
static unsigned char heartBeatCount =0;

void usrtMapInit(void)
{
	unsigned char i,pBuffer[SETTING_BUFFER_SIZE],index;
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	index = UART_MAP_ADDR;
	if( pBuffer[index+0] == 0xff && 
		pBuffer[index+1] == 0xff && 
		pBuffer[index+2] == 0xff && 
		pBuffer[index+3] == 0xff)
	{
		for(i=0;i<UART_MAP_GROUP;i++)
		{
			pBuffer[index+i*2] 		= usrtMap[i][0];
			pBuffer[index+i*2+1] 	= usrtMap[i][1];
		}

		STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	}

	QPRINTF("\r\n***********************\r\n");
	for(i=0;i<UART_MAP_GROUP;i++)
	{
		usrtMap[i][0] = pBuffer[index+i*2];
		usrtMap[i][1] = pBuffer[index+i*2+1];
		QPRINTF("uart%d\t-->\trelay%d.\r\n",usrtMap[i][0],usrtMap[i][1]);
	}
	QPRINTF("***********************\r\n\r\n");
}

void localIpPortInit(void)
{
	unsigned char i,j=0,pBuffer[SETTING_BUFFER_SIZE];
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);

	if( pBuffer[LOCAL_TCP_IP_ADDR+0] == 0xff && 
		pBuffer[LOCAL_TCP_IP_ADDR+1] == 0xff && 
		pBuffer[LOCAL_TCP_IP_ADDR+2] == 0xff && 
		pBuffer[LOCAL_TCP_IP_ADDR+3] == 0xff)
	{
		for(i=LOCAL_TCP_IP_ADDR;i<LOCAL_TCP_IP_ADDR+LOCAL_TCP_IP_SIZE;i++)
			pBuffer[i] 	= clientIPAddr[j++];

		for(i=LOCAL_NETMASK_ADDR;i<LOCAL_NETMASK_ADDR+LOCAL_NETMASK_SIZE;i++)
			pBuffer[i] 	= netmaskAddr[j++];

		for(i=LOCAL_GW_ADDR;i<LOCAL_GW_ADDR+LOCAL_GW_SIZE;i++)
			pBuffer[i] 	= gwAddr[j++];

		for(i=LOCAL_PORT_ADDR;i<LOCAL_PORT_ADDR+LOCAL_PORT_SIZE;i++)
			pBuffer[i] 	= clientPort[j++];

		STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	}

	for(i=0;i<LOCAL_TCP_IP_SIZE;i++)
		clientIPAddr[i] = pBuffer[LOCAL_TCP_IP_ADDR+i];

	for(i=0;i<LOCAL_NETMASK_SIZE;i++)
		netmaskAddr[i] = pBuffer[LOCAL_NETMASK_ADDR+i];

	for(i=0;i<LOCAL_GW_SIZE;i++)
		gwAddr[i] = pBuffer[LOCAL_GW_ADDR+i];

	for(i=0;i<LOCAL_PORT_SIZE;i++)
		clientPort[i] = pBuffer[LOCAL_PORT_ADDR+i];
}

void serverIpPortInit(void)
{
	unsigned char i,j=0,pBuffer[SETTING_BUFFER_SIZE];
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);

	if( pBuffer[SERVER_TCP_IP_ADDR+0] == 0xff && 
		pBuffer[SERVER_TCP_IP_ADDR+1] == 0xff && 
		pBuffer[SERVER_TCP_IP_ADDR+2] == 0xff && 
		pBuffer[SERVER_TCP_IP_ADDR+3] == 0xff)
	{
		for(i=SERVER_TCP_IP_ADDR;i<SERVER_TCP_IP_ADDR+SERVER_TCP_IP_SIZE;i++)
			pBuffer[i] = serverIPAddr[j++];

		for(i=SERVER_PORT_ADDR;i<SERVER_PORT_ADDR+SERVER_PORT_SIZE;i++)
			pBuffer[i] = serverPort[j++];

		STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	}

	for(i=0;i<SERVER_TCP_IP_SIZE;i++)
		serverIPAddr[i] = pBuffer[SERVER_TCP_IP_ADDR+i];

	for(i=0;i<SERVER_PORT_SIZE;i++)
		serverPort[i] = pBuffer[SERVER_PORT_ADDR+i];
}

void versionInit(unsigned char sa,unsigned char sb,unsigned char sc,unsigned char sd,
	unsigned char ha,unsigned char hb,unsigned char hc,unsigned char hd)
{
	unsigned char i;
	unsigned char swClass[8] = {SW_CLASS};
	unsigned char hwClass[8] = {HW_CLASS};
	unsigned char app1_buf[DEBUG1_VERSION_SIZE];
	unsigned short newVersior=0,localVersion=0;
	for(i=0;i<8;i++)
		softVersion[i] 	= swClass[i];
	softVersion[i++] 	= sa;
	softVersion[i++] 	= sb;
	softVersion[i++]	= sc;
	softVersion[i++]	= sd;

	for(i=0;i<8;i++)
		hardVersion[i] 	= hwClass[i];
	hardVersion[i++] 	= ha;
	hardVersion[i++] 	= hb;
	hardVersion[i++]	= hc;
	hardVersion[i++]	= hd;

	newVersior += (sb%10)*100;
	newVersior += (sc%10)*10;
	newVersior += (sd%10);
	
	STMFLASH_Read(DEBUG1_VERSION_ADDR,(unsigned short *)app1_buf,DEBUG1_VERSION_SIZE);

	localVersion += (app1_buf[0]%10)*100;
	localVersion += (app1_buf[1]%10)*10;
	localVersion += (app1_buf[2]%10);
	QPRINTF("localVersion=%d,newVersior=%d\r\n",localVersion,newVersior);
	
	if(newVersior > localVersion)
	{
		app1_buf[0] = newVersior/100%10;
		app1_buf[1] = newVersior/10%10;
		app1_buf[2] = newVersior%10;
		STMFLASH_Write(DEBUG1_VERSION_ADDR,(unsigned short *)app1_buf,DEBUG1_VERSION_SIZE);
	}
}

void MacInit(void) 
{ 
	unsigned int Device_Serial0, Device_Serial1, Device_Serial2,mac;   
	Device_Serial0 = *(volatile unsigned int*)(0x1FFFF7E8); 
	Device_Serial1 = *(volatile unsigned int*)(0x1FFFF7EC); 
	Device_Serial2 = *(volatile unsigned int*)(0x1FFFF7F0);
	//macAddr[3]=(unsigned char)(Device_Serial0>>1);
	//macAddr[4]=(unsigned char)(Device_Serial1>>2);
	//macAddr[5]=(unsigned char)(Device_Serial2>>3);		   /*MAC最后3字节利用STM的唯一ID计算，特别注意此方法可能存在重复，需要配置应用软件做出厂检测MAC的唯一性*/

	mac = (Device_Serial0>>1) + (Device_Serial1>>2) + (Device_Serial2>>3);

	macAddr[2]=(unsigned char)((mac>>24)&0xff);
	macAddr[3]=(unsigned char)((mac>>16)&0xff);
	macAddr[4]=(unsigned char)((mac>>8)&0xff);
	macAddr[5]=(unsigned char)(mac&0xff);		   /*MAC最后3字节利用STM的唯一ID计算，特别注意此方法可能存在重复，需要配置应用软件做出厂检测MAC的唯一性*/

	QPRINTF("CPUID IS 0X%X %X %X.\r\n", Device_Serial0,Device_Serial1, Device_Serial2 ); 
}


unsigned char TcpIpLinkStatus(void)
{
	struct tcp_pcb *cpcb;
//	static unsigned char flag = 0;
	for(cpcb = tcp_active_pcbs;cpcb != NULL; cpcb = cpcb->next)
	{
		if(cpcb->local_port == TwoCharGetShort(clientPort) && cpcb->remote_port == TwoCharGetShort(serverPort))		//如果TCP_LOCAL_PORT端口指定的连接没有断开
		{
			ipconnectFlag = 1;  						//连接标志
			break;							   	
		}
	}

	QPRINTF("ipconnectFlag=%d,LinkStatus=%d\r\n",ipconnectFlag,LinkStatus);
//	if(connect_flag == 0 || (LinkStatus == 0 &&  LinkCount >= LINK_TIME_OUT_MAX_COUNT))  							// TCP_LOCAL_PORT指定的端口未连接或已断开
#if 0
	if(ipconnectFlag == 1 && LinkStatus == 0)
	{
		//if(flag==0)
		//	usr_heartbeat_cmd(100);
		//flag = (flag+1)%2;
	}
#endif

	//if(ipconnectFlag == 0 || (LinkStatus == 0))
	if(ipconnectFlag == 0)
	{
		QPRINTF("TcpIpLinkStatus reconnect tcp\r\n");
		//flag = 0;
		tcp_close(tcp_client_pcb);
		TCP_Client_Init(TwoCharGetShort(clientPort),TwoCharGetShort(serverPort),serverIPAddr); //tcp客户端初始化
	}
	
	return ipconnectFlag;
}

void receive_server_packet(unsigned char *data,unsigned short length)
{
	unsigned short i;
	LinkStatus = 1;
	heartBeatCount = 0;
	//LinkCount = 0;
	if(length > (DATA_OFFSET+PACKET_DATA_LENGTH))
	{
		QPRINTF("receive data length error\r\n");
		return;
	}
	
	serverLength = length;
	for(i=0;i<length;i++)
		clientSendBuffer[i] = data[i];

	ke_timer_set(USR_RECEIVE_SERVER_TIMER,1);
}

void usr_receive_server_timer(unsigned short const msgid)
{
	if(receive_server_data())
		serverCmdProcess();
	else
		QPRINTF("receive packet head is error!\r\n");	
}

void usr_system_reset_timer(unsigned short const msgid)
{
	int icount = 0, jcount = 0;
	QPRINTF("NVIC_SystemReset... \r\n");
	tcp_close(tcp_client_pcb);
	
	for(icount = 0; icount <1000; icount++)
		for(jcount = 0; jcount < 1000; jcount++);
	NVIC_SystemReset();
}

void usr_tcpip_timeout_timer(unsigned short const msgid)
{
	QPRINTF("tcp ip link time out\r\n");
	LinkStatus = 0;
//	LinkCount++;
	LED1_CLOSE;
}

void usr_heartbeat_cmd(unsigned short const timeout)
{
	QPRINTF("usr_heartbeat_cmd\r\n");
	ShortGetTwoChar(clientCmca.cmd,SEND_HEART_BEAT_CMD);
	serverCmdProcess();
	
	ke_timer_set(USR_TCP_IP_DICONNECTTION_TIMER,timeout);

}
void usr_tcp_ip_link_timer(unsigned short const msgid)
{
	
	if(TcpIpLinkStatus() && LinkStatus == 1)
	{
		QPRINTF("TCP IP Linked LinkStatus=%d\r\n",LinkStatus);
		if(LinkStatus)			
			LED1_OPEN;
		
		
	}
	else
	{
		//heartBeatCount = 0;
		if(heartBeatCount++ >=10)
		{
			heartBeatCount = 0;
			usr_heartbeat_cmd(300);
		}
		QPRINTF("TCP IP Discon\r\n");
		LED1_CLOSE;
	}
	ke_timer_set(USR_TCP_IP_LINK_TIMER,100);

	
}


void display_info(void)
{
	QPRINTF("\r\n****************************************\r\n");
	#if LWIP_DHCP
	QPRINTF("LWIP_DHCP Enable\r\n");
	#else
	QPRINTF("LWIP_DHCP Disable\r\n");
	#endif
	QPRINTF("software version %d.%d.%d\r\n",softVersion[9],softVersion[10],softVersion[11]);
	QPRINTF("hardware version %d.%d.%d\r\n",hardVersion[9],hardVersion[10],hardVersion[11]);
	
	QPRINTF("MAC\t\t: %02x:%02x:%02x:%02x:%02x:%02x\r\n",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	QPRINTF("local IP\t:%d.%d.%d.%d\r\n",
		clientIPAddr[0],
		clientIPAddr[1],
		clientIPAddr[2],
		clientIPAddr[3]);
	QPRINTF("local netmask\t:%d.%d.%d.%d\r\n",
		netmaskAddr[0],
		netmaskAddr[1],
		netmaskAddr[2],
		netmaskAddr[3]);
	QPRINTF("local gw\t:%d.%d.%d.%d\r\n",
		gwAddr[0],
		gwAddr[1],
		gwAddr[2],
		gwAddr[3]);
	QPRINTF("local port\t:%d\r\n\r\n",TwoCharGetShort(clientPort));
	
	QPRINTF("server IP\t:%d.%d.%d.%d\r\n",
		serverIPAddr[0],
		serverIPAddr[1],
		serverIPAddr[2],
		serverIPAddr[3]);
	QPRINTF("server port\t:%d\r\n",TwoCharGetShort(serverPort));
	QPRINTF("****************************************\r\n\r\n");
}

