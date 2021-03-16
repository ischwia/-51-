#ifndef __HX711_H__
#define __HX711_H__

#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
//IO����
sbit dout=P1^3; 
sbit sck=P1^2; 
//�������߱�������
extern void delay_us(void);
extern unsigned int HX711_Read(void);

#endif