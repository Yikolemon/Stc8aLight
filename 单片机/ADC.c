#include "stc8a.h"
#include "stdio.h"
#include "intrins.h"

unsigned int dat;
float ftemp;
char light[10];
//char sound[10];
bit sound;

void getLight()
{
	
    P1M0 = 0x00;                            //����P1.2Ϊ��������ģʽ��1.2�ù�������
    P1M1 = 0x04;
		
    ADCCFG = 0x2f;                          //����ADCʱ��Ϊϵͳʱ��/2/16/16
    ADC_CONTR = 0x80;                       //ʹ��ADCģ��

		ADC_CONTR |= 0x42;                      //����ADת�� ��P1.2Ϊadc��
		_nop_();
		_nop_();
		while (!(ADC_CONTR & 0x20));            //��ѯADC��ɱ�־
		ADC_CONTR &= ~0x20;                     //����ɱ�־
		//ADC_RES(��4λ) ADC_RESL����8λ��;     //��ȡADC���
		dat=ADC_RES*256+ADC_RESL;
		ftemp=dat;
		ftemp=ftemp*3.3/256;
		///4096
		sprintf(light,"L:%.2f\r\n\0",ftemp);
		//lightΪ���
	
}

void getSound()
{
	
		P1M0 = 0x00;                                
		P1M1 = 0x08;														//����P1.3Ϊ��������ģʽ��1.3�������ش�����

    ADCCFG = 0x2f;                          //����ADCʱ��Ϊϵͳʱ��/2/16/16
    ADC_CONTR = 0x80;                       //ʹ��ADCģ��

		ADC_CONTR |= 0x43; 											//����ADת�� ��P1.3Ϊadc��
		_nop_();
		_nop_();
		while (!(ADC_CONTR & 0x20));            //��ѯADC��ɱ�־
		ADC_CONTR &= ~0x20;                     //����ɱ�־
		//ADC_RES(��4λ) ADC_RESL����8λ��;     //��ȡADC���
		dat=ADC_RES*256+ADC_RESL;
		ftemp=dat;
		ftemp=ftemp*3.3/256;
		if(ftemp>8)
		{
			sound=1;
		}
		///4096
		//sprintf(sound,"S:%.2f\r\n\0",ftemp);
		//soundΪ���
		
}
	
