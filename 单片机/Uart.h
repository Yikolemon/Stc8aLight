#ifndef __UART_H_   //����һ��������ƣ���ʹ.h�ļ�ֻ����һ��
#define __UART_H_

void Uart1Init(void);		//57600bps@22.1184MHz
void Uart2Init(void);		//9600bps@22.1184MHz
void Uart2SendString(char *str); //��Ƭ���򴮿�2�����ַ���
#endif