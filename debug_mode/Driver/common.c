#include "common.h"

void IntGetFourChar(unsigned char *buf,unsigned int val)
{
	buf[0] = (val>>24)&0xff;
	buf[1] = (val>>16)&0xff;
	buf[2] = (val>>8)&0xff;
	buf[3] = (val)&0xff;
}

void ShortGetTwoChar(unsigned char *buf,unsigned int val)
{
	buf[0] = (val>>8)&0xff;
	buf[1] = (val)&0xff;
}


unsigned int FourCharGetInt(unsigned char *buf)
{
	unsigned int val=0;

	val += buf[0];
	val = (val<<8)+buf[1];
	val = (val<<8)+buf[2];
	val = (val<<8)+buf[3];

	return val;
}

unsigned short TwoCharGetShort(unsigned char *buf)
{
	unsigned short val=0;

	val += buf[0];
	val = (val<<8)+buf[1];
	return val;
}



