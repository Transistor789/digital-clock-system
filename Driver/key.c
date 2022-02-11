#include "key.h"

sbit P30 = P3 ^ 0;
sbit P31 = P3 ^ 1;
sbit P32 = P3 ^ 2;
sbit P33 = P3 ^ 3;

uchar key_scan()
{
    if (P31 == 0)       return 1;
    else if (P30 == 0)  return 2;
    else if (P32 == 0)  return 3;
    else if (P33 == 0)  return 4;
    return 0;
}

