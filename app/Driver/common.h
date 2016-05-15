#ifndef _COMMON_H_
#define _COMMON_H_
#include "stmflash.h"

void IntGetFourChar(unsigned char *buf,unsigned int val);
void ShortGetTwoChar(unsigned char *buf,unsigned int val);
unsigned int FourCharGetInt(unsigned char *buf);
unsigned short TwoCharGetShort(unsigned char *buf);

void flash_read(unsigned int addr,unsigned char *buf,unsigned short size);


#endif

