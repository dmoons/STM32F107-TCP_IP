#ifndef __IAP_H__
#define __IAP_H__
#include "stm32f10x.h"

#define APP1_CODE_ADDR			(0x08003000)	//APP1 start addr	12K
#define APP1_CODE_SIZE			(0x20000)		//APP1 code size 128K
#define APP1_VERSION_ADDR		(APP1_CODE_ADDR+APP1_CODE_SIZE)
#define APP1_VERSION_SIZE		(16)
#define APP1_RESERVE_SIZE		(0x1000)		//app1 reserve size 4K
#define USER_SETTING_ADDR		(APP1_VERSION_ADDR+APP1_RESERVE_SIZE)
#define USER_SETTING_SIZE		(0x1000)


#define DEBUG1_CODE_ADDR		(USER_SETTING_ADDR + USER_SETTING_SIZE)//0x8025000
#define DEBUG1_CODE_SIZE		(0x19000)		//100k
#define DEBUG1_VERSION_ADDR		(DEBUG1_CODE_ADDR+DEBUG1_CODE_SIZE)
#define DEBUG1_VERSION_SIZE		(16)
#define DEBUG1_RESERVE_SIZE		(0x1000)

#define APP2_CODE_ADDR			(0x00000000)	//APP2 start addr	from spi flash
#define APP2_CODE_SIZE			(0x20000)		//app2 code size 128K
#define APP2_VERSION_ADDR		(APP2_CODE_ADDR+APP2_CODE_SIZE)
#define APP2_VERSION_SIZE		(16)
#define APP2_RESERVE_SIZE		(0x1000)		//app2 reserve size 4K

#define DEBUG2_CODE_ADDR		(APP2_VERSION_ADDR + APP2_RESERVE_SIZE)
#define DEBUG2_CODE_SIZE		(0x19000)		//100k
#define DEBUG2_VERSION_ADDR		(DEBUG2_CODE_ADDR+DEBUG2_CODE_SIZE)
#define DEBUG2_VERSION_SIZE		(16)
#define DEBUG2_RESERVE_SIZE		(0x1000)



		
// 2byte version	4byte app code size		4byte app code addr 	1byte CRC		5byte reserve

extern void iap_load_app(u32 appxaddr);			//执行flash里面的app程序
extern void flash_erase(u32 appAddr,u32 appCodeSize);
extern void flash_write(u32 appxaddr,u8 *appbuf,u32 applen);	//在指定地址开始,写入bin
extern void flash_read(unsigned int ReadAddr,unsigned char *pBuffer,unsigned int NumToRead);
extern unsigned char iap_update_check(void);
extern void iap_update(void);
extern unsigned char debug_iap_update_check(void);
extern void debug_iap_update(void);


#endif







































