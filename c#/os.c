#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)
typedef unsigned char u8;
typedef unsigned int u16;

sbit IN1=P1^2;
sbit IN2=P1^3;
sbit IN3=P1^5;
sbit IN4=P1^4;
sbit IN5=P1^6;
sbit IN6=P1^7;
sbit IN7=P3^7;
sbit IN8=P5^5;
// sbit IN8=P3^6;

sbit nSLEEP=P1^1;
// sbit nSLEEP=P3^3;

sbit nFAULT=P1^0;
// sbit nFAULT=P3^2;
// sbit LED=P3^4;

bit busy;
u8 wptr,rptr,buffer[16],*ID;

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
P1M0=0xfe;P1M1=0x01;	    //1111 1110
// P1M0=0xfe;P1M1=0x01;		//1111 1110

P3M0=0x82;P3M1=0x01;	    //1000 0010
// P3M0=0xda;P3M1=0x25;		//1101 1010

P5M0=0x20;P5M1=0x10;		//0010 0000

IN1=0;IN2=0;IN5=0;IN6=0;nSLEEP=0;Delay10ms();nSLEEP=1;Delay10ms();
}

void Usart() interrupt 4
{ 
	ES = 0; 
	if (RI) {
	RI = 0; if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
	buffer[wptr++] = SBUF;
	wptr &= 0x0f;
	}
	if (TI) { TI = 0; busy = 0;} 
	ES = 1;
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

//void UartSend(char dat)
//{
//    while (busy);
//    busy = 1;
//    SBUF = dat;
//}

void motor(u8 s)
{
if(nFAULT){
	switch (s){
		case 0x12:IN1=0;IN2=1;
			break;
		case 0x21:IN1=0;IN2=1;
			break;

		case 0x34:IN3=0;IN4=1;
			break;
		case 0x43:IN3=1;IN4=0;
			break;

		case 0x56:IN5=0;IN6=1;
			break;
		case 0x65:IN5=1;IN6=0;
			break;

		case 0x78:IN7=0;IN8=1;
			break;
		case 0x87:IN7=1;IN8=0;
			break;

		// case 0xaa:
		//     {for(i=0;i<7;i++) UartSend(ID[i]);}break;

		case 0x00:IN1=0;IN2=0;IN3=0;IN4=0;IN5=0;IN6=0;IN7=0;IN8=0;
			break;

		case 0x11:
			IN1=1;IN2=1;IN3=1;IN4=1;IN5=1;IN6=1;IN7=1;IN8=1;
			Delay200ms();SBUF=0x00;
			break;
		default:
			break;
}}}

void main(void)
{
u8 i=0;
UartInit();ES = 1;EA = 1;
IO_Init();
//ID = (char idata *)0xf1;
motor(0x00);
while(1){
	motor(SBUF);
}}