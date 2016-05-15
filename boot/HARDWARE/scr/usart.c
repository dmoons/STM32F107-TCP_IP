#include "usart.h"

#if DEBUG

#define QPRINTF_USE		USART1

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(QPRINTF_USE,USART_FLAG_TC)==RESET); 
    USART_SendData(QPRINTF_USE,(uint8_t)ch);   
	return ch;
}


#if USART1_ENABLE
unsigned char USART1_REC_Flag = 0;                  //接受到数据标志
unsigned char USART1_buff[USART1_REC_BUFF_SIZE];    //用于接收数据缓冲区
unsigned int  USART1_rec_counter = 0;               //用于USART1接收计数

void USART1_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

	/********************以下为USART1配置**************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                                    //波特率115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //发送和接受使能
	USART_Init(USART1, &USART_InitStructure);                                       //初始化串口2
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                  //接收中断使能
	USART_Cmd(USART1, ENABLE);                                                      //使能时串口2                     
	USART_ClearITPendingBit(USART1, USART_IT_TC);                                   //清除中断TC位
}

void USART1_IRQHandler(void)  
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到了数据
	{	
		USART1_buff[USART1_rec_counter] = USART1->DR;//
		USART1_rec_counter ++;
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART1, USART_IT_TXE);           /* Clear the USART1 transmit interrupt                  */
    }
}

//串口2发中断设置函数 
void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);              //中断分组：2组

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	         //串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	         //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	 //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                              //根据指定的参数初始化VIC寄存器
}
#endif

#if USART2_ENABLE
unsigned char USART2_REC_Flag = 0;                  //接受到数据标志
unsigned char USART2_buff[USART2_REC_BUFF_SIZE];    //用于接收数据缓冲区
unsigned int  USART2_rec_counter = 0;               //用于USART2接收计数

void USART2_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

	/********************以下为USART2配置**************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
	/*
	*  USART2_TX -> PD5 , USART2_RX ->PD6
	*/	 
    //初始化PD5和PD6	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                                    //波特率115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //发送和接受使能
	USART_Init(USART2, &USART_InitStructure);                                       //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                  //接收中断使能
	USART_Cmd(USART2, ENABLE);                                                      //使能时串口2                     
	USART_ClearITPendingBit(USART2, USART_IT_TC);                                   //清除中断TC位
}

void USART2_IRQHandler(void)  
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到了数据
	{	
		USART2_buff[USART2_rec_counter] = USART2->DR;//
		USART2_rec_counter ++;
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART2, USART_IT_TXE);           /* Clear the USART2 transmit interrupt                  */
    }
}

//串口2发中断设置函数 
void USART2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);              //中断分组：2组

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	         //串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	         //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	 //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                              //根据指定的参数初始化VIC寄存器
}
#endif

#endif
