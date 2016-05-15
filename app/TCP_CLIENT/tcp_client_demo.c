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
		err = tcp_write(tcp_client_pcb,buff,length,1);	//��������
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
		tcp_recved(pcb, p->tot_len);				//��ȡ���ݳ��� tot_len��tcp���ݿ�ĳ���
		receive_server_packet((unsigned char*)(p->payload),p->tot_len);
		pbuf_free(p); 							    
	}
	else	 										//����������Ͽ����ӣ���ͻ���ҲӦ�Ͽ�
	{
		tcp_close(pcb); 						
	}
	err = ERR_OK;
	return err;	
}
/***********************************************************************
�������ƣ�err_t RS232_TCP_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
��    �ܣ����RS232��TCP�����ݷ���
���������
���������
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺����һ���ص���������TCP�ͻ�����������ӽ���ʱ������
***********************************************************************/
err_t TCP_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
{
	tcp_recv(pcb,TCP_Client_Recv); 				/* ����tcp���ջص����� */
	return ERR_OK;
}
/***********************************************************************
�������ƣ�TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char a,unsigned char b,unsigned char c,unsigned char d)
��    �ܣ�tcp�ͻ��˳�ʼ��
���������local_port���ض˿ںţ�remote_port��Ŀ��˿ںţ�a,b,c,d��������ip
���������
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
***********************************************************************/
void TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char *ipBuf)
{
	struct ip_addr ipaddr;
	
	IP4_ADDR(&ipaddr,ipBuf[0],ipBuf[1],ipBuf[2],ipBuf[3]);           //������IP��ַ
	tcp_client_pcb = tcp_new(); /* ����ͨ�ŵ�TCP���ƿ�(Clipcb) */
	tcp_bind(tcp_client_pcb,IP_ADDR_ANY,local_port); /* �󶨱���IP��ַ�Ͷ˿ں� ������ip��ַ��LwIP_Init()���Ѿ���ʼ��*/
    tcp_connect(tcp_client_pcb,&ipaddr,remote_port,TCP_Connected);//ע��ص�����
}


