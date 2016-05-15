#include "iap_code.h"
#include "common.h"
#include "usr_task.h"
#include "usr_design.h"
#include "flash.h"
#include "ke_timer.h"
#include "usart.h"

ota_struct gOTA;
unsigned char gOtaFlg = 0;

void OTA_start(unsigned short total_packet)
{
	gOTA.ota_flg 				= 1;
	gOTA.ota_end_flg			= 0;
	gOTA.ota_current_packet 	= 0;
	gOTA.ota_flah_write_addr 	= DEBUG2_CODE_ADDR;
	gOTA.ota_total_packet 		= total_packet;
	gOTA.ota_packet_size 		= 0;

	gOtaFlg = 1;
}

unsigned char OTA_Process(unsigned char *ota_buf,unsigned short length)
{
	if(length > PACKET_SIZE)
		return 1;

	if(gOTA.ota_current_packet >= gOTA.ota_total_packet)
		return 2;

	if(gOTA.ota_flah_write_addr - DEBUG2_CODE_ADDR >= DEBUG2_CODE_SIZE )
		return 3;

	if(gOTA.ota_flg == 0)
		return 4;

	SPI_Flash_Write(gOTA.ota_flah_write_addr,ota_buf,length);
	
	gOTA.ota_flah_write_addr 	+= length;
	gOTA.ota_packet_size 		+= length;
	gOTA.ota_current_packet 	+= 1;

	ke_timer_set(USR_OTA_TIME_OUT_TIMER,10*100);//ota time out process
	return 0;
}

void OTA_Version_Update(unsigned char *buffer)
{
	unsigned char i,version_buffer[DEBUG2_VERSION_SIZE];

	QPRINTF("OTA_Version_Update:");
	for(i=0;i<4;i++)
		QPRINTF("%02x",buffer[i]);
	QPRINTF("\r\n");

	version_buffer[0] = buffer[1];	//version 1.0
	version_buffer[1] = buffer[2];
	version_buffer[2] = buffer[3];

	QPRINTF("gOTA.ota_packet_size=%d,  gOTA.ota_total_packet=%d\r\n",gOTA.ota_packet_size,gOTA.ota_total_packet);
	IntGetFourChar(&version_buffer[3],gOTA.ota_packet_size);
	IntGetFourChar(&version_buffer[7],gOTA.ota_total_packet);
	IntGetFourChar(&version_buffer[11],DEBUG2_CODE_ADDR);
	SPI_Flash_Write(DEBUG2_VERSION_ADDR,version_buffer,DEBUG2_VERSION_SIZE);	
	ke_timer_set(USR_SYSTEM_RESET_TIMER,300);
}

void OTA_end(unsigned char flag)
{
	gOtaFlg = 0;
	
	gOTA.ota_flg	= flag;
	
	if(gOTA.ota_flg)
	{
		gOTA.ota_end_flg = 1;
		ke_timer_set(USR_OTA_TIME_OUT_TIMER,3*100);
	}
	else
	{
		gOTA.ota_current_packet 	= 0;
		gOTA.ota_flah_write_addr 	= DEBUG2_CODE_ADDR;
		gOTA.ota_total_packet 		= 0;
		gOTA.ota_packet_size 		= 0;
		gOTA.ota_end_flg 			= 0;
		ke_timer_set(USR_SYSTEM_RESET_TIMER,10);
	}
}

void usr_ota_time_out_timer(unsigned short const msgid)
{
	OTA_end(0);
}

