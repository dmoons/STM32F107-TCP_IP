#include "timer.h"
#include "stm32f10x.h"

void schedule_timer_init(void)         //��ʱ��������
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);  //Timer2 ʱ��ʹ��

	TIM_DeInit(TIM2);                              //��λTIM2��ʱ��      
	TIM_TimeBaseStructure.TIM_Period=20;          //��ʱ������
	TIM_TimeBaseStructure.TIM_Prescaler=36000-1;     //Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;   //TIM2ʱ�ӷ�Ƶ,Ϊ1��ʾ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//��ʱ������Ϊ���ϼ���ģʽ

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);           //�����ʱ��2�������־λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);      //ʹ�ܶ�ʱ��2����ж�
	TIM_Cmd(TIM2, ENABLE);                       //��ʱ��2ʹ��
}




