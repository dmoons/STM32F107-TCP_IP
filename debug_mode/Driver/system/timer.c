#include "timer.h"
#include "stm32f10x.h"

void schedule_timer_init(void)         //定时器的配置
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);  //Timer2 时钟使能

	TIM_DeInit(TIM2);                              //复位TIM2定时器      
	TIM_TimeBaseStructure.TIM_Period=20;          //定时器周期
	TIM_TimeBaseStructure.TIM_Prescaler=36000-1;     //预分频数
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;   //TIM2时钟分频,为1表示不分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//定时器计数为向上计数模式

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);           //清除定时器2的溢出标志位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);      //使能定时器2溢出中断
	TIM_Cmd(TIM2, ENABLE);                       //定时器2使能
}




