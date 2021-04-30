#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 19200 / 4)
typedef unsigned char u8;
typedef unsigned int u16;

sbit nSLEEP1=P1^3;
sbit nFAULT1=P1^4;
sbit X1=P1^5;
sbit X2=P1^6;
sbit SW=P1^7;
sbit LED1=P3^7;
sbit IN9=P3^5;
sbit IN10=P3^4;

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
P1M0=0x0a;P1M1=0xf5;	    //0000 1010

P3M0=0xbd;P3M1=0x41;	    //1011 1110
// P3M0=0xda;P3M1=0x25;		//1101 1010

IN9=0;IN10=0;nSLEEP1=0;Delay10ms();
nSLEEP1=1;Delay10ms();
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
if(nFAULT1){
	switch (s){
		case 0x12:IN9=0;IN10=1;
			break;
		case 0x21:IN9=0;IN10=1;
			break;

		case 0x00:IN9=0;IN10=0;
			break;

		// case 0xaa:
		//     {for(i=0;i<7;i++) UartSend(ID[i]);}break;

		case 0x11:IN9=1;IN10=1;Delay200ms();IN9=0;IN10=0;LED1=0;
			break;
		default:
			break;
}}}

void main(void)
{
u8 i=0;
UartInit();
ES = 1;EA = 1;
IO_Init();
//ID = (char idata *)0xf1;
motor(0x00);
while (X2) motor(0x12);
motor(0x11);
while(1){
	if(!SW){
		Delay10ms();
		if(!SW) {
			motor(0x12);Delay200ms();
			while (X1&&X2) motor(0x12);
			motor(0x11);
}}
motor(0x00);
if(!X1) LED1=1;
else LED1=0;

}}