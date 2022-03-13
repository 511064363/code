#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 19200 / 4)
typedef unsigned char u8;
typedef unsigned int u16;
typedef int s16;
sbit X2=P1^6;sbit X1=P1^7;sbit Y1=P1^4;sbit Y2=P1^5;
sbit A1=P1^0;sbit A2=P1^2;sbit B1=P1^1;sbit B2=P1^3;	//????????
sbit LED=P4^7;sbit BEEP=P5^5;
sbit nFAULT=P3^2;sbit SLEEP=P4^6;sbit RST=P2^0;			//0100 0100    1000 0000
sbit M1=P4^0;sbit M2=P4^1;sbit M3=P4^2;sbit M4=P4^5;		//1110 0111
sbit K1=P2^1;sbit K2=P2^2;sbit K3=P2^3;sbit K4=P2^4;sbit K5=P2^5;	//0011 1111 
u16 CC=0x2100,C2=0x4200,cX=0x0000,cY=0x0000,tX=0x0000,tY=0x0000,tZ=0x0000,eep=0x0400;//CC=0x2400,C2=0x4800
bit xx=0,yy=0,ir=1,jj=1,busy;
u8 mm=0x00,table[10],wptr,rptr,buffer[16],*ID;
void motor(u8 t);
void encoder();
void Delay500ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 61;
	j = 225;
	k = 62;
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


void Delay5ms()		//@24.000MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 156;
	j = 213;
	do
	{
		while (--j);
	} while (--i);
}


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


void UartIsr() interrupt 4
{
    if (TI)
    {
        TI = 0;
        busy = 0;
    }
    if (RI)
    {
        RI = 0;
        if(SBUF==0xda)
        	{Delay100ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
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

void reset(bit re)
{
bit x=1,y=1,m=1,n=1,ir=0;LED=1;
	if(!re)
		{
		while(m||n){
			if(x&&X2) motor(5);
			else if(!X2) {x=0;motor(6);}
			else if(!x&&cX<CC) motor(4);
			else if(!x&&cX>=CC) {m=0;motor(6);}
			
			if(y&&Y1) motor(1);
			else if(!Y1) {y=0;motor(2);}
			else if(!y&&cY<CC) motor(0);
			else if(!y&&cY>=CC) {n=0;motor(2);}
		}
		}
			
			
			
//			if(m&&x&&X2) motor(1);
//			else if(m&&x&&!X2) {motor(2);Delay200ms();x=0;}
//			else if(m&&!x&&!X2) motor(0);
//			else if(m&&!x&&cY<CC) motor(0);
//			else if(m&&!x&&cY>=CC) {motor(2);Delay200ms();m=0;}

//			if(n&&y&&Y1) motor(5);
//			else if(n&&y&&!Y1) {motor(6);Delay200ms();y=0;}
//			else if(n&&!y&&!Y1) motor(4);
//			else if(n&&!y&&cX<CC) motor(4);
//			else if(n&&!y&&cX>=CC) {motor(6);Delay200ms();n=0;}
//		}

//		while(X2||Y1){
//						if(X2)motor(1);else motor(2);
//						if(Y1)motor(5);else motor(6);}
//		motor(8);
//		while(cX<CC||cY<CC){
//						if(cX<CC)motor(4);else motor(6);
//						if(cY<CC)motor(0);else motor(2);}
//		motor(8);}
	else if(re){
		if(cX>CC&&cY>CC){while(cX>CC||cY>CC)
										{if(cX>CC) motor(5);else motor(6);
										if(cY>CC) motor(1);else motor(2);}}
		else if(cX<CC&&cY<CC){while(cX<CC||cY<CC)
										{if(cX<CC) motor(4);else motor(6);
										if(cY<CC)motor(0);else motor(2);}}
		else if(cX>CC&&cY<CC){while(cX>CC||cY<CC)
										{if(cX>CC)motor(5);else motor(6);
										if(cY<CC)motor(0);else motor(2);}}
		else if(cX<CC&&cY>CC){while(cX<CC||cY>CC)
										{if(cX<CC)motor(4);else motor(6);
										if(cY>CC)motor(1);else motor(2);}}
		else if(cX<CC) while(cX<CC) motor(4);
		else if(cX>CC) while(cX>CC) motor(5);
		else if(cY>CC) while(cY>CC) motor(1);
		else if(cY<CC) while(cY<CC) motor(0);
}
Delay100ms();
motor(9);
}

void motor(u8 t)
{
	encoder();LED=1;
	if(nFAULT){
		switch(t)
		{
			case 0:M1=0;M2=1;break;
			case 1:M1=1;M2=0;break;
			case 2:M1=1;M2=1;break;
			case 3:M1=1;M2=1;Delay100ms();M1=0;M2=0;break;
			
			case 4:M3=0;M4=1;break;
			case 5:M3=1;M4=0;break;
			case 6:M3=1;M4=1;break;
			case 7:M3=1;M4=1;Delay100ms();M3=0;M4=0;break;
			
			case 8:M1=1;M2=1;M3=1;M4=1;Delay100ms();M1=0;M2=0;M3=0;M4=0;break;
			case 9:M1=0;M2=0;M3=0;M4=0;break;
			default:break;
			}}}

void encoder()
{
	if(X2==0) cY=0;
	if(Y1==0) cX=0;

	if(A1) yy=1;
	if(yy&&!A1) {
		yy=0;
		if(B1==1) cY--;
		else if(B1==0) cY++;}
	
	if(A2) xx=1;
	if(xx&&!A2) {
		xx=0;
		if(B2==1) cX--;
		else if(B2==0) cX++;}}

void IO_Init()
{
	BEEP=0;LED=0;
	P1M0=0x00;P1M1=0xff;	//????????
	P2M0=0x00;P2M1=0x1f;
	P3M0=0x02;P3M1=0x05;//0000 0101
	P4M0=0xe7;P4M1=0x00;	//????????
	P5M0=0x20;P5M1=0x10;//1110 0111
	M3=0;M4=0;SLEEP=0;Delay5ms();SLEEP=1;Delay5ms();	//????????????????AIN1 AIN2?????????????????????SLEEP????0-1
	M1=0;M2=0;
}

void main(void)
{
bit sw=1;u8 h=0;
IO_Init();//ID = (char idata *)0xf1;
UartInit();ES = 1;EA = 1;

//if(!RST||!K1||!K2||!K3||!K4){
//	Delay10ms();if(!RST||!K1||!K2||!K3||!K4){
//	LED=1;
//	for(h=500;h>0;h--)
//		{
//		while(X2||Y1)
//			{
//				if(X2)motor(1);else motor(2);
//				if(Y1)motor(5);else motor(6);
//			}
//		motor(8);
//		while(cX<C2||cY<C2){
//			if(cX<C2) motor(4);else motor(6);
//			if(cY<C2) motor(0);else motor(2);}
//		motor(8);
//		if(!RST) break;}}}
//reset(0);
while(1){
//	if (rptr != wptr)
//        {
//            UartSend(buffer[rptr++]);
//            rptr &= 0x0f;
//            sw=1;
//        }

//	if(sw&&SBUF==0xff){
//		sw=0;
//		for(mm=0;mm<7;mm++)	UartSend(ID[mm]);UartSend(0xdd);
//	}

//	if(sw&&SBUF==0xdd){
//		sw=0;
//		UartSend(cX>>8);UartSend(cX);UartSend(0xee);
//		UartSend(cY>>8);UartSend(cY);UartSend(0xff);
//	}

	if(!RST&&ir) {Delay10ms();if(!RST&&ir) {reset(1);ir=0;}}
	else if(!K1) {Delay10ms();while(!K1&&(cY<C2)) motor(0);motor(3);ir=1;}
	else if(!K2) {Delay10ms();while(!K2&&X2) motor(1);motor(3);ir=1;}
	else if(!K3) {Delay10ms();while(!K3&&Y1) motor(5);motor(7);ir=1;}
	else if(!K4) {Delay10ms();while(!K4&&(cX<C2)) motor(4);motor(7);ir=1;}

	while(!(K1||(cY<C2))||!(K3||Y1)) BEEP=1;
	while(!(K2||X2)||!(K4||(cX<C2))) BEEP=1;
	BEEP=0;LED=0;
}}