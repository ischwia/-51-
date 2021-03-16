#ifndef _UART_H
#define _UART_H
#include "reg52.h"

extern  unsigned int UART_RX_STA;         		//����״̬���	
extern  unsigned int UART_STA;         		//����״̬���	
extern  unsigned char UART_BUF[50];
void uart_init(void);

//�봮����ͨѶ
extern void Send_Byte(char mydata);
extern void Send_Str(char *s);
extern void Send_END(void);				//���ͽ�����
extern void Send_start(void);				//��ʼ��




#endif
