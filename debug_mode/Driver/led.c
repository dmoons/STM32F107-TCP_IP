#include "led.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE );//PORTB时钟使能 

	GPIO_InitStructure.GPIO_Pin = LED1_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED2_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	LED1_CLOSE;
	LED2_CLOSE;
}

