#include "tcp_client_demo.h" 
#include "usr_design.h"
#include "usart.h"

struct tcp_pcb *tcp_client_pcb;
extern unsigned char ipconnectFlag;

err_t TCP_Client_Send_Data(unsigned char *buff,unsigned int length)
{
	unsigned short i=0;
	err_t err;

	QPRINTF("TCP_Client_Send_Data:\r\n");
	for(i=0;i<length;i++)
		QPRINTF("%02x ",buff[i]);
	QPRINTF("\r\n");

//	if(TcpIpLinkStatus() == 1)
	if(ipconnectFlag)
	{
		err = tcp_write(tcp_client_pcb,buff,length,1);	//发送数据
	}
	else
		QPRINTF("---------- tcp send drop package length = %d\r\n",length);

	if(err != ERR_OK)
		QPRINTF("++++++++++ tcp send drop package length = %d\r\n",length);
	return err;					
}

err_t  TCP_Client_Recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
	if(p != NULL)
	{
		tcp_recved(pcb, p->tot_len);				//获取数据长度 tot_len：tcp数据块的长度
		receive_server_packet((unsigned char*)(p->payload),p->tot_len);
		pbuf_free(p); 							    
	}
	else	 										//如果服务器断开连接，则客户端也应断开
	{
		tcp_close(pcb); 						
	}
	err = ERR_OK;
	return err;	
}
/***********************************************************************
函数名称：err_t RS232_TCP_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
功    能：完成RS232到TCP的数据发送
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：这是一个回调函数，当TCP客户端请求的连接建立时被调用
***********************************************************************/
err_t TCP_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
{
	tcp_recv(pcb,TCP_Client_Recv); 				/* 设置tcp接收回调函数 */
	return ERR_OK;
}
/***********************************************************************
函数名称：TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char a,unsigned char b,unsigned char c,unsigned char d)
功    能：tcp客户端初始化
输入参数：local_port本地端口号；remote_port：目标端口号；a,b,c,d：服务器ip
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
void TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char *ipBuf)
{
	struct ip_addr ipaddr;
	
	IP4_ADDR(&ipaddr,ipBuf[0],ipBuf[1],ipBuf[2],ipBuf[3]);           //服务器IP地址
	tcp_client_pcb = tcp_new(); /* 建立通信的TCP控制块(Clipcb) */
	tcp_bind(tcp_client_pcb,IP_ADDR_ANY,local_port); /* 绑定本地IP地址和端口号 ，本地ip地址在LwIP_Init()中已经初始化*/
    tcp_connect(tcp_client_pcb,&ipaddr,remote_port,TCP_Connected);//注册回调函数
}


