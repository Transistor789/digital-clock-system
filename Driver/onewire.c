#include "onewire.h"

sbit DQ = P3 ^ 7;  //单总线接口

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	while (t--);
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for (i = 0;i < 8;i++)
	{
		DQ = 0;
		DQ = dat & 0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;

	for (i = 0;i < 8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if (DQ)
		{
			dat |= 0x80;
		}
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
	bit initflag = 0;

	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;
	Delay_OneWire(80);
	DQ = 1;
	Delay_OneWire(10);
	initflag = DQ;
	Delay_OneWire(5);

	return initflag;
}

unsigned int rd_temperature(void)
{
	unsigned char high, low;

	init_ds18b20();		// 初始化
	Write_DS18B20(0xcc);// 跳过ROM
	Write_DS18B20(0x44);// 转换温度

	init_ds18b20();		// 初始化
	Write_DS18B20(0xcc);// 跳过ROM
	Write_DS18B20(0xbe);// 转换温度

	low = Read_DS18B20(); // 低8位
	high = Read_DS18B20();// 高8位

	return ((high << 8) | low);
}
