#ifndef __FLASH_H
#define __FLASH_H			    
#include "stm32f10x.h"
#include "products.h"

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

#if BOARD_MODE == DEVELOPER_MODE
#define SPI_FLASH_CS_LOW	GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define SPI_FLASH_CS_HIGH	GPIO_SetBits(GPIOB,GPIO_Pin_0)	
#elif BOARD_MODE == NORMAL_MODE
#define SPI_FLASH_CS_LOW	GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH	GPIO_SetBits(GPIOA,GPIO_Pin_4)			 
#endif
void SPI_Flash_Init(void);
void SPI_Flash_Read(u32 ReadAddr,u8* pBuffer,u16 NumByteToRead);   //读取flash
void SPI_Flash_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite);//写入flash
#endif
















