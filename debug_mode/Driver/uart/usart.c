#include "stm32f10x.h"
#include "usart.h"
#include "ke_timer.h"
#include "data_transfer.h"

#define PRINTF_USE_UART_PORT	USART1

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 

GPIO_InitTypeDef        GPIO_InitStructure;
USART_InitTypeDef       USART_InitStructure;
USART_ClockInitTypeDef  USART_ClockInitStructure;

static void RCC_Configuration(void)
{
	unsigned int usart_enable=0;
	#if USART1_ENABLE
	usart_enable |= RCC_APB2Periph_USART1;
	#endif
	RCC_APB2PeriphClockCmd( usart_enable | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
						   |RCC_APB2Periph_AFIO , ENABLE );

	usart_enable = 0;
	#if USART2_ENABLE
	usart_enable |= RCC_APB1Periph_USART2;
	#endif

	#if USART3_ENABLE
	usart_enable |= RCC_APB1Periph_USART3;
	#endif

	#if USART4_ENABLE
	usart_enable |= RCC_APB1Periph_UART4;
	#endif

	#if USART5_ENABLE
	usart_enable |= RCC_APB1Periph_UART5;
	#endif
	
	RCC_APB1PeriphClockCmd(usart_enable, ENABLE ); 
}

static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if USART1_ENABLE	
    /*Configure USART1 Tx (PA.09) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  	/*Configure USART1 Rx (PA.10) as input floating*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif	

#if USART2_ENABLE
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);    
	/*Configure USART2 Tx (PD.05) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
  	/*Configure USART2 Rx (PD.6) as input floating*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif

#if USART3_ENABLE
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);
	/*Configure USART3 Tx (PD.08) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
  	/*Configure USART3 Rx (PD.9) as input floating*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif

#if USART4_ENABLE
	/*Configure UART4 Tx (PC.10) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
  	/*Configure UART4 Rx (PC.11) as input floating*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

#if USART5_ENABLE
	/*Configure UART5 Tx (PC.12) as alternate function push-pull*/ 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
  	/*Configure UART5 Rx (PD.2) as input floating*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
}

static void USART_Configuration(void)
{
	USART_InitTypeDef 	 USART_InitStructure;

	/* 设置串口参数 */
	USART_InitStructure.USART_BaudRate				=115200;
	USART_InitStructure.USART_WordLength			=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				=USART_StopBits_1;
	USART_InitStructure.USART_Parity				=USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl	=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                  =USART_Mode_Rx | USART_Mode_Tx;
#if USART1_ENABLE	
	/*设置串口1*/ 
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1,ENABLE);
#endif

#if USART2_ENABLE
	/*设置串口2*/ 
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2,ENABLE);
#endif

#if USART3_ENABLE
	/*设置串口3*/ 
	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3,ENABLE);
#endif

#if USART4_ENABLE
	/*设置串口4*/ 
	USART_Init(UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4,ENABLE);
#endif

#if USART5_ENABLE
	/*设置串口5*/ 
	USART_Init(UART5, &USART_InitStructure);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART5,ENABLE);
#endif
}

//系统中断管理
static void NVIC_Configuration(void)
{ 
	unsigned char subPriority =1;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	#if USART1_ENABLE
	/* Enable the USART1 Interrupt */ 
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority++;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  	NVIC_Init(&NVIC_InitStructure);
	#endif

	#if USART2_ENABLE
	/* Enable the USART2 Interrupt */ 
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority++;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  	NVIC_Init(&NVIC_InitStructure);
	#endif

	#if USART3_ENABLE
	/* Enable the USART3 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority++;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  	NVIC_Init(&NVIC_InitStructure);
	#endif

	#if USART4_ENABLE
	/* Enable the UART4 Interrupt */ 
  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority++;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  	NVIC_Init(&NVIC_InitStructure);
	#endif

	#if USART5_ENABLE
	/* Enable the UART5 Interrupt */ 
  	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority++;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  	NVIC_Init(&NVIC_InitStructure);
	#endif 
}

void USR_USART_Init(void)
{
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	USART_Configuration();
}
#if USART1_ENABLE
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)		   //  判断，一旦接收到数据
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);		   //  清除中断标志位
        #if USART1_ENABLE
		if(gUart1_length < UART1_DATA_LENGTH)
        	uart1_buffer[gUart1_length++] =USART_ReceiveData(USART1);			//接收字符存入数组
	    ke_timer_set(USR_UART1_RECEIVE_TIMER,2);
		#endif
    }
}
#endif

#if USART2_ENABLE
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)		   //  判断，一旦接收到数据
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);		   //  清除中断标志位
        #if USART2_ENABLE
		if(gUart2_length < UART2_DATA_LENGTH)
        	uart2_buffer[gUart2_length++] =USART_ReceiveData(USART2);			//接收字符存入数组
	    ke_timer_set(USR_UART2_RECEIVE_TIMER,2);
		#endif
    }
}
#endif

#if USART3_ENABLE
void UART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE)==SET)		   //  判断，一旦接收到数据
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);		   //  清除中断标志位
        #if USART3_ENABLE
		if(gUart3_length < UART3_DATA_LENGTH)
        	uart3_buffer[gUart3_length++] =USART_ReceiveData(USART3);			//接收字符存入数组
	    ke_timer_set(USR_UART3_RECEIVE_TIMER,2);
		#endif
    }
}
#endif

#if USART4_ENABLE
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE)==SET)		   //  判断，一旦接收到数据
    {
        USART_ClearITPendingBit(UART4,USART_IT_RXNE);		   //  清除中断标志位
        #if USART4_ENABLE
		if(gUart4_length < UART4_DATA_LENGTH)
        	uart4_buffer[gUart4_length++] =USART_ReceiveData(UART4);			//接收字符存入数组
	    ke_timer_set(USR_UART4_RECEIVE_TIMER,2);
		#endif
    }
}
#endif

#if USART5_ENABLE
void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE)==SET)		   //  判断，一旦接收到数据
    {
        USART_ClearITPendingBit(UART5,USART_IT_RXNE);		   //  清除中断标志位
        #if USART5_ENABLE
		if(gUart5_length < UART5_DATA_LENGTH)
        	uart5_buffer[gUart5_length++] =USART_ReceiveData(UART5);			//接收字符存入数组
	    ke_timer_set(USR_UART5_RECEIVE_TIMER,2);
		#endif
    }
}
#endif

/* Use no semihosting */
#pragma import(__use_no_semihosting)
struct __FILE
{  
	int handle;
};
FILE __stdout;

_sys_exit(int x)
{
	x = x;
}

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
  	/* e.g. write a character to the USART */
	USART_SendData(PRINTF_USE_UART_PORT,(u8)ch);

  	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(PRINTF_USE_UART_PORT, USART_FLAG_TXE) == RESET)
  	{}

  	return ch;
}




