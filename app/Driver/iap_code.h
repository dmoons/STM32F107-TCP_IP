#ifndef _IAP_CODE_H_
#define _IAP_CODE_H_
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


#define PACKET_SIZE		(512)

typedef struct
{
	unsigned char ota_flg;
	unsigned char ota_end_flg;
	unsigned short ota_version;
	unsigned int ota_packet_size;
	unsigned int ota_current_packet;
	unsigned int ota_total_packet;

	unsigned int ota_flah_write_addr;
	
}ota_struct;

extern unsigned char gOtaFlg;

void OTA_start(unsigned short total_packet);
unsigned char OTA_Process(unsigned char *ota_buf,unsigned short length);
void OTA_end(unsigned char flag);
void OTA_Version_Update(unsigned char *buffer);
void usr_ota_time_out_timer(unsigned short const msgid);

#endif
