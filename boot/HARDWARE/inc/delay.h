#ifndef __DELAY_H
#define __DELAY_H 	

#include "stm32f10x.h"

void delay_init(void);   //��ʱ��ʼ��
void delay_ms(u16 nms);  //��ʱ���뺯�� Ҫ�󵥴���ʱС��1846ms
void delay_us(u32 nus);  //�ӳ�΢�뺯��

#endif





























