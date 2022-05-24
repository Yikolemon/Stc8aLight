#include "stc8a.h"
//#include "ADC.h"

unsigned long T0Set=0;   //��ʱ������
unsigned int SendTickTimerSet=0;		//�����Ķ�ʱ��
long ConnectErrorTimerSet=60000;		//ͨѶ���Ӷ�ʱ��
unsigned int soundTime=100;//0.1sһ��
//Dh11�Ķ�ʱ��

extern float sound;

sbit Key=P0^3;//����������ȵ�İ���
unsigned int KeyCount=0;
bit SetEspToAPFlag=0;

unsigned int LightFive=5000;
bit lightShortFlag=0;//�������Ʊ�־
sbit LED0=P0^2;//��ʱ����̵���
bit forgetLight=0;//���ǹصƱ�־

bit soundFlag=0;



//��ʱ��0��ʼ��
void Timer0Init(void)		//1����@22.1184MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x9A;		//���ö�ʱ��ֵ
	TH0 = 0xA9;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;   //������ʱ��0���ж�
}


//��ʱ��0���жϷ������
void T0Isr() interrupt 1
{
	TL0 = 0x9A;		//���ö�ʱ��ֵ
	TH0 = 0xA9;		//���ö�ʱ��ֵ
	
	if(Key==0)
	{
		KeyCount++;//���Ϊ5000�򵽴�
		if(SetEspToAPFlag)
		{
			KeyCount=0;//KeyCount����Ϊ0	//��ֹһ�ΰ���10s���ظ���������
		}
		if(KeyCount>=5000)
		{//5���
			KeyCount=0;//KeyCount����Ϊ0
			SetEspToAPFlag=1;//�����Ϊ1
		}
	}
	if(T0Set)T0Set--;
	if(SendTickTimerSet)SendTickTimerSet--;
	if(forgetLight){//���ǹصƣ��Ž�������ж�
		if(LightFive)//����ʱ�����0
		{
			LightFive--;
		}
		else{
			if(lightShortFlag==1){//˵��Ҫ����
				LightFive=5000;//����5s
				LED0=0;//����������
				lightShortFlag=0;
			}
			else{//˵������
				LED0=0xff;
			}
		}
	}
	
	//if(ConnectErrorTimerSet)ConnectErrorTimerSet--;
	//���������������ļ��
//	if(soundTime>0)
//	{
//		soundTime--;
//	}
//	else
//	{
//		//����0.1s��ad���һ��
//		getSound();
//		if(sound>3)
//		{
//			//˵���������ˣ�Ҫ������Ϣ���߷����,ͬʱ�ӳ����ʱ��
//			soundTime=5000;//5s���ٽ��м��
//		}
//		else
//		{//û������������0.1s���
//			soundTime=100;
//		}
//	}
	
	
}
