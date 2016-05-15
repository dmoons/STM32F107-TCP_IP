#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "iap.h"
#include "flash.h"
#include "products.h"

unsigned char gMode = 0;	//0-->normal mode		1-->test mode
#define MODE_TEST_PIN	GPIO_Pin_3
void mode_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //���ļ����ڳ�ʼ��GPIO�õ��Ľṹ��

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	    //�⺯����ʹ��GPIOʱ��
	//����PA2  �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	if(!GPIO_ReadInputDataBit(GPIOE, MODE_TEST_PIN))
	{
		gMode = 1;;
	}
}

int main()
{	
	delay_init();                    //��ʱ��ʼ��
	#if DEBUG
	
	#if USART1_ENABLE
	USART1_NVIC_Configuration();     //����1�ж����ú��� 
	USART1_Init();     
	#endif
	
	#if USART2_ENABLE
	USART2_NVIC_Configuration();     //����2�ж����ú��� 
	USART2_Init();                   //����2��ʼ��
	#endif
	
	QPRINTF("boot code.....\r\n");
	#endif

	SPI_Flash_Init();
	mode_io_init();

	if(!gMode)
	{
		QPRINTF("Normal mode....\r\n");
		if(iap_update_check())
		{
			QPRINTF("normal iap_update\r\n");
			iap_update();	
		}
		else
		{
			QPRINTF("normal iap_load_app\r\n");
			delay_ms(100);
			iap_load_app(APP1_CODE_ADDR);
		}
	}
	else
	{
		QPRINTF("Debug mode....\r\n");
		if(debug_iap_update_check())
		{
			QPRINTF("debug iap_update\r\n");
			debug_iap_update();	
		}
		else
		{
			QPRINTF("debug iap_load_app\r\n");
			delay_ms(100);
			iap_load_app(DEBUG1_CODE_ADDR);
		}
	}
	
}
