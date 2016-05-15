#include "iap.h"
#include "stmflash.h"
#include "usart.h"
#include "flash.h"
#include "products.h"

#define APP_CODE_BUF_SIZE	(1024)
unsigned char appBuf[6*APP_CODE_BUF_SIZE]={0x00};
u16 iapbuf[1024];   

typedef  void (*pFunction)(void);
pFunction jump_to_application;

unsigned char iap_update_check(void)
{
	unsigned char app1_buf[APP1_VERSION_SIZE]={0x00};
	unsigned char app2_buf[APP2_VERSION_SIZE]={0x00};
	unsigned char ret=0,i=0;
	unsigned short app1Version = 0,app2Version=0;

	STMFLASH_Read(APP1_VERSION_ADDR,(unsigned short *)app1_buf,APP1_VERSION_SIZE);

	QPRINTF("app1 version:");
	for(i=0;i<APP1_VERSION_SIZE;i++)
		QPRINTF("%02x,",app1_buf[i]);
	QPRINTF("\r\n");

	SPI_Flash_Read(APP2_VERSION_ADDR,app2_buf,APP2_VERSION_SIZE);

	QPRINTF("app2 version:");
	for(i=0;i<APP2_VERSION_SIZE;i++)
		QPRINTF("%02x,",app2_buf[i]);
	QPRINTF("\r\n");

	if(app1_buf[0] == 0xff && app1_buf[1] == 0xff && app1_buf[2] == 0xff)
	{
		app1_buf[0] = 0x00;
		app1_buf[1] = 0x00;
		app1_buf[2] = 0x01;
		STMFLASH_Write(APP1_VERSION_ADDR,(unsigned short*)app1_buf,APP1_VERSION_SIZE);
		ret = 0;
	}
	else if(app2_buf[0] == 0xff && app2_buf[1] == 0xff && app2_buf[2] == 0xff)
	{
		
		app2_buf[0] = 0x00;
		app2_buf[1] = 0x00;
		app2_buf[2] = 0x00;
		SPI_Flash_Write(APP2_VERSION_ADDR,app2_buf,APP2_VERSION_SIZE);
		ret = 0;
	}
	else
	{
		app1Version 	+= 	((app1_buf[0]%10)*100);
		app1Version 	+= 	((app1_buf[1]%10)*10);
		app1Version 	+= 	((app1_buf[2]%10)*10);

		app2Version 	+= 	((app2_buf[0]%10)*100);
		app2Version 	+= 	((app2_buf[1]%10)*10);
		app2Version 	+= 	((app2_buf[2]%10)*10);
		
		if(app2Version > app1Version)
		{
			ret = 1;
		}
		else
		{
			for(i=0;i<APP2_VERSION_SIZE;i++)
				app2_buf[i] = 0;
			SPI_Flash_Write(APP2_VERSION_ADDR,app2_buf,APP2_VERSION_SIZE);
		}
	}

	return ret;
}

void iap_update(void)
{
	unsigned int appCodeSize = 0,i = 0;
	u32 secpos;
	u16 secoff;
	
	unsigned char app1VersionBuf[APP1_VERSION_SIZE]={0x00};
	unsigned char app2VersionBuf[APP2_VERSION_SIZE]={0x00};

	QPRINTF("iap_update start.......\r\n");
	
	STMFLASH_Read(APP1_VERSION_ADDR,(unsigned short *)app1VersionBuf,APP1_VERSION_SIZE);
	SPI_Flash_Read(APP2_VERSION_ADDR,app2VersionBuf,APP2_VERSION_SIZE);	
	appCodeSize = appCodeSize+ app2VersionBuf[3];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[4];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[5];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[6];

	QPRINTF("app1 code addr: 0x%x    \r\n",APP1_CODE_ADDR);
	for(i=0;i<APP1_VERSION_SIZE;i++)
		QPRINTF("%x,",app1VersionBuf[i]);
	QPRINTF("\r\n");

	QPRINTF("app2 code addr: 0x%x    \r\n",APP2_CODE_ADDR);
	for(i=0;i<APP2_VERSION_SIZE;i++)
		QPRINTF("%x,",app2VersionBuf[i]);
	QPRINTF("\r\n");
	
	secpos = appCodeSize/APP_CODE_BUF_SIZE;
	secoff = appCodeSize%APP_CODE_BUF_SIZE;
	QPRINTF("appCodeSize=%d, secpos=%d, secof=%d,\r\n",appCodeSize,secpos,secoff);
	
	STMFLASH_Earse(APP1_CODE_ADDR,APP1_CODE_SIZE);
	
	for(i=0;i<secpos;i++)
	{
		SPI_Flash_Read(APP2_CODE_ADDR+i*APP_CODE_BUF_SIZE,appBuf,APP_CODE_BUF_SIZE);
		STMFLASH_Write(APP1_CODE_ADDR+i*APP_CODE_BUF_SIZE,(unsigned short *)appBuf,APP_CODE_BUF_SIZE);		
	}

	if(secoff != 0)
	{
		SPI_Flash_Read(APP2_CODE_ADDR+i*APP_CODE_BUF_SIZE,appBuf,secoff);
		STMFLASH_Write(APP1_CODE_ADDR+i*APP_CODE_BUF_SIZE,(unsigned short *)appBuf,secoff+1);
	}

	QPRINTF("iap_update end \r\n");
	STMFLASH_Write(APP1_VERSION_ADDR,(unsigned short *)app2VersionBuf,APP_CODE_BUF_SIZE);
	QPRINTF("NVIC_SystemReset... \r\n");
	NVIC_SystemReset();
}


unsigned char debug_iap_update_check(void)
{
	unsigned char app1_buf[DEBUG1_VERSION_SIZE]={0x00};
	unsigned char app2_buf[DEBUG2_VERSION_SIZE]={0x00};
	unsigned char ret=0,i=0;
	unsigned short app1Version = 0,app2Version=0;

	STMFLASH_Read(DEBUG1_VERSION_ADDR,(unsigned short *)app1_buf,DEBUG1_VERSION_SIZE);

	QPRINTF("debug1 version:");
	for(i=0;i<DEBUG1_VERSION_SIZE;i++)
		QPRINTF("%02x,",app1_buf[i]);
	QPRINTF("\r\n");

	SPI_Flash_Read(DEBUG2_VERSION_ADDR,app2_buf,DEBUG2_VERSION_SIZE);

	QPRINTF("debug2 version:");
	for(i=0;i<DEBUG2_VERSION_SIZE;i++)
		QPRINTF("%02x,",app2_buf[i]);
	QPRINTF("\r\n");

	if(app1_buf[0] == 0xff && app1_buf[1] == 0xff && app1_buf[2] == 0xff)
	{
		app1_buf[0] = 0x00;
		app1_buf[1] = 0x00;
		app1_buf[2] = 0x01;
		STMFLASH_Write(DEBUG1_VERSION_ADDR,(unsigned short*)app1_buf,DEBUG1_VERSION_SIZE);
		ret = 0;
	}
	else if(app2_buf[0] == 0xff && app2_buf[1] == 0xff && app2_buf[2] == 0xff)
	{
		
		app2_buf[0] = 0x00;
		app2_buf[1] = 0x00;
		app2_buf[2] = 0x00;
		SPI_Flash_Write(DEBUG2_VERSION_ADDR,app2_buf,DEBUG2_VERSION_SIZE);
		ret = 0;
	}
	else
	{
		app1Version 	+= 	((app1_buf[0]%10)*100);
		app1Version 	+= 	((app1_buf[1]%10)*10);
		app1Version 	+= 	((app1_buf[2]%10)*10);

		app2Version 	+= 	((app2_buf[0]%10)*100);
		app2Version 	+= 	((app2_buf[1]%10)*10);
		app2Version 	+= 	((app2_buf[2]%10)*10);
		
		if(app2Version > app1Version)
		{
			ret = 1;
		}
	}

	return ret;
}

void debug_iap_update(void)
{
	unsigned int appCodeSize = 0,i = 0;
	u32 secpos;
	u16 secoff;
	
	unsigned char app1VersionBuf[DEBUG1_VERSION_SIZE]={0x00};
	unsigned char app2VersionBuf[DEBUG2_VERSION_SIZE]={0x00};

	QPRINTF("iap_update start.......\r\n");
	
	STMFLASH_Read(DEBUG1_VERSION_ADDR,(unsigned short *)app1VersionBuf,DEBUG1_VERSION_SIZE);
	SPI_Flash_Read(DEBUG2_VERSION_ADDR,app2VersionBuf,DEBUG2_VERSION_SIZE);	
	appCodeSize = appCodeSize+ app2VersionBuf[3];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[4];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[5];
	appCodeSize = (appCodeSize<<8) + app2VersionBuf[6];

	QPRINTF("DEBUG1 code addr: 0x%x    \r\n",DEBUG1_CODE_ADDR);
	for(i=0;i<DEBUG1_VERSION_SIZE;i++)
		QPRINTF("%x,",app1VersionBuf[i]);
	QPRINTF("\r\n");

	QPRINTF("DEBUG2 code addr: 0x%x    \r\n",DEBUG2_CODE_ADDR);
	for(i=0;i<DEBUG2_VERSION_SIZE;i++)
		QPRINTF("%x,",app2VersionBuf[i]);
	QPRINTF("\r\n");
	
	secpos = appCodeSize/APP_CODE_BUF_SIZE;
	secoff = appCodeSize%APP_CODE_BUF_SIZE;
	QPRINTF("appCodeSize=%d, secpos=%d, secof=%d,\r\n",appCodeSize,secpos,secoff);
	
	STMFLASH_Earse(DEBUG1_CODE_ADDR,DEBUG1_CODE_SIZE);
	
	for(i=0;i<secpos;i++)
	{
		SPI_Flash_Read(DEBUG2_CODE_ADDR+i*APP_CODE_BUF_SIZE,appBuf,APP_CODE_BUF_SIZE);
		STMFLASH_Write(DEBUG1_CODE_ADDR+i*APP_CODE_BUF_SIZE,(unsigned short *)appBuf,APP_CODE_BUF_SIZE);		
	}

	if(secoff != 0)
	{
		SPI_Flash_Read(DEBUG2_CODE_ADDR+i*APP_CODE_BUF_SIZE,appBuf,secoff);
		STMFLASH_Write(DEBUG1_CODE_ADDR+i*APP_CODE_BUF_SIZE,(unsigned short *)appBuf,secoff+1);
	}

	QPRINTF("debug iap_update end \r\n");
	STMFLASH_Write(DEBUG1_VERSION_ADDR,(unsigned short *)app2VersionBuf,APP_CODE_BUF_SIZE);
	QPRINTF("NVIC_SystemReset... \r\n");
	NVIC_SystemReset();
}



void iap_load_app(u32 appxaddr)
{
	if (((*(__IO uint32_t*)appxaddr) & 0x2FFE0000 ) == 0x20000000)
	{
		jump_to_application = (pFunction) *(__IO uint32_t*) (appxaddr + 4);

		__set_MSP(*(__IO uint32_t*) appxaddr);
		jump_to_application();
	}

}

