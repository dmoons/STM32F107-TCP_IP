#ifndef  _USART2_H_
#define  _USART2_H_
#include <stdio.h>
#include "stm32f10x.h"
#include "products.h"
#if DEBUG

#define QPRINTF		printf

#if USART1_ENABLE
#define USART1_REC_BUFF_SIZE				512   //缓冲区长度

extern unsigned char USART1_REC_Flag ;                  //接收到数据标志  
extern unsigned char USART1_buff[USART1_REC_BUFF_SIZE] ;//用于接收数据
extern unsigned int  USART1_rec_counter ;               //用于USART2接收计数器

void USART1_Init(void);                                              //串口2初始化函数
void USART1_NVIC_Configuration(void);                                //串口2发中断设置函数   

#endif


#if USART2_ENABLE
#define USART2_REC_BUFF_SIZE				512   //缓冲区长度

extern unsigned char USART2_REC_Flag ;                  //接收到数据标志  
extern unsigned char USART2_buff[USART2_REC_BUFF_SIZE] ;//用于接收数据
extern unsigned int  USART2_rec_counter ;               //用于USART2接收计数器

void USART2_Init(void);                                              //串口2初始化函数
void USART2_NVIC_Configuration(void);                                //串口2发中断设置函数   

#endif

#endif

#endif
