#include "stc8a.h"
#include "stdio.h"
#include "intrins.h"
#include "math.h"

#define uchar unsigned char 
#define uint unsigned int
	
uchar DH11_date[20];   //������ʾ�Ľ�����������

	
//���Թ���Ƶ��Ϊ22.1184MHz
	
bit Uar2SendFlag=0;
sbit Data = P3^6;//����������
unsigned char temp;
unsigned char checknum;
unsigned char loopnum=1;
	
//==============�ӳ�10ms===========
void Delay10ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 32;
	k = 60;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}




//==============�ӳ�1000ms===========
void Delay1000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 113;
	j = 53;
	k = 228;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}





//==============�ӳ�1500ms===========
void Delay1500ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 169;
	j = 80;
	k = 87;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



//==============��ʱ3us================
void Delay3us()		//@22.1184MHz
{
	unsigned char i;

	i = 20;
	while (--i);
}



//==============��ʱ5us================
void Delay5us()		//@22.1184MHz
{
	unsigned char i;

	i = 35;
	while (--i);
}


//==============��ʱ7us================
void Delay7us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 49;
	while (--i);
}



//==============��ʱ8us================
void Delay8us()		//@22.1184MHz
{
	unsigned char i;

	i = 57;
	while (--i);
}


//==============��ʱ10us================
void Delay10us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 71;
	while (--i);
}

//==============��ʱ20us================
void Delay20us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	i = 145;
	while (--i);
}



//==============DHT11��ʼ�ź�=============
             
void DHT11_start()
{
	Data=0;
	Delay10ms();   //��ʱ18ms����
	Delay10ms();
	Data=1;
	Delay10us();
	Delay3us();
}

//=============���հ�λ������================            
uchar DHT11_rec_byte()      //����һ���ֽ�
{
	uchar i,dat=0;
	for(i=0;i<8;i++)    //�Ӹߵ������ν���8λ����
	{          
		while(!Data);
		Delay20us();
		Delay10us(); 		//��ʱ30us�������Ϊ��������Ϊ1������Ϊ0 ����Ϊ27-28us�ߵ�ƽΪ0��70usΪ1��������30usʱ
		dat<<=1;        //��λʹ��ȷ����8λ���ݣ�����Ϊ0ʱֱ����λ
		if(Data==1)    			 //dh11һλ���ͽ����������յ�һλ�жϼ���dat��
		{
			dat+=1;
		}
		while(Data);    //�ȴ�����������    
	}
	return dat;
}

//=============DH11����40bit����==================
void DHT11_receive()      //����40λ������
{
	unsigned char R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
	DHT11_start();			//����DHT11_start������ʹDh11������׼���׶�ת��������״̬
	if(Data==0)
	{
		while(Data==0);   //�ȴ�����     
		Delay20us();
		Delay20us();
		Delay20us();
		Delay20us();
		Delay7us();
	//���ߺ���ʱ80us
		R_H=DHT11_rec_byte();    //����ʪ�ȸ߰�λ  
		R_L=DHT11_rec_byte();    //����ʪ�ȵͰ�λ  
		T_H=DHT11_rec_byte();    //�����¶ȸ߰�λ  
		T_L=DHT11_rec_byte();    //�����¶ȵͰ�λ
		revise=DHT11_rec_byte(); //����У��λ
	 
		Delay20us();
		Delay5us();
	
		if((R_H+R_L+T_H+T_L)==revise)      //У��
		{
			RH=R_H;
			RL=R_L;
			TH=T_H;
			TL=T_L;
		}
		/*���ݴ���������ʾ*/
		//char����洢Dh11�ռ�������
		DH11_date[0]='0'+(RH/10);
		DH11_date[1]='0'+(RH%10);
		DH11_date[2] ='.';
		DH11_date[3]='0'+(RL/10);
		DH11_date[4]='0'+(RL%10);
		DH11_date[5] = 'R' ;
		DH11_date[6] = 'H';
		DH11_date[7]=' ';
		DH11_date[8]=' ';
		DH11_date[9]=' ';
		DH11_date[10]=' ';
		DH11_date[11]='0'+(TH/10);
		DH11_date[12]='0'+(TH%10);
		DH11_date[13] ='.';
		DH11_date[14]='0'+(TL%10);
		DH11_date[15]='0'+(TL/10);
		DH11_date[16] = 'C';
		DH11_date[17] = '\r';
		DH11_date[18] = '\n';
		DH11_date[19] = '\0';
	}
}