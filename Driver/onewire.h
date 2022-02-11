#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "REG52.H"

void Delay_OneWire(unsigned int t);
void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);

unsigned int rd_temperature(void);  

#endif // _ONEWIRE_H_
