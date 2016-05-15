#ifndef __STMFLASH_H__
#define __STMFLASH_H__

//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 256//512 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
 

void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Earse(unsigned int EarseAddr,unsigned int length);

#endif

















