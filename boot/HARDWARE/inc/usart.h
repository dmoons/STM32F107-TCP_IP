#ifndef  _USART2_H_
#define  _USART2_H_
#include <stdio.h>
#include "stm32f10x.h"
#include "products.h"
#if DEBUG

#define QPRINTF		printf

#if USART1_ENABLE
#define USART1_REC_BUFF_SIZE				512   //����������

extern unsigned char USART1_REC_Flag ;                  //���յ����ݱ�־  
extern unsigned char USART1_buff[USART1_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int  USART1_rec_counter ;               //����USART2���ռ�����

void USART1_Init(void);                                              //����2��ʼ������
void USART1_NVIC_Configuration(void);                                //����2���ж����ú���   

#endif


#if USART2_ENABLE
#define USART2_REC_BUFF_SIZE				512   //����������

extern unsigned char USART2_REC_Flag ;                  //���յ����ݱ�־  
extern unsigned char USART2_buff[USART2_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int  USART2_rec_counter ;               //����USART2���ռ�����

void USART2_Init(void);                                              //����2��ʼ������
void USART2_NVIC_Configuration(void);                                //����2���ж����ú���   

#endif

#endif

#endif
