#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 19200 / 4)

// #define WT_30M 0x80
// #define WT_24M 0x81
// #define WT_20M 0x82
// #define WT_12M 0x83
// #define WT_6M 0x84
// #define WT_3M 0x85
// #define WT_2M 0x86
// #define WT_1M 0x87
// #define ENLVR 0x40
// #define LVD3V0 0x03
// #define LVDF 0x20
typedef unsigned char u8;
typedef unsigned int u16;

sbit nSLEEP1=P1^3;
sbit nFAULT1=P1^4;
sbit X1=P1^5;
sbit X2=P1^6;
sbit SW=P1^7;
sbit LED1=P3^7;
sbit IN9=P3^2;
sbit IN10=P3^3;

bit busy;
u8 wptr,rptr,buffer[16],*ID;
void Delay3000ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 110;
	j = 66;
	k = 136;
	do
	{
		do
		{
			while (--k);
		} while (--j);
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

void Delay500ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 46;
	j = 153;
	k = 245;
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

void Delay50ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 7;
	j = 23;
	k = 105;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay5000ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 97;
	j = 195;
	k = 144;
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
P3M0=0xbd;P3M1=0x41;	    //1011 0010
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
		case 0x21:IN9=1;IN10=0;
			break;

		case 0x00:IN9=0;IN10=0;
			break;

		// case 0xaa:
		//     {for(i=0;i<7;i++) UartSend(ID[i]);}break;

		case 0x11:IN9=1;IN10=1;Delay200ms();IN9=0;IN10=0;
			break;
		default:
			break;
}}}

// void Lvd_Isr() interrupt 6
// {
// 	PCON &= ~LVDF; 
// 	motor(21);LED1=1;
// }

void main(void)
{
u8 i=0;
UartInit();
ES = 1;EA = 1;
IO_Init();
ID = (char idata *)0xf1;
motor(0x00);
while(1){
// RSTCFG = LVD3V0;ELVD = 1;
	if(!SW){
		Delay10ms();
		if(!SW) {
			LED1=1;
			motor(0x12);Delay50ms();
			motor(0x11);while (!SW);
}}
if(!X2){
	Delay10ms();
	if(!X2){
		LED1=1;
		motor(0x21);Delay3000ms();
		while (!X2);
	}

}

motor(0x00);LED1=0;
}}