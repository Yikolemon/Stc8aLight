#include "stc8a.h"
#include "intrins.h"


void IapIdle()
{
    IAP_CONTR = 0;                              //�ر�IAP����
    IAP_CMD = 0;                                //�������Ĵ���
    IAP_TRIG = 0;                               //��������Ĵ���
    IAP_ADDRH = 0x80;                           //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

char IapRead(int addr)
{
    char dat;

    IAP_CONTR = 0x80;                         //ʹ��IAP
    IAP_CMD = 1;                                //����IAP������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
    dat = IAP_DATA;                             //��IAP����
    IapIdle();                                  //�ر�IAP����

    return dat;
}

void IapProgram(int addr, char dat)
{
    IAP_CONTR = 0x80;                         //ʹ��IAP
    IAP_CMD = 2;                                //����IAPд����
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_DATA = dat;                             //дIAP����
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
    IapIdle();                                  //�ر�IAP����
}

void IapErase(int addr)
{
    IAP_CONTR = 0x80;                         //ʹ��IAP
    IAP_CMD = 3;                                //����IAP��������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();                                    //
    IapIdle();                                  //�ر�IAP����
}


void FlashRead(int addr,char *buff)
{
	char i=0;
	char temp;
	temp=IapRead(addr);
	while(temp!=0 && i<50)
	{
		buff[i]=temp;
		i++;
		temp=IapRead(addr+i);
	}
}

void FlashWrite(int addr,char *buff)
{
	char i=0;
	char temp;
	IapErase(addr);
	temp=buff[i];
	while(temp!=0 && i<50)
	{
		IapProgram(addr+i,temp);
		i++;
		temp=buff[i];
	}
	IapProgram(addr+i,0);
}