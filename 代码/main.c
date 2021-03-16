#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "HX711.h"
#include "eeprom52.h"

#define uchar unsigned char
#define uint  unsigned int
	
char table[50];

unsigned long HX711_Buffer = 0;
unsigned long Weight_Maopi = 0;
unsigned long Weight_Maopi_0 = 0;
unsigned int qupi=0;
unsigned long Weight_Shiwu = 0;


unsigned  GapValue=500,GapValue1;

float temp=1420;
float idata price;     //���ۣ�������ֵ����λΪ��   
float idata money;     //�ܼۣ�������ֵ����λΪ��

volatile bit FlagTest = 0;		//��ʱ���Ա�־��ÿ0.5����λ��������0

sbit LED=P1^1;
sbit Buzzer = P1^0;

volatile bit ClearWeighFlag = 0; //�����������־λ�����0Ư

void Delay_ms(unsigned int n);
void Get_Maopi();
void Get_Weight();

//��������
void write_eeprom()
{
	SectorErase(0x1000);
	GapValue1=GapValue&0x00ff;
	byte_write(0x2000, GapValue1);
	GapValue1=(GapValue&0xff00)>>8;
	byte_write(0x2001, GapValue1);
	byte_write(0x2060, a_a);	
}

//��ȡ����
void read_eeprom()
{
	GapValue   = byte_read(0x2001);
	GapValue   = (GapValue<<8)|byte_read(0x2000);
	a_a      = byte_read(0x2060);
}

//��ʼ��
void init_eeprom() 
{
	read_eeprom();		
	if(a_a != 1)		//�µĵ�Ƭ����ʼ��Ƭ������eeprom
	{
		GapValue  = 1475;
		a_a = 1;
		write_eeprom();	   //��������
	}	
}
void Data_Init()
{
   price = 0;
}
//��ʱ��0��ʼ��
void Timer0_Init()
{
	ET0 = 1;        
	TMOD |= 1;      
	TL0 = 0xb0;     
	TH0 = 0x3c;    
	TR0 = 1;     
}


void Timer0_ISR (void) interrupt 1 using 0
{
	uchar Counter;
	TL0 = 0xb0;
	TH0 = 0x3c;     //��ʱ�������ֵ
					//ÿ0.5����ˢ������
    Counter ++;
    if (Counter >= 10)
    {
       FlagTest = 1;
	   Counter = 0;
    }
}

//t0�ŵ���
void Display_Price()
{
	Send_Str("����");
	sprintf(table,"%f",price);
	Send_Str(table);
	Send_END();		
}
//t1������
void Display_Weight()
{
	Send_Str("t1.txt=\"");
	sprintf(table,"%ld",Weight_Shiwu);
	Send_Str(table);
	Send_Str("\"");
	Send_END();		
}
//t2���ܼ�
void Display_Money()
{
	Send_Str("t2.txt=\"");
	sprintf(table,"%.2f",money);
	Send_Str(table);
	Send_Str("\"");
	Send_END();		
}	

void main()
{
	init_eeprom(); 
  Data_Init();
	uart_init();
	Send_start();
  Timer0_Init();
  EA = 1;//����ʼ����ɣ�����
	LED=0;
	Get_Maopi();
	LED=1;
	while(1)
	{
		 if (FlagTest==1)
		{
			Get_Weight();
			
			FlagTest = 0;
		}	
		
		if(UART_STA==1)
		{
			if(strcmp(UART_BUF,"BIC")==0)
			{
				Buzzer=0;
				Delay_ms(15);
				Buzzer=1;
				price=0;
				Display_Price();
			}
			else if(strcmp(UART_BUF,"qupi")==0)
			{
				Buzzer=0;
				Delay_ms(15);
				Buzzer=1;
				if(qupi==0)qupi=Weight_Shiwu;
				else qupi=0;
				Display_Price();
			}
			else if(strcmp(UART_BUF,"jia")==0)
			{
				if(temp<10000)
				temp-=1;
				Buzzer=0;
				Delay_ms(15);
				Buzzer=1;
//				Send_Str("temp:");
//				sprintf(table,"%f",temp);
//				Send_Str(table);
//				Send_Str("\r\n");
				
			}
			else if(strcmp(UART_BUF,"jian")==0)
			{
				if(temp>0.5)
				temp+=1;
				Buzzer=0;
				Delay_ms(15);
				Buzzer=1;
//				Send_Str("temp:");
//				sprintf(table,"%f",temp);
//				Send_Str(table);
//				Send_Str("\r\n");				
			}
			else if(strcmp(UART_BUF,"reset")==0)
			{
				Get_Maopi();
				Buzzer=0;
				Delay_ms(15);
				Buzzer=1;
				Send_Str("t4.txt=\"");
				Send_Str("�๦�ܵ��ӳ����(������)");
				Send_Str("\"");
				Send_END();
			}
			else 
			{
				//Buzzer=0;
				Delay_ms(15);
				Buzzer=1;				
				price=atoi(UART_BUF);			
			}
			
			UART_STA=0;
			memset(UART_BUF,0,sizeof(UART_BUF));
			UART_RX_STA=0;	
		}
	}
	
	
}

void Get_Maopi()
{
	unsigned char clear;
mm:	Weight_Maopi_0 = HX711_Read();
	for(clear=0;clear<10;clear++)
	{
		Buzzer=1;
		LED=1;
		Delay_ms(100);
		LED=0;
		Delay_ms(100);	
	}
	Weight_Maopi = HX711_Read();
	if(Weight_Maopi/GapValue!=Weight_Maopi_0/GapValue)
	{
		goto mm;
	}
	if(Weight_Maopi==32293|Weight_Maopi==32291)Weight_Maopi=32292;
	Buzzer=0;
	Delay_ms(500);
	Buzzer=1;
	
}
void Get_Weight()
{
	m1:	Weight_Shiwu = HX711_Read();
	if(Weight_Shiwu==32293|Weight_Shiwu==32291)Weight_Shiwu=32292;
	Send_Str("t3.txt=\"");
	sprintf(table,"%ld",Weight_Shiwu);
	Send_Str(table);
	Send_Str("\"");
	Send_END();
	Weight_Shiwu = (unsigned int)((float)(Weight_Shiwu - Weight_Maopi)*1000/temp)-qupi;		//��ȡ����
	if(Weight_Shiwu==32293)
	{
		Get_Maopi();//���ܻ���ֶ���Weight_Maopi�ȱ�׼ֵС1��ʹ��Weight_Shiwu��������һ�������ݴ�����ֵΪ12032
		goto m1;	
	}
	if(Weight_Shiwu > 2000)		//���ر���
	{
		Buzzer = !Buzzer;	
		LED=!LED;
		Send_Str("t4.txt=\"");
		Send_Str("���ӳ��쳣������");
		Send_Str("\"");
		Send_END();
		Delay_ms(100);
		
	}
	else
	{
		Send_Str("t4.txt=\"");
				Send_Str("�๦�ܵ��ӳ����(������)");
				Send_Str("\"");
				Send_END();
		if(Weight_Shiwu==0) 
		LED=1;
		else if(Weight_Shiwu>0)
		LED=0; 
		//Buzzer = 1;
		Display_Weight();
		money = Weight_Shiwu*(price/1000);  
	       //��ʾ�ܽ��
	    Display_Money();
	}

}

void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}	

