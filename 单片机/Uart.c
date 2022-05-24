#include "stc8a.h"
#include "string.h"

//����2�õ��ı���
bit Uart2SendFinishFlag=0;   //����2�����ַ���ɱ�־  0--��ʾ�Ѿ���� 1--��ʾδ���
char xdata Uart2RecBuff[100];  //����2�Ľ��ջ���
char Uart2RecCount=0;          //����2�Ľ��ռ��������100
char Uart2RecFlag=0;            //����2���ձ�� 1--��ʾ���յ����ݣ����ݷ��ڻ�������ݳ���ΪUart2RecCount
char Uart2RecType=0;         //����ģʽ��Ϊ0--����ģʽ ��\nΪ������>1Ϊ����ģʽ��ר�����ڽ���+IPD��ͷ������
char Uart2RecStepFlag=0;      //�������ģʽ�µĲ������
char Uart2RecNextCount=0;     //����ģʽ�º����ַ�����

extern long ConnectErrorTimerSet;

void Uart1Init(void)		//57600bps@22.1184MHz
{
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFE;		//�趨��ʱ��ֵ
	TH1 = 0xFE;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	ES=1;      //��������1�ж�
}

void Uart2Init(void)		//9600bps@22.1184MHz
{
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xC0;		//�趨��ʱ��ֵ
	T2H = 0xFD;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	IE2=ES2;     //��������2�ж�
}

//����1���жϷ������
void Uart1ISR() interrupt 4
{
	char temp;
	if(TI)
	{//��������ж�
		TI=0;
	}
	
	if(RI)
	{//��������ж�
		RI=0;
		temp=SBUF;
		S2BUF=temp;  //�����յ����ݷ��͸�����2
	}
}

//����2���жϷ������
void Uart2ISR() interrupt 8
{
	char temp;
	unsigned char itemp=0;
	char i;
	if(S2CON & 0x02)
	{//��������ж�
		S2CON &= ~0x02;
		Uart2SendFinishFlag=0;
	}
	
	if(S2CON & 0x01)  //xxxx  xx TI RI
	{//�����ж�
		S2CON &= ~0x01; //~0x01= ~(0000 0001)=1111 1110 =0xfe
		temp=S2BUF;
		SBUF=temp; //�����յ����ݷ��͸�����1
		
		//�����ݷ��뻺��
		Uart2RecBuff[Uart2RecCount]=temp;  //�����յ������ݷ��뻺��
		Uart2RecCount++;  
		//�жϽ���ģʽ
		if(Uart2RecType==0 && Uart2RecCount==1 && temp=='+')
		{//�������ģʽ
			Uart2RecType=1;
		}
		else if(Uart2RecType==0 && Uart2RecCount==1 && temp=='>')
		{//׼�������ַ���ģʽ
			Uart2RecType=2;
		}
		
		if(Uart2RecCount>=100)Uart2RecCount=0;      //����������100����0
		
		if(Uart2RecType==0)
		{//��������ģʽ
			//�ж��Ƿ���յ������ַ���
			if(temp=='\n')
			{//�ӵ�����
				Uart2RecFlag=1;  //�����ձ����1
				Uart2RecBuff[Uart2RecCount]=0;  //��ǽ����ַ�������
				if(strcmp(Uart2RecBuff,"0,CLOSED\r\n")==0)
				{//�Ͽ�������Ϣ
					ConnectErrorTimerSet=0;
				}
				Uart2RecCount=0;
			}
		}
		else if(Uart2RecType==1)
		{//������ģʽ����Ҫ���ݳ������ж��Ƿ����
			//+IPD���ַ���  +IPD,0,1234:qwr
			//����ֳ�3������
			//1.�洢��+�ŵ������ַ���
			//2.�ӣ���ʼ����ǰ�ң��ţ����ã��ŵ���֮�����ֵȡ������ת��Ϊ10�������������Ǻ����ֽڳ����趨����ֵ
			//3.ÿ���յ�һ���ַ������뻺�棬�������ȼ���ֵ��1��ֱ��Ϊ0����
			if(Uart2RecStepFlag==0)
			{//����1
				if(temp==':')
				{
					i=Uart2RecCount;  //��ȡ��ǰλ�� --�����һ��λ��
					while(Uart2RecBuff[i]!=',')i--;  //��ǰ��ģ���
					i++;  //����ƶ�һλ��ָʾ������λ��
					itemp=0; //��ֵ��0
					//�������ַ���ת��Ϊ10������ atoi()
					while(Uart2RecBuff[i]!=':')
					{
						itemp=itemp*10;
						itemp=itemp+(Uart2RecBuff[i]-0x30);
						i++;
					}
					Uart2RecNextCount=itemp;  //��������ַ�������
					//SBUF=Uart2RecNextCount+0x30;
					Uart2RecCount=0; 
					Uart2RecStepFlag++;
				}
			}
			else if(Uart2RecStepFlag==1)
			{//����2 ���պ����ַ� �ַ�������Uart2RecNextCount��
				Uart2RecNextCount--;
				if(Uart2RecNextCount==0)
				{//�����ַ��������
					Uart2RecBuff[Uart2RecCount]=0;  //�ַ����ض�
					Uart2RecFlag=2;  //�����ձ����2
					ConnectErrorTimerSet=60000;
					Uart2RecStepFlag=0;
					Uart2RecType=0; 
					Uart2RecCount=0;
				}	
			}
		}
		else if(Uart2RecType==2)
		{//���ַ����յ�>��
			Uart2RecFlag=3;
			Uart2RecType=0; 
			Uart2RecCount=0;
		}
	}
}

//��Ƭ���򴮿�2�����ַ���
void Uart2SendString(char *str)
{
	char i=0;
	while(str[i]!=0 && i<100)
	{
		S2BUF=str[i];
		Uart2SendFinishFlag=1;
		while(Uart2SendFinishFlag==1);
		i++;
	}
}