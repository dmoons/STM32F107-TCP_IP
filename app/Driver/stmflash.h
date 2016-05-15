#ifndef __STMFLASH_H__
#define __STMFLASH_H__

//用户根据自己的需要设置
#define STM32_FLASH_SIZE 256//512 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//FLASH解锁键值
 

void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead);   		//从指定地址开始读出指定长度的数据
void STMFLASH_Earse(unsigned int EarseAddr,unsigned int length);

#endif

















