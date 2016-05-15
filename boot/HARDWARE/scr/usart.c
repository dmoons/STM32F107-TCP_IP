#include "usart.h"

#if DEBUG

#define QPRINTF_USE		USART1

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(QPRINTF_USE,USART_FLAG_TC)==RESET); 
    USART_SendData(QPRINTF_USE,(uint8_t)ch);   
	return ch;
}


#if USART1_ENABLE
unsigned char USART1_REC_Flag = 0;                  //���ܵ����ݱ�־
unsigned char USART1_buff[USART1_REC_BUFF_SIZE];    //���ڽ������ݻ�����
unsigned int  USART1_rec_counter = 0;               //����USART1���ռ���

void USART1_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

	/********************����ΪUSART1����**************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                                    //������115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //��У��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //���ͺͽ���ʹ��
	USART_Init(USART1, &USART_InitStructure);                                       //��ʼ������2
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                  //�����ж�ʹ��
	USART_Cmd(USART1, ENABLE);                                                      //ʹ��ʱ����2                     
	USART_ClearITPendingBit(USART1, USART_IT_TC);                                   //����ж�TCλ
}

void USART1_IRQHandler(void)  
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�������
	{	
		USART1_buff[USART1_rec_counter] = USART1->DR;//
		USART1_rec_counter ++;
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART1, USART_IT_TXE);           /* Clear the USART1 transmit interrupt                  */
    }
}

//����2���ж����ú��� 
void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);              //�жϷ��飺2��

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	         //����2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	         //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	 //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                              //����ָ���Ĳ�����ʼ��VIC�Ĵ���
}
#endif

#if USART2_ENABLE
unsigned char USART2_REC_Flag = 0;                  //���ܵ����ݱ�־
unsigned char USART2_buff[USART2_REC_BUFF_SIZE];    //���ڽ������ݻ�����
unsigned int  USART2_rec_counter = 0;               //����USART2���ռ���

void USART2_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

	/********************����ΪUSART2����**************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
	/*
	*  USART2_TX -> PD5 , USART2_RX ->PD6
	*/	 
    //��ʼ��PD5��PD6	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                                    //������115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //��У��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //���ͺͽ���ʹ��
	USART_Init(USART2, &USART_InitStructure);                                       //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                  //�����ж�ʹ��
	USART_Cmd(USART2, ENABLE);                                                      //ʹ��ʱ����2                     
	USART_ClearITPendingBit(USART2, USART_IT_TC);                                   //����ж�TCλ
}

void USART2_IRQHandler(void)  
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�������
	{	
		USART2_buff[USART2_rec_counter] = USART2->DR;//
		USART2_rec_counter ++;
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART2, USART_IT_TXE);           /* Clear the USART2 transmit interrupt                  */
    }
}

//����2���ж����ú��� 
void USART2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);              //�жϷ��飺2��

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	         //����2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	         //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	 //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                              //����ָ���Ĳ�����ʼ��VIC�Ĵ���
}
#endif

#endif
