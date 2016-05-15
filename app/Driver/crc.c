#include "crc.h"

#define Poly 0xEDB88320L	//CRC32±ê×¼
static unsigned int crc_tab32[256];

static void init_crc32_tab( void ) 
{
	int i, j;
	unsigned int crc;
 
	for (i=0; i<256; i++)
	{
		crc = (unsigned long)i;
		for (j=0; j<8; j++) 
		{
			if ( crc & 0x00000001L )
				crc = ( crc >> 1 ) ^ Poly;
			else      
				crc = crc >> 1;
		}
		crc_tab32[i] = crc;
	}
}

unsigned int get_crc32(unsigned int crcinit, unsigned char * bs, unsigned int bssize)
{
	unsigned int crc = crcinit^0xffffffff;
 
	init_crc32_tab();
	while(bssize--)
		crc=(crc >> 8)^crc_tab32[(crc & 0xff) ^ *bs++];
 
	return crc ^ 0xffffffff;
}



