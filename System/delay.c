#include "delay.h"

void delay(unsigned int num)
{
	unsigned int i;
	while (num--)
		for (i = 0;i < 110;i++);
}





