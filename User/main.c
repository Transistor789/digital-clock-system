#include "timer.h"	// 定时器
#include "key.h"	// 按键
#include "seg.h"	// 数码管
#include "ds1302.h"	// DS1032时钟芯片
#include "i2c.h"	// AT24C02外部存储芯片
#include "onewire.h"// DS18B20温度传感器
#include "delay.h"	// 软件延时

sbit beep = P1 ^ 5; // 蜂鸣器引脚定义
uint ms = 0; // 毫秒变量
// 数码管字符串数组、段码数组、显示位置
uchar seg_string[10], seg_code[9], pos = 0;
// 时间数组、闹钟数组、缓存数组
uchar rtc[3] = { 23, 59, 55 }, alarm[3] = { 22,0,0 }
, temp[3] = { 0,0,0 };
bit alarm_state = 0, alarm_enable = 0; // 闹钟标志位
uint temperature = 0; // 温度
uchar mode = 0; // 模式

void eeprom_init();	// eeprom初始化
void alarm_proc();	// 闹钟处理
void seg_proc();	// 数码管处理
void key_proc();	// 按键处理

void main()
{
	eeprom_init();	// eeprom初始化
	timer_init();	// 定时器初始化
	set_rtc(rtc);	// 设置时间

	while (1)
	{
		seg_proc();		// 数码管处理
		key_proc();		// 按键处理
		alarm_proc();	// 闹钟处理
	}
}
void timer() interrupt 1
{
	TH0 = (65536 - 922) / 256; // 重装初值
	TL0 = (65536 - 922) % 256;
	if (++ms == 1000)	ms = 0; // ms变量以1000为周期
	if (alarm_enable && alarm_state) // 蜂鸣器工作条件
		beep = ~beep; // 2ms周期即输出500Hz方波
	display(seg_code, pos);	// 数码管显示
	if (++pos == 8)	pos = 0;// 周期8ms即刷新率为125Hz 
}
void eeprom_init()
{
	uchar flag, i;
	flag = At24c02Read(0x06); // 读取数据

	if (flag == 0x66) { // 如果为特定值则读取数据
		for (i = 0;i < 3;i++) {
			rtc[i] = At24c02Read(i);
			delay(1);
			alarm[i] = At24c02Read(i + 3);
			delay(1);
		}
	}
	else { // 否则写入
		for (i = 0;i < 3;i++) {
			At24c02Write(i, rtc[i]);
			delay(1);
			At24c02Write(i + 3, alarm[i]);
			delay(1);
		}
		At24c02Write(6, 0x66);
	}
}
void alarm_proc()
{
	uchar i;
	if (ms)	return; // 1000ms周期

	if (rtc[0] == alarm[0] && rtc[1] == alarm[1] && rtc[2] == alarm[2])
		alarm_state = 1, alarm_enable = 1; // 开启闹钟状态和使能标志位
	else if (alarm_enable == 1 && rtc[0] == temp[0] && rtc[1] == temp[1] && rtc[2] == temp[2])
		alarm_state = 1;// 再次开启闹钟状态标志位

	if (alarm_state && alarm_enable && rtc[2] == 30) {
		alarm_state = 0; // 30秒自动关闭
		temp[1] += 2;
	}
	else if (!alarm_state && !alarm_enable) {
		for (i = 0;i < 3;i++)
			temp[i] = alarm[i];
	}
	// 数据边界处理
	if (temp[1] > 59) {
		temp[1] -= 60;
		if (++temp[0] == 24) temp[0] = 0;
	}
}
void key_proc()
{
	static uchar key_val, key_down, key_old;
	uchar i;
	if (ms % 10)	return; // 10ms周期

	// 按键消抖
	key_val = key_scan();
	key_down = key_val & (key_val ^ key_old);
	key_old = key_val;

	switch (key_down)
	{
	case 1:
		// 模式轮换以及数据存储
		if (mode == 0x20)
			mode = 0;
		else if (mode & 0x0f) {
			mode &= 0xf0;
			for (i = 0;i < 3;i++) {
				At24c02Write(i, rtc[i]);
				delay(1);
				At24c02Write(i + 3, alarm[i]);
				delay(1);
			}
		}
		else
			mode = (mode + 0x10) & 0xf0;
		P2 = ~((mode & 0xf0) << 2); // LED指示灯
		break;
	case 2:// 进入设置模式 修改数值轮换
		mode++;
		if ((mode < 0x10 && (mode & 0x0f) == 4)
			|| (mode >= 0x10 && mode < 0x20 && (mode & 0x0f) == 3))
			mode &= 0xf0;
		break;
	case 3:
		if ((mode & 0x0f) != 0) {
			if (mode < 0x10) {
				switch (mode)
				{
				case 0x01:
					if (++rtc[0] > 23)	rtc[0] = 0;break;
				case 0x02:
					if (++rtc[1] > 59)	rtc[1] = 0;break;
				case 0x03:
					if (++rtc[1] == 60)	rtc[1] = 0;
					rtc[2] = 0;break;
				}
				set_rtc(rtc);
			}
			else {
				switch (mode)
				{
				case 0x11:
					if (++alarm[0] > 23)	alarm[0] = 0;break;
				case 0x12:
					if (++alarm[1] > 59)	alarm[1] = 0;break;
				}
			}
		}
		if (alarm_state)
			alarm_state = 0, temp[1] += 2;
		break;
	case 4:
		if ((mode & 0x0f) != 0) {
			if (mode < 0x10) {
				switch (mode)
				{
				case 0x01:
					if (--rtc[0] > 23)	rtc[0] = 23;break;
				case 0x02:
					if (--rtc[1] > 59)	rtc[1] = 59;break;
				case 0x03:
					rtc[2] = 0;break;
				}
				set_rtc(rtc);
			}
			else {
				switch (mode)
				{
				case 0x11:
					if (--alarm[0] > 23)	alarm[0] = 23;break;
				case 0x12:
					if (--alarm[1] > 59)	alarm[1] = 59;break;
				}
			}
		}
		if (alarm_state || alarm_enable)
			alarm_enable = 0;
		break;
	default:
		break;
	}
}
void seg_proc()
{
	if (ms % 500)	return; // 500ms周期

	rd_rtc(rtc);
	if (mode >= 0 && mode < 0x10) { // 显示时间
		seg_string[0] = 48 + rtc[0] / 10;
		seg_string[1] = 48 + rtc[0] % 10;
		seg_string[2] = '-';
		seg_string[3] = 48 + rtc[1] / 10;
		seg_string[4] = 48 + rtc[1] % 10;
		seg_string[5] = '-';
		seg_string[6] = 48 + rtc[2] / 10;
		seg_string[7] = 48 + rtc[2] % 10;
	}
	else if (mode < 0x20) { // 显示闹钟
		seg_string[0] = 48 + alarm[0] / 10;
		seg_string[1] = 48 + alarm[0] % 10;
		seg_string[2] = '-';
		seg_string[3] = 48 + alarm[1] / 10;
		seg_string[4] = 48 + alarm[1] % 10;
		seg_string[5] = '-';
		seg_string[6] = 48 + alarm[2] / 10;
		seg_string[7] = 48 + alarm[2] % 10;
	}
	else { // 显示温度
		EA = 0;
		temperature = rd_temperature();
		EA = 1;
		seg_string[0] = 'C';
		seg_string[1] = ' ';
		seg_string[2] = ' ';
		seg_string[3] = ' ';
		seg_string[4] = ' ';
		seg_string[5] = 48 + (temperature >> 4) / 10;
		seg_string[6] = 48 + (temperature >> 4) % 10;
		seg_string[7] = '.';
		seg_string[8] = 48 + (temperature / 16.0 - (temperature >> 4)) * 10;
	}
	// 设置模式闪烁
	if (mode < 0x20 && (mode & 0x0f) != 0 && ms < 500) {
		seg_string[((mode & 0x0f) - 1) * 3] = ' ';
		seg_string[((mode & 0x0f) - 1) * 3 + 1] = ' ';
	}
	convert(seg_string, seg_code); // 字符串->段码
}