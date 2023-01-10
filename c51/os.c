#include <STC8F.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)
typedef unsigned char u8;
typedef unsigned int u16;

sbit IN1=P1^3;
sbit IN2=P1^2;
sbit IN3=P3^6;
sbit IN4=P3^7;
sbit IN5=P1^6;
sbit IN6=P1^7;
sbit IN7=P1^4;
sbit IN8=P1^5;
sbit nSLEEP=P3^3;
sbit nFAULT=P3^2;
sbit LED=P3^4;

bit busy;
bit sw=0;
u8 wptr,rptr,buffer[16],*ID;
u8 it=0,Ln=60;

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 56;
	k = 172;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay200ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 25;
	j = 90;
	k = 176;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay100ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 13;
	j = 45;
	k = 214;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void IO_Init()
{
	P1M0 = 0xfc; P1M1 = 0x03; 
	P3M0 = 0xca; P3M1 = 0x25; 
	P5M0 = 0x00; P5M1 = 0xff; 

	IN1=0;IN2=0;IN5=0;IN6=0;
	nSLEEP=0;Delay10ms();nSLEEP=1;
	Delay10ms();
}

void Timer4Init()
{
	T4T3M &= 0xDF;			//?????12T??
	T4L = 0xA0;				//???????
	T4H = 0x15;				//???????
	T4T3M |= 0x80;			//???4????
	IE2 = 0x40; 
}

void UartInit()
{
	SCON = 0x50;
	T2L = BRT;
	T2H = BRT >> 8;
	AUXR = 0x15;
	wptr = 0x00;
	rptr = 0x00;
	busy = 0;
}

void UartSend(char dat)
{
	while (busy);
	busy = 1;
	SBUF = dat;
}

void UartSendStr(char *p)
{
	while (*p)
	{
			UartSend(*p++);
	}
}

void motor(u8 s)
{
//if(nFAULT){
	switch (s)
		{
			case 0x12:IN1=0;IN2=1;LED=0;
				break;
			case 0x21:IN1=0;IN2=1;LED=0;
				break;

			case 0x34:IN3=0;IN4=1;LED=0;
				break;
			case 0x43:IN3=1;IN4=0;LED=0;
				break;

			case 0x56:IN5=0;IN6=1;LED=0;
				break;
			case 0x65:IN5=1;IN6=0;LED=0;
				break;

			case 0x78:IN7=0;IN8=1;LED=0;
				break;
			case 0x87:IN7=1;IN8=0;LED=0;
				break;

			case 0x00:IN1=0;IN2=0;IN3=0;IN4=0;
								IN5=0;IN6=0;IN7=0;IN8=0;
				break;

			case 0x11:IN1=1;IN2=1;IN3=1;IN4=1;
								IN5=1;IN6=1;IN7=1;IN8=1;
								LED=0;Delay200ms();
				break;
			default:
				break;
		}
}

void main(void)
{
	u8 i=0;
	IO_Init();
	Timer4Init();
	UartInit();
	ES = 1;EA = 1;
	motor(0x00);
	while(1)
		{
			motor(SBUF);
			if(sw) 
				{
					sw=0;
					UartSend(0xcc);
				}
		}
}

void TM4_Isr() interrupt 20
{
	if(it++>Ln)
		{
			it=0;
			LED = !LED;
		}
	AUXINTIF &= ~0x04;
}

void UartIsr() interrupt 4
{
	if(TI)
		{
			TI = 0;
			busy = 0;
		}
	if(RI)
		{
			RI = 0;
			if(SBUF==0xda)
				{
					Delay100ms();
					IAP_CONTR = 0x60;
				}
			else if(SBUF==0xbb) 
				{
					sw=1;
					Ln=20;
				}
			buffer[wptr++] = SBUF;
			wptr &= 0x0f;
		}
}
