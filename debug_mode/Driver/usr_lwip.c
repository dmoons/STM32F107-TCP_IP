#include "usr_lwip.h"
#include "common.h"
#include "usr_design.h"

err_t ethernetif_init(struct netif *netif); //�����ļ��ں�������
err_t ethernetif_input(struct netif *netif);//�����ļ��ں�������

u8_t MACaddr[6]={0,0,0,0,0,1};              //MAC��ַ������Ψһ��

u8_t tcp_data[] = "����TCP�ͻ���2!\n";

struct netif netif;             //����һ��ȫ�ֵ�����ӿڣ�ע����������Ҫ�õ�  
__IO uint32_t TCPTimer = 0;     //TCP��ѯ��ʱ�� 
__IO uint32_t ARPTimer = 0;     //ARP��ѯ��ʱ��
__IO uint32_t CLIENTTimer = 0;  //�ͻ��˷��ͼ�ʱ��
extern uint32_t LWipTime;       //LWip����ʱ��

#if LWIP_DHCP                   //�������DHCP
u32 DHCPfineTimer=0;	        //DHCP��ϸ�����ʱ��
u32 DHCPcoarseTimer=0;	        //DHCP�ֲڴ����ʱ��
#endif

void LWIP_Init(void)
{
  struct ip_addr ipaddr;     //����IP��ַ�ṹ��
  struct ip_addr netmask;    //������������ṹ��
  struct ip_addr gw;         //�������ؽṹ��
  mem_init();  //��ʼ����̬�ڴ��
  memp_init(); //��ʼ���ڴ��
  lwip_init(); //��ʼ��LWIP�ں�
	
#if LWIP_DHCP     //�������DHCP���Զ���ȡIP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
#else            //������DHCP��ʹ�þ�̬IP
  IP4_ADDR(&ipaddr, clientIPAddr[0], clientIPAddr[1], clientIPAddr[2], clientIPAddr[3]);      //����IP��ַ
  IP4_ADDR(&netmask, netmaskAddr[0], netmaskAddr[1], netmaskAddr[2], netmaskAddr[3]);   //������������
  IP4_ADDR(&gw, gwAddr[0], gwAddr[1], gwAddr[2], gwAddr[3]);          //��������
#endif

  ETH_MACAddressConfig(ETH_MAC_Address0, MACaddr);  //����MAC��ַ

  //ע����������   ethernetif_init��������Ҫ�Լ�ʵ�֣���������netif����ֶΣ��Լ���ʼ���ײ�Ӳ����
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  //��ע����������ע�����������ΪĬ������
  netif_set_default(&netif);


#if LWIP_DHCP           //���������DHCP��λ
  dhcp_start(&netif);   //����DHCP
#endif

  //������
  netif_set_up(&netif);
}

//�������ݺ���
void LwIP_Pkt_Handle(void)
{
  //�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
  ethernetif_input(&netif);
}

//LWIP����������
void lwip_periodic_handle(void)
{
#if LWIP_TCP
	//ÿ250ms����һ��tcp_tmr()����
	if (LWipTime - TCPTimer >= 250)
	{
		TCPTimer =  LWipTime;
		tcp_tmr();
	}
#endif

	//ARPÿ5s�����Ե���һ��
	if ((LWipTime - ARPTimer) >= ARP_TMR_INTERVAL)
	{
		ARPTimer =  LWipTime;
		etharp_tmr();
	}
	
#if LWIP_DHCP //���ʹ��DHCP�Ļ�
	//ÿ500ms����һ��dhcp_fine_tmr()
	if (LWipTime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  LWipTime;
		dhcp_fine_tmr();
	}

	//ÿ60sִ��һ��DHCP�ֲڴ���
	if (LWipTime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
	{
		DHCPcoarseTimer =  LWipTime;
		dhcp_coarse_tmr();
	}  
#endif
}





