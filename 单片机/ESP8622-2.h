#ifndef __ESP8266-2__H__
#define __ESP8266-2__H__

//����2�õ��ı���
extern bit Uart2SendFinishFlag;   //����2�����ַ���ɱ�־  0--��ʾ�Ѿ���� 1--��ʾδ���
extern char xdata Uart2RecBuff[100];  //����2�Ľ��ջ���
extern char Uart2RecCount;          //����2�Ľ��ռ��������100
extern char Uart2RecFlag;            //����2���ձ�� 1--��ʾ���յ����ݣ����ݷ��ڻ�������ݳ���ΪUart2RecCount
extern unsigned int T0Set;   //��ʱ������

extern char xdata ESP8266RecBuf[100];  //ESP8266���ݻ���

#endif
