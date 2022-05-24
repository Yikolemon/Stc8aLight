#include "stc8a.h"
#include "Uart.h"
#include "Timer0.h"
#include "string.h"
#include "stdio.h"
#include "FlashIAP.h"


sbit LED0=P2^0;
sbit LED1=P2^1;

extern bit SetEspToAPFlag;
extern char Uart2RecFlag;
extern char xdata Uart2RecBuff[100];
//��Dh11������data���ݣ����ڷ���
extern char DH11_date[18];

extern unsigned long T0Set;   //��ʱ������

char xdata ESP8266RecBuf[100];  //ESP8266���ݻ���

extern long ConnectErrorTimerSet;//ͨѶ���Ӷ�ʱ��

extern bit SendTickTimerFlag;
extern bit ConnectErrorTimerFlag;


char xdata FlashBuff[100];
char xdata APName[50];
char xdata APPassword[50];




//��ATָ���ESP8266����
//����1���������ַ���   ����2 ��ʱʱ��  ����3 �������ص��ַ���
//����ֵ�� 1--���ͳɹ�   -1--��ʾ����ʧ��
char SendATTo8266(char *ATStr,long OutTime,char *RetStr)
{
	//����ָ��
	Uart2SendString(ATStr);
	Uart2RecFlag=0;
	//�ȴ�����2�ظ�
	while(1)
	{
		T0Set=OutTime; //���ó�ʱʱ��
		while(Uart2RecFlag==0 && T0Set!=0); //�ȴ�����
		if(Uart2RecFlag==1)
		{//û�г�ʱ���������յ�����
			Uart2RecFlag=0;
			if(strcmp(Uart2RecBuff,RetStr)==0)
			{//���յ���λ�������
				return 1;
			}
		}
		else if(Uart2RecFlag==3)
		{//û�г�ʱ���յ�>
			Uart2RecFlag=0;
			return 1;
		}
		else if(T0Set==0)return -1;  //��ʱ����
		else if(strcmp(Uart2RecBuff,"ERROR\r\n")==0)
		{//�յ���ʱERROR������
			return -1;
		}
	}
}

//��ͨѶ�Է������ַ���
//����AT+CIPSENDָ��
//����1��ͨ����  ����2�����͵�����
//����ֵ��-1 ��ʾ����ʧ�ܣ�1 ��ʾ���ͳɹ�
char ESP8266SendData(int Channel,char *strdata)
{
	char xdata stemp[25];
	unsigned int len=strlen(strdata);  //��ȡ���ݳ���
	//��� AT+CIPSEND=Channel,len
	memset(stemp,0,25*sizeof(char));
	sprintf(stemp,"AT+CIPSEND=%d,%d\r\n",Channel,len);  // AT+CIPSEND=Channel,len\r\n
	if(SendATTo8266(stemp,5000,">")==1)
	{//���ͳɹ� �����յ�>�ţ���Ҫ���ͺ�������
		if(SendATTo8266(strdata,10000,"SEND OK\r\n"))
		{//���ͺ����ַ�����������
			return 1;
		}
		else return -1;
	}
	return -1;
}

//��ȡFlahs�е�AP
char ReadAPFromFlash()
{
	char i;
	memset(FlashBuff,0,100*sizeof(char));//�ȵ���:����
	FlashRead(0,FlashBuff);
	//�滻:Ϊ�ո�
	i=0;
	while(FlashBuff[i]!=':'&&FlashBuff[i]!=0)i++;
	if(FlashBuff[i]==0) return 0;
	FlashBuff[i]=' ';
	//�ָ��ַ���
	memset(APName,0,50*sizeof(char));
	memset(APPassword,0,50*sizeof(char));
	i=sscanf(FlashBuff,"%s %s",APName,APPassword);
	if(i!=2) return 0;
	else 
	{
		return 1;
	}
}

//ESP8266��ʼ��
char ESP8266Init()
{
	char stepflag=0;
	while(1)
	{
		//������λָ��
		if(SetEspToAPFlag){//����apģʽ
			break;//����ѭ��
		}
		if(stepflag==0)
		{
			SendATTo8266("AT+CIPCLOSE=0 \r\n\0",2000,"OK\r\n\0");
			if(SendATTo8266("AT+RST\r\n\0",2000,"ready\r\n")==1)
			{
				stepflag++;
			}
		}
		else if(stepflag==1)
		{//����ATָ��
			if(SendATTo8266("AT\r\n\0",100,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==2)
		{//����AT+CWMODE=2ָ��--����8266ΪSTAģʽ
			if(SendATTo8266("AT+CWMODE=1\r\n\0",500,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==3)
		{
			if(ReadAPFromFlash()==1)
			{//��ȡ�ɹ�
				LED0=0;
				memset(FlashBuff,0,100*sizeof(char));
				sprintf(FlashBuff,"AT+CWJAP=\"%s\",\"%s\"\r\n\0",APName,APPassword);	
			}
			else
			{//��ȡʧ��
				LED1=0;
				memset(FlashBuff,0,100*sizeof(char));
				//sprintf(FlashBuff,"AT+CWJAP=\"yikolemon\",\"88888888\"\r\n\0");	
				sprintf(FlashBuff,"AT+CWJAP=\"computer\",\"123456789\"\r\n\0");	
			}
			//����AT+CWSAP="yyds","12345678",1,3 ָ��--����8266�ȵ�����
			if(SendATTo8266(FlashBuff,30000,"OK\r\n")==1)
			//if(SendATTo8266("AT+CWJAP=\"yikolemon\",\"88888888\"\r\n\0",180000,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else 
			{
				stepflag=0;
			}
		}
		else if(stepflag==4)
		{//����AT+CIPMUX=1 ָ��--����Ϊ��·ģʽ
			if(SendATTo8266("AT+CIPMUX=1\r\n\0",500,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==5)
		{//����AT+CIPSERVER=1,8100 ָ��--����������
			//if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"114.115.237.170\",8000\r\n\0",10000,"OK\r\n")==1)
			//if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"10.109.236.75\",5001\r\n\0",10000,"OK\r\n")==1)
			if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"192.168.137.1\",7777\r\n\0",10000,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==6){
			//ESP8266SendData(0,FlashBuff);
			if(ESP8266SendData(0,"2120191799 duanfuqiang\n")==-1)
			{
				stepflag=0;
			}
			else//��Ϣ���ͳɹ�
			{
				//ConnectErrorTimerSet=180000;
				ConnectErrorTimerSet=60000;//���ó�ʱ����ʱ��
				SendTickTimerFlag=1;		//������
				ConnectErrorTimerFlag=1;//�����Ӵ�����
				return 1;
			}
		}
	}
}	



	//ESP8266����AP��ʼ��
char ESP8266Init_AP()
{
	char stepflag=0;
	while(1)
	{
		//������λָ��
		if(stepflag==0)
		{
			if(SendATTo8266("AT+RST\r\n\0",2000,"ready\r\n")==1)
			{
				stepflag++;
			}
		}
		else if(stepflag==1)
		{//����ATָ��
			if(SendATTo8266("AT\r\n\0",100,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==2)
		{//����AT+CWMODE=2ָ��--����8266ΪAPģʽ
			if(SendATTo8266("AT+CWMODE=2\r\n\0",500,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==3)
		{//����AT+CWSAP="yyds","12345678",1,3 ָ��--����8266�ȵ�����
			if(SendATTo8266("AT+CWSAP=\"yikolemon_8266\",\"12345678\",1,3\r\n\0",30000,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==4)
		{//����AT+CIPMUX=1 ָ��--����Ϊ��·ģʽ
			if(SendATTo8266("AT+CIPMUX=1\r\n\0",500,"OK\r\n")==1)
			{//��������
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==5)
		{//����AT+CIPSERVER=1,8100 ָ��--����������
			if(SendATTo8266("AT+CIPSERVER=1,8200\r\n\0",5000,"OK\r\n")==1)
			{//��������
				SendTickTimerFlag=0;		//�ر�����
				ConnectErrorTimerFlag=0;//�ر����Ӵ�����
				return 1;
			}
			else stepflag=0;
		}
	}
}
