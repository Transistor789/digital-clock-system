#ifndef _DS1302_H_
#define _DS1302_H_

#include "REG52.H"

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte(unsigned char address, unsigned char dat);
unsigned char Read_Ds1302_Byte(unsigned char address);
void set_rtc(unsigned char* rtc);
void rd_rtc(unsigned char* rtc);

#endif // _DS1302_H_
