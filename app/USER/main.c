#include "stm32f10x.h"
#include "stm32_eth.h"
#include "lwip/tcp.h"
#include "tcp_client_demo.h"
#include "products.h"
#include "usart.h"
#include "usr_task.h"
#include "ke_timer.h"
#include "usr_lwip.h"
#include "usr_design.h"
#include "wdg.h"
#include "flash.h"
#include "relay.h"
#include "products_test.h"
#include "common.h"
#include "led.h"
#include "commucation.h"


__IO uint32_t LWipTime = 0;     //LWip时钟周期计数器
unsigned char gMode = 0;	//0-->normal mode		1-->test mode

#define PHY_ADDRESS       0x01  //DP83848地址，根据核心板硬件连接决定
#define RMII_MODE               //配置网卡接口为RMII
// #define MII_MODE
static void NVIC_Configuration1(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;            //库文件内，NVIC结构体

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //库函数，设置优先级分组：先占优先级和从优先级
                                                    //NVIC_PriorityGroup_2：先占优先级2位 从优先级2位
  
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;           //设置中断通道=以太网中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//设置先占优先级=2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //设置从优先级=0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能中断
  NVIC_Init(&NVIC_InitStructure);                          //库函数，根据NVIC_InitStructure结构体内的参数初始化NVIC寄存器
}

static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; //库文件，内初始化GPIO用到的结构体

  //配置PA2  复用推免输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //配置PC1  复用推免输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //配置PB11 PB12 PB13 复用推免输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  //改变RX_DV RXD0和RXD1的映射 
  GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

  //配置PA0 PA1 浮空输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //配置PB10 浮空输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  //配置PD8 PD9 PD10 浮空输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  //配置PA8 复用推免输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void Ethernet_Configuration(void)
{ 
	ETH_InitTypeDef ETH_InitStructure;  //库文件，初始化以太网用到的结构体

//选择接口是MII还是RMII，这里使用RMII
#ifdef MII_MODE //选择使用MII接口 MII接口需要时钟25M
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);//该函数设置以太网接口为MII

	RCC_MCOConfig(RCC_MCO_HSE);//配置MCO时钟输出 RCC_MCO_HSE参数设置外部高速时钟作为MCO输出，核心板用的25M，正好提供MII的25M时钟

#elif defined RMII_MODE //选择使用RMII接口 RMII接口需要时钟50M
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);//该函数设置以太网接口为RMII

	//程序思路利用PLL3时钟为MCO提供RMII需要的50M时钟
	//系统初始化STM32主时钟为72M的时候，PLL3被设置为了外部25M晶振5分频 25/5=5M
	RCC_PLL3Config(RCC_PLL3Mul_10);//PLL3倍频10倍 5*10=50M

	RCC_PLL3Cmd(ENABLE);//使能PLL3时钟

	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)//等待PLL3使能成功
	{}
	  
	RCC_MCOConfig(RCC_MCO_PLL3CLK);//选择MCO的时钟源为PLL3
#endif

	ETH_DeInit();       //复位以太网外设寄存器到默认值，先使能，在禁止，已恢复默认值

	ETH_SoftwareReset();//软件复位，通过设置ETH->DMABMR寄存器的位0实现

	while (ETH_GetSoftwareResetStatus() == SET);//等待软复位成功

	ETH_StructInit(&ETH_InitStructure);//初始化结构体ETH_InitStructure各个成员为默认值

	//ETH_InitStructure结构体内成员较多，填充外默认值后，修改一些关键成员，以适应当前的需要
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//开启网络自适应功能
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//关闭反馈
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//关闭重传功能
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//关闭自动去除PDA/CRC功能 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//关闭接收所有的帧
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//允许接收所有广播帧
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//关闭混合模式的地址过滤  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//对于组播地址使用完美地址过滤   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//对单播地址使用完美地址过滤 
    
	#if  !CHECKSUM_GEN_ICMP    //判断是否开启硬件校验,关闭软件校验
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//开启ipv4和TCP/UDP/ICMP的帧校验和卸载   
	#endif
	
	//当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
	//这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //开启丢弃TCP/IP错误帧
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //开启接收数据的存储转发模式    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //开启发送数据的存储转发模式  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//禁止转发错误帧  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//不转发过小的好帧 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//打开处理第二帧功能
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//开启DMA传输的地址对齐功能
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//开启固定突发功能    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA发送的最大突发长度为32个节拍   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA接收的最大突发长度为32个节拍
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	ETH_Init(&ETH_InitStructure,PHY_ADDRESS);		        //配置ETH

	ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//使能以太网接收中断	
}

static void usr_system_init(void)
{
	#if PRODUCTS_TEST_ENABLE
	mode_io_init();
	#endif
	clientBufInit();
	USR_USART_Init();
	MacInit();
	versionInit(SW_VERSION,HW_VERSION);
	ke_timer_init();
	SPI_Flash_Init();
	relay_gpio_init();
	usrtMapInit();
	led_init();
	serverIpPortInit();
	localIpPortInit();
	
	if(!gMode)//normal mode
	{
		QPRINTF("Normal mode\r\n");
		LED2_CLOSE;
	}
	#if PRODUCTS_TEST_ENABLE
	else// setup mode
	{
		QPRINTF("Test mode\r\n");
		LED2_OPEN;
	}
	#endif
	display_info();
}

void delay(unsigned int time)
{
	unsigned int i=0,j=0;
	for(i=0;i<time;i++)
		for(j=0;j<1000;j++)
			;
}
int main()
{
	RCC_ClocksTypeDef RCC_Clocks;//库文件结构体，定义一个时钟参数的结构体备用
	
	NVIC_Configuration1();       //自定义函数，设置以太网NVIC中断
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | 
						  RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);	                                            //库函数，使能MAC、MAC发送和MAC接收时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
						  RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO , ENABLE);	    //库函数，使能GPIO时钟
	GPIO_Configuration();       //自定义函数，初始化用到的GPIO
	
	usr_system_init();

	Ethernet_Configuration();   //自定义函数，配置以太网
	RCC_GetClocksFreq(&RCC_Clocks);                   //库函数，获取当前系统的时钟参数
	SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);//库函数，配置SysTick时钟，产生一个10ms的中断
	NVIC_SetPriority (SysTick_IRQn, 1);               //库函数，设置SysTick的中断优先级
	
	LWIP_Init();
	TCP_Client_Init(TwoCharGetShort(clientPort),TwoCharGetShort(serverPort),serverIPAddr); //tcp客户端初始化

#if WDT_ENABLE
	watchdog_init();
#endif
	
	ke_timer_set(USR_TCP_IP_LINK_TIMER,300);
	QPRINTF("NORMAL INIT OK\r\n");
	while(1)
	{ 
		#if WDT_ENABLE
		IWDG_ReloadCounter();
		#endif

		ke_schedule();
		lwip_periodic_handle();						    //作为服务器的例行事件服务，主要是更新TCP timers
	}

}

