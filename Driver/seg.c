#include "seg.h"

void convert(uchar* string, uchar* seg_code)
{// string:字符串指针 seg_code:段码数组指针
    char i = 0, j = 0, temp;
    for (i = 0;i < 8;i++, j++) {
        switch (string[j]) {
        case '0':temp = 0x3f;break;
        case '1':temp = 0x06;break;
        case '2':temp = 0x5b;break;
        case '3':temp = 0x4f;break;
        case '4':temp = 0x66;break;
        case '5':temp = 0x6d;break;
        case '6':temp = 0x7d;break;
        case '7':temp = 0x07;break;
        case '8':temp = 0x7f;break;
        case '9':temp = 0x6f;break;
        case 'C':temp = 0x39;break;
        case ' ':temp = 0x00;break;
        case '-':temp = 0x40;break;
        default:temp = ' ';
        }
        if (string[j + 1] == '.') {
            j++;
            temp |= 0x80;
        }
        seg_code[i] = temp;
    }
}
void display(uchar* seg_code, uchar pos)
{// seg_code:段码数组指针 pos:位置变量
    // 消影
    P0 = 0;
    P2 &= 0xe3;
    // 显示
    P0 = seg_code[pos]; //段选
    P2 = P2 & 0xe3 | ((7 - pos) << 2); // 位选
}