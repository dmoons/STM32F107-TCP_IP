#ifndef __DELAY_H
#define __DELAY_H 	

#include "stm32f10x.h"

void delay_init(void);   //延时初始化
void delay_ms(u16 nms);  //延时毫秒函数 要求单次延时小于1846ms
void delay_us(u32 nus);  //延迟微秒函数

#endif





























