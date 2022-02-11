#include "intrins.h"
#include "ds1302.h"

sbit SCK = P3 ^ 6;
sbit SDA = P3 ^ 4;
sbit RST = P3 ^ 5;

void Write_Ds1302(unsigned  char temp)
{
	unsigned char i;
	for (i = 0;i < 8;i++)
	{
		SCK = 0;
		SDA = temp & 0x01;
		temp >>= 1;
		SCK = 1;
	}
}

void Write_Ds1302_Byte(unsigned char address, unsigned char dat)
{
	RST = 0;	_nop_();
	SCK = 0;	_nop_();
	RST = 1; 	_nop_();
	Write_Ds1302(address);
	Write_Ds1302(dat);
	// RST = 0;
}

unsigned char Read_Ds1302_Byte(unsigned char address)
{
	unsigned char i, temp = 0x00;
	RST = 0;	_nop_();
	SCK = 0;	_nop_();
	RST = 1;	_nop_();
	Write_Ds1302(address);
	for (i = 0;i < 8;i++)
	{
		SCK = 0;
		temp >>= 1;
		if (SDA)
			temp |= 0x80;
		SCK = 1;
	}
	// RST = 0;	_nop_();
	SCK = 0;	_nop_();
	SCK = 1;	_nop_();
	SDA = 0;	_nop_();
	SDA = 1;	_nop_();
	return (temp);
}

void set_rtc(unsigned char* rtc)
{ // rtc:时间数组指针
	unsigned char temp;
	Write_Ds1302_Byte(0x8e, 0); // 允许写操作

	temp = ((rtc[0] / 10) << 4) + (rtc[0] % 10);
	Write_Ds1302_Byte(0x84, temp); // 写入时
	temp = ((rtc[1] / 10) << 4) + (rtc[1] % 10);
	Write_Ds1302_Byte(0x82, temp); // 写入分
	temp = ((rtc[2] / 10) << 4) + (rtc[2] % 10);
	Write_Ds1302_Byte(0x80, temp); // 写入秒

	Write_Ds1302_Byte(0x8e, 0x80); // 禁止写操作
}

void rd_rtc(unsigned char* rtc)
{ // rtc:时间数组指针
	unsigned char temp;

	temp = Read_Ds1302_Byte(0x85); // 读取时
	rtc[0] = ((temp >> 4) * 10) + (temp & 0x0f);
	temp = Read_Ds1302_Byte(0x83); // 读取分
	rtc[1] = ((temp >> 4) * 10) + (temp & 0x0f);
	temp = Read_Ds1302_Byte(0x81); // 读取秒
	rtc[2] = ((temp >> 4) * 10) + (temp & 0x0f);
}
