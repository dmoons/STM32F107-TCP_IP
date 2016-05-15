#ifndef _LED_H_
#define _LED_H_
#include "products.h"
#include "stm32f10x.h"

#define LED1_PIN	GPIO_Pin_14
#define LED2_PIN	GPIO_Pin_2

#define LED1_OPEN	GPIO_ResetBits(GPIOB,LED1_PIN)
#define LED1_CLOSE	GPIO_SetBits(GPIOB,LED1_PIN)

#define LED2_OPEN	GPIO_ResetBits(GPIOE,LED2_PIN)
#define LED2_CLOSE	GPIO_SetBits(GPIOE,LED2_PIN)

void led_init(void);

#endif


