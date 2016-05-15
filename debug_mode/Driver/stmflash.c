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
	    WriteAddr+=2;//地址增加2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 

unsigned short STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite)	
{
	unsigned int secpos;	   //扇区地址
	unsigned short secoff;	   //扇区内偏移地址(16位字计算)
	unsigned short secremain; //扇区内剩余地址(16位字计算)	   
 	unsigned short i;    
	unsigned int offaddr;   //去掉0X08000000后的地址
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//非法地址
		
	FLASH_Unlock();						//解锁
	offaddr	=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos	=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff	=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;			//扇区剩余空间大小 
	
	if(NumToWrite<=secremain)
		secremain=NumToWrite;//不大于该扇区范围
		
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)
				break;//需要擦除  	  
		}
		
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else 
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 		
			
		if(NumToWrite==secremain)
			break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
		   	
			if(NumToWrite>(STM_SECTOR_SIZE/2))
				secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else 
				secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead)   	
{
	unsigned short i;
	NumToRead /= 2;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

void STMFLASH_Earse(unsigned int EarseAddr,unsigned int length)
{
	unsigned int secpos;	   //扇区地址
	unsigned short secoff;	   //扇区内偏移地址(16位字计算)
	unsigned short i =0;
	
	if(EarseAddr<STM32_FLASH_BASE||(EarseAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//非法地址
		
	FLASH_Unlock();

	secpos = length/STM_SECTOR_SIZE;		//扇区地址  0~127 for STM32F103RBT6
	secoff = length%STM_SECTOR_SIZE;		//在扇区内的偏移(2个字节为基本单位.)

	for(i=0;i<secpos;i++)
		FLASH_ErasePage(i*STM_SECTOR_SIZE + EarseAddr);//擦除这个扇区

	if(secoff != 0)
	{
		FLASH_ErasePage(length + EarseAddr - STM_SECTOR_SIZE);
	}

	FLASH_Lock();//上锁

}















