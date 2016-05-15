#include "products_test.h"
#if PRODUCTS_TEST_ENABLE
#include <string.h>
#include "stm32f10x.h"
#include "iap_code.h"
#include "usart.h"
#include "stmflash.h"
#include "usr_design.h"
#include "ke_timer.h"
#include "commucation.h"
#include "common.h"
#include "relay.h"

#define MODE_TEST_PIN	GPIO_Pin_3

extern unsigned char gMode;

unsigned int sendCount = 0,ackCount=0,nAckCount=0;


typedef struct 
{
	unsigned char head[AT_HEAD_SIZE];
	unsigned char dict[AT_DICT_SIZE];
	unsigned char cmd[AT_CMD_MAX_SIZE];
	unsigned char dataCount;
	unsigned char dataLen[AT_DATA_MAC_COUNT];
	unsigned char data[AT_DATA_MAC_COUNT*AT_DATA_MAX_SIZE];
}at_struct;

at_struct gAtStruct;
void mode_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //库文件，内初始化GPIO用到的结构体

	//配置PA2  复用推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	if(!GPIO_ReadInputDataBit(GPIOE, MODE_TEST_PIN))
	{
		gMode = 1;;
	}
}

void writeServerTcpIpToflash(unsigned char *data)
{
	unsigned char i,j=0,pBuffer[SETTING_BUFFER_SIZE];
	
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	
	for(i=SERVER_TCP_IP_ADDR;i<SERVER_TCP_IP_ADDR+SERVER_TCP_IP_SIZE;i++)
		pBuffer[i] = data[j++];
	for(i=SERVER_PORT_ADDR;i<SERVER_PORT_ADDR+SERVER_PORT_SIZE;i++)
		pBuffer[i] = data[j++];
	
	STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
}

void writeLocalTcpIpToflash(unsigned char *data)
{
	unsigned char i,j=0,pBuffer[SETTING_BUFFER_SIZE];
	
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);

	for(i=LOCAL_TCP_IP_ADDR;i<LOCAL_TCP_IP_ADDR+LOCAL_TCP_IP_SIZE;i++)
		pBuffer[i] = data[j++];

	for(i=LOCAL_NETMASK_ADDR;i<LOCAL_NETMASK_ADDR+LOCAL_NETMASK_SIZE;i++)
		pBuffer[i] = data[j++];

	for(i=LOCAL_GW_ADDR;i<LOCAL_GW_ADDR+LOCAL_GW_SIZE;i++)
		pBuffer[i] = data[j++];

	for(i=LOCAL_PORT_ADDR;i<LOCAL_PORT_ADDR+LOCAL_PORT_SIZE;i++)
		pBuffer[i] = data[j++];

	STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
}

void writeUartMapToflash(unsigned char *data)
{
	unsigned char i,j=0,pBuffer[SETTING_BUFFER_SIZE];
	
	STMFLASH_Read(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
	for(i=UART_MAP_ADDR;i<UART_MAP_ADDR+UART_MAP_SIZE;i++)
		pBuffer[i] = data[j++];
	
	STMFLASH_Write(USER_SETTING_ADDR,(unsigned short*)pBuffer,SETTING_BUFFER_SIZE);
}

unsigned char cmdGetParm(unsigned char *cmd,unsigned char dict,unsigned char *paramLen)
{
	unsigned char ret=0;
	//LOCAL_IP
	if(cmd[0] == 'L' &&
		cmd[1] == 'O' &&
		cmd[2] == 'C' &&
		cmd[3] == 'A' &&
		cmd[4] == 'L' &&
		cmd[5] == '_' &&
		cmd[6] == 'I' &&
		cmd[7] == 'P')
	{
		if(dict == AT_DICT_S)
			*paramLen = 4;
		else
			*paramLen = 0;
		
		ret = LOCAL_IP;
	}
	else if(cmd[0] == 'S' &&
		cmd[1] == 'E' &&
		cmd[2] == 'R' &&
		cmd[3] == 'V' &&
		cmd[4] == 'E' &&
		cmd[5] == 'R' &&
		cmd[6] == '_' &&
		cmd[7] == 'I' &&
		cmd[8] == 'P')
	{
		if(dict == AT_DICT_S)
			*paramLen = 2;
		else
			*paramLen = 0;
		ret = SERVER_IP;
	}
	else if(cmd[0] == 'U' &&
		cmd[1] == 'A' &&
		cmd[2] == 'R' &&
		cmd[3] == 'T' &&
		cmd[4] == '_' &&
		cmd[5] == 'M' &&
		cmd[6] == 'A' &&
		cmd[7] == 'P')
	{
		if(dict == AT_DICT_S)
			*paramLen = 4;
		else
			*paramLen = 0;
		ret =UART_MAP;
	}
	else if(cmd[0] == 'M' &&
		cmd[1] == 'C' &&
		cmd[2] == 'U' &&
		cmd[3] == '_' &&
		cmd[4] == 'V' &&
		cmd[5] == 'E' &&
		cmd[6] == 'R' &&
		cmd[7] == 'S' &&
		cmd[8] == 'I' &&
		cmd[9] == 'O' &&
		cmd[10] == 'N')
	{
		*paramLen = 0;
		ret = MCU_VERSION;
	}
	else if(cmd[0] == 'M' &&
		cmd[1] == 'C' &&
		cmd[2] == 'U' &&
		cmd[3] == '_' &&
		cmd[4] == 'M' &&
		cmd[5] == 'A' &&
		cmd[6] == 'C')
	{
		*paramLen = 0;
		ret = MCU_MAC;
	}

	return ret;
}

//从字串p中获取ip整数到整数数组a，成功返回0，失败返回1
int getipaddress(unsigned char *pp, unsigned char *a,unsigned char length)
{
    int d[5]={-1},di=1;
    int i,j,m;
    unsigned char p[16];
	
	for(i=0;i<length;i++)
		p[i] = pp[i];
     
    //删除p中非数字，非小数点的字符
    for(i=length-1;i>=0;i--)
        if(p[i]!='.' && (p[i]<'0' || p[i]>'9'))
        for(j=i;j<length;j++)
             p[j]=p[j+1];
 
    //检查获取小数点所在的位置
    if (p[0]=='.') return 1;     //小数点在第一位，错误 
    for(i=0;i<length;i++)
        if(p[i]=='.') {
            d[di++]=i; 
            if (di>4) return 1;   //小数点数量超过3个，错误 
        }
    if (di!=4) return 1;   //小数点数量少于3个，错误 
    d[4]=length;
 
    //计算各ip的分段值
    for(i=0;i<4;i++){
        m=0;       
        for(j=d[i]+1;j<=d[i+1]-1;j++)
            m=m*10+p[j]-'0';
        if (m<0 || m>255) return 1; //整数的数值过大 
        a[i]=m;
    }

    return 0;
} 

unsigned char getUartMap(unsigned char *data,unsigned char length,unsigned char *map)
{
	unsigned char i,left=0,right=0,find=0;
	
	for(i=0;i<length;i++)
	{
		if(data[i]<'0' || data[i]>'9')
		{
			if(data[i] != '.')
				return 0;
		}
	}

	for(i=0;i<length;i++)
	{
		if(data[i] != '.' && find == 0)
		{
			left = left*10 + data[i]-'0';
		}
		else if(data[i] == '.')
		{
			find = 1;
			continue;
		}

		if(find)
		{
			right = right*10 + data[i]-'0';
		}
	}

	map[0] = left;
	map[1] = right;

	return 1;
	
}

unsigned short getPortByAsic(unsigned char *data,unsigned char length,unsigned char *port)
{
	unsigned short m=0;
	unsigned char i;
	
	for(i=0;i<length;i++)
	{
		if(data[i]<'0' || data[i]>'9')
		{
			return 0;
		}
	}

	for(i=0;i<length;i++)
		m=m*10+data[i]-'0';

	port[0] = (m>>8)&0xff;
	port[1] = (m)&0xff;
	return m;
}

void usr_test_sendData_timer(unsigned short const msgid)
{
	ke_timer_set(USR_TEST_SEND_TIMER,100);
	ShortGetTwoChar(clientCmca.cmd,TEST_SEND_DATA_CMD);
	serverCmdProcess();
	sendCount++;
	QPRINTF("sendCount=%d,ackCount=%d,nAckCount=%d\r\n",sendCount,ackCount,nAckCount);
}
void UartCmdPhase(unsigned char *data,unsigned short length)
{
	if(length >= 4)
	{
		if(data[0] == 'T' && data[1] == 'E' && data[2] == 'S' && data[3] == 'T')
		{
			if(data[4] == '_' && data[5] == 'B')
			{
				sendCount = 0;
				ackCount=0;
				nAckCount=0;
				ke_timer_set(USR_TEST_SEND_TIMER,5);
			}
			else if(data[4] == '_' && data[5] == 'E')
			{
				QPRINTF("test end sendCount=%d,ackCount=%d,nAckCount=%d\r\n",sendCount,ackCount,nAckCount);
				ke_timer_clear(USR_TEST_SEND_TIMER);
			}
		}
	}
}

void UartATCmd(unsigned char *data,unsigned short length)
{
	unsigned short i =0,j,cmdLen=0,port;
	unsigned char parmCount=0,parmLen;
	unsigned char ip[4],cmd=0,dict,buffer[128]={0x00};
	
	memset(&gAtStruct,0,sizeof(gAtStruct));
	for(i=AT_HEAD_ADDR;i<AT_HEAD_SIZE;i++)
		gAtStruct.head[i] = data[i];

	if(gAtStruct.head[0] != 'A' || gAtStruct.head[1] != 'T' || gAtStruct.head[2] != '+')
	{
		QPRINTF("AT+ is error!\r\n");
		return;
	}

	gAtStruct.dict[0] = data[AT_DICT_ADDR];

	if(gAtStruct.dict[0] != 'S' && gAtStruct.dict[0] != 'G')
	{
		QPRINTF("dict is error!\r\n");
		return;
	}
	else
	{
		if(gAtStruct.dict[0] == 'S')
			dict = AT_DICT_S;
		else if(gAtStruct.dict[0] == 'G')
			dict = AT_DICT_G;
	}

	i = AT_CMD_ADDR;
	if(data[i] == '<')
	{
		i++;
		while(data[i] != '>' && i<length && cmdLen<AT_CMD_MAX_SIZE)
		{
			gAtStruct.cmd[cmdLen++] = data[i++];
		}
		cmd = cmdGetParm(gAtStruct.cmd,dict,&parmCount);
	}
	else
	{	
		QPRINTF("data error\r\n");
		return;
	}
	
	if(cmd)
	{
		for(j=0;j<parmCount;j++)
		{
			i++;
			if(data[i] == '<')
			{
				i++;
				parmLen = 0;
				while(data[i] != '>' && i<length && parmLen<AT_DATA_MAX_SIZE)
				{
					gAtStruct.data[(j*AT_DATA_MAX_SIZE)+parmLen] = data[i++];
					parmLen++;
				}
				gAtStruct.dataLen[j] = parmLen;
				if(data[i] == '>')
					gAtStruct.dataCount++;
			}
		}
	}
	else
	{
		QPRINTF("CMD is error\r\n");
		return;
	}	

	if(gAtStruct.dict[0] == 'S')
	{
		if(gAtStruct.dataCount != parmCount)
		{
			QPRINTF("param error!\r\n");
			return;
		}
		switch(cmd)
		{
			case LOCAL_IP:
				for(i=0;i<gAtStruct.dataCount;i++)
				{
					if(!getipaddress(&gAtStruct.data[i*AT_DATA_MAX_SIZE], ip,gAtStruct.dataLen[i]))
					{
						for(j=0;j<4;j++)
							buffer[i*4+j] = ip[j];
					}
					else if(i == (gAtStruct.dataCount-1))
					{
						port = getPortByAsic(&gAtStruct.data[i*AT_DATA_MAX_SIZE],gAtStruct.dataLen[i],&buffer[i*4]);
					}
					else
					{
						QPRINTF("LOCAL_IP ip is error!\r\n");
						return;
					}
				}

				if(port == 0)
				{
					QPRINTF("LOCAL_IP port is error!\r\n");
					return;
				}
				
				writeLocalTcpIpToflash(buffer);
				localIpPortInit();
				QPRINTF("AT+A<LOCAL_IP><OK>\r\n");
				break;
				
			case SERVER_IP:
				for(i=0;i<gAtStruct.dataCount;i++)
				{
					if(!getipaddress(&gAtStruct.data[i*AT_DATA_MAX_SIZE], ip,gAtStruct.dataLen[i]))
					{
						for(j=0;j<4;j++)
							buffer[i*4+j] = ip[j];
					}
					else
					{
						port = getPortByAsic(&gAtStruct.data[i*AT_DATA_MAX_SIZE],gAtStruct.dataLen[i],&buffer[i*4]);
					}
				}
				writeServerTcpIpToflash(buffer);
				serverIpPortInit();
				QPRINTF("AT+A<SERVER_IP><OK>\r\n");
				break;
				
			case UART_MAP:
				for(i=0;i<gAtStruct.dataCount;i++)
				{
					if(!getUartMap(&gAtStruct.data[i*AT_DATA_MAX_SIZE],gAtStruct.dataLen[i],&buffer[i*2]))
					{
						QPRINTF("UART_MAP param is error!\r\n");
					}
				}

				for(i=0;i<gAtStruct.dataCount*2;i++)
				{
					if(buffer[i] > (RELAY_MAX-1))
						break;
				}

				if(i == gAtStruct.dataCount*2)
				{
					writeUartMapToflash(buffer);
					usrtMapInit();
					QPRINTF("AT+A<UART_MAP><OK>\r\n");
				}
				else
				{
					QPRINTF("UART_MAP param <= 4\r\n");
				}
				break;
				
			default:
				QPRINTF("AT+S CMD is error!\r\n");
				break;
			
		}
		
	}
	else if(gAtStruct.dict[0] == 'G')
	{
		switch(cmd)
		{
			case LOCAL_IP:
				if(gAtStruct.dataCount == parmCount)
				{
					QPRINTF("AT+A<LOCAL_IP><%d.%d.%d.%d><%d.%d.%d.%d><%d.%d.%d.%d><%d>\r\n",
						clientIPAddr[0],
						clientIPAddr[1],
						clientIPAddr[2],
						clientIPAddr[3],
						netmaskAddr[0],
						netmaskAddr[1],
						netmaskAddr[2],
						netmaskAddr[3],
						gwAddr[0],
						gwAddr[1],
						gwAddr[2],
						gwAddr[3],
						((clientPort[0]<<8)+clientPort[1]));
				}
				else
				{
					QPRINTF("GET LOCAL_IP param error!\r\n");
				}
				break;
				
			case SERVER_IP:
				if(gAtStruct.dataCount == parmCount)
				{
					QPRINTF("AT+A<SERVER_IP><%d.%d.%d.%d><%d>\r\n",
						serverIPAddr[0],
						serverIPAddr[1],
						serverIPAddr[2],
						serverIPAddr[3],
						((serverPort[0]<<8)+serverPort[1]));
				}
				else
				{
					QPRINTF("GET SERVER_IP param error!\r\n");
				}
				break;

			case UART_MAP:
				if(gAtStruct.dataCount == parmCount)
				{
					QPRINTF("AT+A<UART_MAP><uart%d--scan%d><uart%d--scan%d><uart%d--scan%d><uart%d--scan%d>\r\n",
						usrtMap[0][0],usrtMap[0][1],
						usrtMap[1][0],usrtMap[1][1],
						usrtMap[2][0],usrtMap[2][1],
						usrtMap[3][0],usrtMap[3][1]);
				}
				else
				{
					QPRINTF("GET UART_MAP param error!\r\n");
				}
				break;
				
			case MCU_VERSION:
				if(gAtStruct.dataCount == parmCount)
				{
					QPRINTF("AT+A<MCU_VERSION><V %d.%d.%d>\r\n",
						softVersion[9]%10,
						softVersion[10]%10,
						softVersion[11]%10);
				}
				else
				{
					QPRINTF("GET MCU_VERSION param error!\r\n");
				}
				break;

			case MCU_MAC:
				if(gAtStruct.dataCount == parmCount)
				{
					QPRINTF("AT+A<MCU_MAC><%02x:%02x:%02x:%02x:%02x:%02x>\r\n",
						macAddr[0],
						macAddr[1],
						macAddr[2],
						macAddr[3],
						macAddr[4],
						macAddr[5]);
				}
				else
				{
					QPRINTF("GET MCU_MAC param error!\r\n");
				}
				break;
			default:
				QPRINTF("AT+G CMD is error!\r\n");
				break;
		}
	}	
}


#endif

