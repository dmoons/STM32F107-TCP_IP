#include "usr_lwip.h"
#include "common.h"
#include "usr_design.h"

err_t ethernetif_init(struct netif *netif); //其他文件内函数声明
err_t ethernetif_input(struct netif *netif);//其他文件内函数声明

u8_t MACaddr[6]={0,0,0,0,0,1};              //MAC地址，具有唯一性

u8_t tcp_data[] = "我是TCP客户端2!\n";

struct netif netif;             //定义一个全局的网络接口，注册网卡函数要用到  
__IO uint32_t TCPTimer = 0;     //TCP查询计时器 
__IO uint32_t ARPTimer = 0;     //ARP查询计时器
__IO uint32_t CLIENTTimer = 0;  //客户端发送计时器
extern uint32_t LWipTime;       //LWip周期时钟

#if LWIP_DHCP                   //如果开启DHCP
u32 DHCPfineTimer=0;	        //DHCP精细处理计时器
u32 DHCPcoarseTimer=0;	        //DHCP粗糙处理计时器
#endif

void LWIP_Init(void)
{
  struct ip_addr ipaddr;     //定义IP地址结构体
  struct ip_addr netmask;    //定义子网掩码结构体
  struct ip_addr gw;         //定义网关结构体
  mem_init();  //初始化动态内存堆
  memp_init(); //初始化内存池
  lwip_init(); //初始化LWIP内核
	
#if LWIP_DHCP     //如果开启DHCP，自动获取IP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
#else            //不开启DHCP，使用静态IP
  IP4_ADDR(&ipaddr, clientIPAddr[0], clientIPAddr[1], clientIPAddr[2], clientIPAddr[3]);      //设置IP地址
  IP4_ADDR(&netmask, netmaskAddr[0], netmaskAddr[1], netmaskAddr[2], netmaskAddr[3]);   //设置子网掩码
  IP4_ADDR(&gw, gwAddr[0], gwAddr[1], gwAddr[2], gwAddr[3]);          //设置网关
#endif

  ETH_MACAddressConfig(ETH_MAC_Address0, MACaddr);  //配置MAC地址

  //注册网卡函数   ethernetif_init函数，需要自己实现，用于配置netif相关字段，以及初始化底层硬件。
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  //将注册网卡函数注册的网卡设置为默认网卡
  netif_set_default(&netif);


#if LWIP_DHCP           //如果开启了DHCP复位
  dhcp_start(&netif);   //开启DHCP
#endif

  //打开网卡
  netif_set_up(&netif);
}

//接收数据函数
void LwIP_Pkt_Handle(void)
{
  //从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
  ethernetif_input(&netif);
}

//LWIP周期性任务
void lwip_periodic_handle(void)
{
#if LWIP_TCP
	//每250ms调用一次tcp_tmr()函数
	if (LWipTime - TCPTimer >= 250)
	{
		TCPTimer =  LWipTime;
		tcp_tmr();
	}
#endif

	//ARP每5s周期性调用一次
	if ((LWipTime - ARPTimer) >= ARP_TMR_INTERVAL)
	{
		ARPTimer =  LWipTime;
		etharp_tmr();
	}
	
#if LWIP_DHCP //如果使用DHCP的话
	//每500ms调用一次dhcp_fine_tmr()
	if (LWipTime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  LWipTime;
		dhcp_fine_tmr();
	}

	//每60s执行一次DHCP粗糙处理
	if (LWipTime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
	{
		DHCPcoarseTimer =  LWipTime;
		dhcp_coarse_tmr();
	}  
#endif
}





