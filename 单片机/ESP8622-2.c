#include "stc8a.h"
#include <intrins.h>   //ʹ����nop()������ʱ
#include <string.h>
#include "stdio.h"
#include "Uart.h"
#include "Timer0.h"
#include "ESP.h"
#include "ESP8622-2.h"
#include "stdlib.h"
#include "FlashIAP.h"
#include "Dh11.h"
#include "ADC.h"



//=========5.19=========

sbit LED0=P0^2;//��ʱ����̵���
extern bit lightShortFlag;
//=======================

#define SendTickSet 15000;//15��

extern unsigned int SendTickTimerSet;		//�����Ķ�ʱ��
extern long ConnectErrorTimerSet;		
extern bit soundFlag;
extern bit forgetLight;//û�صƱ�־

//��Dh11������data���ݣ����ڷ���
extern char DH11_date[20];
//��ADC���������ݣ����ڷ���
extern char light[10];
//extern char sound[10];
extern bit sound;

extern bit SetEspToAPFlag;

bit SendTickTimerFlag=1;
bit ConnectErrorTimerFlag=1;
//======================



char ansComm(char *Str,int *CommNo,char *CommDat)
{
	char xdata Head[5];
	//�����ַ��� �滻������BCCֵ
	int i=0;
	int itemp=0; //BCC����
	int Count=0; //ͳ�ƣ�������
	int BccDat=0;
	while(Str[i]!=0)
	{
		if(Str[i]==',')
		{
			Count++;
			itemp^=Str[i];
			Str[i]=' ';
		}
		else if(Count<3)
		{
			itemp^=Str[i];
		}
		i++;
	}
	i=sscanf(Str,"%s %d %s %d;",Head,CommNo,CommDat,&BccDat);
	if(i==4)
	{
		if(strcmp(Head,"NJUE")==0)
		{
			if(itemp==BccDat)
			{
				return 1;
			}
		}
	}
	return 0;
}



main()
{
	
	
	int i=0,itemp=0;
	int No,Timer;
	int CommNo;
	char xdata CommDat[20];
	
	Uart1Init();
	Uart2Init();
	Timer0Init();
	EA=1;
	
	//5.19	Delay1500ms();  //DHT11�ϵ��Ҫ�ȴ�1.5S��Խ�����ȶ�״̬�ڴ��ڼ䲻�ܷ����κ�ָ��
	Delay1500ms();  //DHT11�ϵ��Ҫ�ȴ�1.5S��Խ�����ȶ�״̬�ڴ��ڼ䲻�ܷ����κ�ָ��
	ESP8266Init();	
	
	P0M0 = 0x04;                                //����P0.2Ϊ�������
	P0M1 = 0x00;
	
	
	while(1)
	{
		//=========5.19===========
//		if(Dh11Flag){
//			DHT11_receive();
//			ESP8266SendData(0,DH11_date);
//			Dh11Flag=0;
//			//���͹�������
//		}
		//ESP8266SendData(0,);
		//=========================
//		if(soundFlag){
//			ESP8266SendData(0,"�������ˣ�ע��/r/n");
//			soundFlag=0;
//		}
		getSound();
		if(sound)//��⵽����
		{
			ESP8266SendData(0,"this is sound\r\n");
			sound=0;
		}
		if(Uart2RecFlag==2)
		{//���յ�����
			Uart2RecFlag=0;
			while(!TI);
			TI=0;	
			memset(ESP8266RecBuf,0,100*sizeof(char));
			strcpy(ESP8266RecBuf,Uart2RecBuff);
			//��P2����������������
			if(strcmp(ESP8266RecBuf,"TICK\r\n\0")!=0)//�ж��ǲ���TICK,�����ȷ���0,�������ʾ�ⲻ��TICK
			{
				i=ansComm(ESP8266RecBuf,&CommNo,CommDat);
				if(i==1)
				{//�����ɹ�
					switch(CommNo)
					{
						case 1:
						{//����led����Ϊ��ȡ��ʪ��
							//itemp=atoi(CommDat);//��ȡ��Ų���Ҫ
//							switch(itemp)
//							{
								DHT11_receive();
								ESP8266SendData(0,DH11_date);
							//}
								break;
						}
						case 2:
						{//���led
							//itemp=atoi(CommDat);//��ȡ���
							//switch(itemp)
							//{
							//}
							getLight();
							ESP8266SendData(0,light);
							LED0=0;
							break;
						}
						case 3:
						{//���ü̵���״̬
							itemp=atoi(CommDat);//��ȡ���
							switch(itemp)
							{
								case 1:
								{//����Ϊͨ· NJUE,3,1,58;
									forgetLight=0;
									P0M0 = 0x04;                                //����P0.2Ϊ�������
									P0M1 = 0x00;
									LED0=0;
									break;
								}
								case 2:
								{//����Ϊ��· NJUE,3,2,57;
									forgetLight=1;
									P0M0 = 0x04;                                //����P0.2Ϊ�������
									P0M1 = 0x00;
									LED0=0xff;
									break;
								}
							}
							break;
						}
						case 4:
						{//�����ȵ�
							FlashWrite(0,Com?mDat);
							ESP8266Init();
							//memset(APName,0,30*sizeof(char));
							//FlashRead(0,APName);
							//ESP8266SendData(0,APName);
							break;
						}
						case 5:
						{//��������5s
							lightShortFlag=1;//���ö������Ʊ�־
							break;
						}
						break;
					}
				}
				else
				{
					ESP8266SendData(0,"Error\r\n\0");
				}
				//P2=~i;
			}
			else//�յ�����TICK�����и���Tick��ʱ
			{
				ConnectErrorTimerSet=60000;
			}
			//��ʾ����
//			i=0;
//			while(ESP8266RecBuf[i]!=0) //12345
//			{
//				SBUF=ESP8266RecBuf[i];
//				while(!TI);
//				TI=0;
//				i++;
//			}
//			i=strlen(ESP8266RecBuf);  //�ַ�������   12345678  8
//			ESP8266RecBuf[i-1]++;     //���һ���ֽ����ݼ�1
//			ESP8266SendData(0,ESP8266RecBuf);
		}
		
		
		if(SendTickTimerSet==0 && SendTickTimerFlag)
		{//������Ϣ����
			memset(ESP8266RecBuf,0,100*sizeof(char));
			strcpy(ESP8266RecBuf,"TICK\r\n");
			if(ESP8266SendData(0,ESP8266RecBuf)==-1)
			{//����ʧ��
				ESP8266Init();
			}
			//ESP8266SendData(0,"shit\n");
			else SendTickTimerSet=SendTickSet;
		}
		
		if(ConnectErrorTimerSet==0 && ConnectErrorTimerFlag)
		{//3����δ�յ�����������
			ESP8266Init();
		}
		
		if(SetEspToAPFlag)
		{//�����ȵ㰴��������
			SetEspToAPFlag=0;
			ESP8266Init_AP();
		}
	}
}