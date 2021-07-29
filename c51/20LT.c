#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 19200 / 4)
typedef unsigned char u8;
typedef unsigned int u16;
sbit K1=P1^2;
sbit K2=P1^3;
sbit K3=P1^4;
sbit K4=P1^5;
sbit K5=P1^6;
sbit K6=P1^7;
sbit K7=P5^5;
sbit K8=P3^1;
sbit K9=P3^2;
sbit K10=P3^3;
sbit K11=P3^4;
sbit K12=P3^5;
sbit K13=P3^6;
sbit K14=P3^7;
sbit K15=P1^0;
sbit K16=P1^1;
sbit LED=P5^4;
u8 m,wptr,rptr,buffer[16];
u16 n=0;
bit busy;
u8 t=0;
void Delay100ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay20000ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 132;
	j = 11;
	k = 72;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void IoInit()
{
	P1M1=0x00;P1M0=0xff;//
	P3M1=0x01;P3M0=0xfe;//1111 1110
	P5M1=0x00;P5M0=0x30;//0011 0000	
	K1=1;K2=1;K3=1;K4=1;K5=1;K6=1;K7=1;K8=1;
	K9=1;K10=1;K11=1;K12=1;K13=1;K14=1;K15=1;K16=1;
}

void UartInit()
{
	 SCON = 0x50;
	 T2L = BRT;
	 T2H = BRT >> 8;
	 AUXR |= 0x15;
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

void Timer0Init(void)		//30o?¡ì?¨¦??@24.000MHz
{
	AUXR &= 0x7F;		//???¨¬?¡ìo?¡§¡è????¡ìo?¡§¡è??¡ì?12T??¡§o?¡ìo?
	TMOD &= 0xF0;		//?¡ì|?¡ì?¡ì?????¨¬?¡ìo?¡§¡è?????¡§o?¡ìo?
	TL0 = 0xA0;		//?¡ì|?¡ì?¡ì?????¨¬?¡ìo?¡§¡è3??¡ìo??|¡§?
	TH0 = 0x15;		//?¡ì|?¡ì?¡ì?????¨¬?¡ìo?¡§¡è3??¡ìo??|¡§?
	TF0 = 0;		//??3yTF0?¡§¡è?¡ìo??
	TR0 = 1;		//???¨¬?¡ìo?¡§¡è???0?a?¡ìo????¡ìo?¡§¡è
}

void main()
{
IoInit();
Timer0Init();
UartInit();
ES = 1;
ET0 = 1;
EA = 1;
Delay20000ms();	
WDT_CONTR = 0x24;
while(1)
	{
		WDT_CONTR |= 0x10;
		  
	  if(t==0)
		{
			K1=0;K2=1;K3=1;K4=1;
			K5=0;K6=1;K7=1;K8=1;
			K9=0;K10=1;K11=1;K12=1;
			K13=0;K14=1;K15=1;K16=1;
		}
	  if(t==1)
		{
			K1=1;K2=0;K3=1;K4=1;
			K5=1;K6=0;K7=1;K8=1;
			K9=1;K10=0;K11=1;K12=1;
			K13=1;K14=0;K15=1;K16=1;
		}
		if(t==2)
		{
			K1=1;K2=1;K3=0;K4=1;
			K5=1;K6=1;K7=0;K8=1;
			K9=1;K10=1;K11=0;K12=1;
			K13=1;K14=1;K15=0;K16=1;
		}
		if(t==3)
		{
			K1=1;K2=1;K3=1;K4=0;
			K5=1;K6=1;K7=1;K8=0;
			K9=1;K10=1;K11=1;K12=0;
			K13=1;K14=1;K15=1;K16=0;
		}
	}
}

void Usart() interrupt 4
{ 
	if (RI) 
		{
			RI = 0; 
			if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
			buffer[wptr++] = SBUF;
			wptr &= 0x0f;
		}
	if (TI) {TI = 0; busy = 0;} 
	ES = 1;
}

void TM0_Isr() interrupt 1
{
	m++;
	if(m==30)
		{
			LED = ~LED;
			m=0;n++;
		}
	if(n==20)
		{
			n=0;
			t++;
		}
	if(t==4) t=0;
}