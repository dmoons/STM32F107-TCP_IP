#include "stmflash.h"
#include "stm32f10x.h"
static unsigned short STMFLASH_ReadHalfWord(unsigned int faddr)
{
	return *(unsigned short*)faddr; 
}

#if STM32_FLASH_WREN  
void STMFLASH_Write_NoCheck(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite)   
{ 			 		 
	unsigned short i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 

unsigned short STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite)	
{
	unsigned int secpos;	   //������ַ
	unsigned short secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	unsigned short secremain; //������ʣ���ַ(16λ�ּ���)	   
 	unsigned short i;    
	unsigned int offaddr;   //ȥ��0X08000000��ĵ�ַ
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//�Ƿ���ַ
		
	FLASH_Unlock();						//����
	offaddr	=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos	=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff	=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;			//����ʣ��ռ��С 
	
	if(NumToWrite<=secremain)
		secremain=NumToWrite;//�����ڸ�������Χ
		
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)
				break;//��Ҫ����  	  
		}
		
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}
		else 
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 		
			
		if(NumToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
		   	
			if(NumToWrite>(STM_SECTOR_SIZE/2))
				secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else 
				secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead)   	
{
	unsigned short i;
	NumToRead /= 2;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

void STMFLASH_Earse(unsigned int EarseAddr,unsigned int length)
{
	unsigned int secpos;	   //������ַ
	unsigned short secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	unsigned short i =0;
	
	if(EarseAddr<STM32_FLASH_BASE||(EarseAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//�Ƿ���ַ
		
	FLASH_Unlock();

	secpos = length/STM_SECTOR_SIZE;		//������ַ  0~127 for STM32F103RBT6
	secoff = length%STM_SECTOR_SIZE;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)

	for(i=0;i<secpos;i++)
		FLASH_ErasePage(i*STM_SECTOR_SIZE + EarseAddr);//�����������

	if(secoff != 0)
	{
		FLASH_ErasePage(length + EarseAddr - STM_SECTOR_SIZE);
	}

	FLASH_Lock();//����

}















