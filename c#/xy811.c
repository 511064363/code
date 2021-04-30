#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 19200 / 4)
#define WT_30M 0x80
#define WT_24M 0x81
#define WT_20M 0x82
#define WT_12M 0x83
#define WT_6M 0x84
#define WT_3M 0x85
#define WT_2M 0x86
#define WT_1M 0x87
#define ENLVR           0x40                    //RSTCFG.6                 //LVD@2.7V
#define LVD3V0          0x03                    //LVD@3.0V
#define LVDF            0x20

typedef unsigned char u8;
typedef unsigned int u16;
sbit X2=P1^6;sbit X1=P1^7;sbit Y1=P1^4;sbit Y2=P1^5;
sbit A1=P1^0;sbit A2=P1^2;sbit B1=P1^1;sbit B2=P1^3;	//????????
sbit LED=P4^7;sbit BEEP=P5^5;
sbit nFAULT=P3^2;sbit SLEEP=P4^6;sbit RST=P2^0;			//0100 0100    1000 0000
sbit M1=P4^0;sbit M2=P4^1;sbit M3=P4^2;sbit M4=P4^5;		//1110 0111
sbit K1=P2^1;sbit K2=P2^2;sbit K3=P2^3;sbit K4=P2^4;sbit K5=P2^5;	//0011 1111 
u16 CC=0x2800,C2=0x5000,cX=0x0000,cY=0x0000,tX=0x0000,tY=0x0000,tZ=0x0000,eep=0x0400;
bit xx=0,yy=0,ir=1,jj=1,busy,one=1;
u8 mm=0x00,table[]={0x00,0x00,0x00,0x00,0x00,0x00},wptr,rptr,buffer[16],*ID;
void motor(u8 t);
void encoder();

void Delay200ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 19;
	j = 62;
	k = 43;
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

void Delay5ms()		//@24.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 234;
	k = 113;
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
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void IapIdle()
{
	IAP_CONTR = 0; //?? IAP ??
	IAP_CMD = 0; //???????
	IAP_TRIG = 0; //???????
	IAP_ADDRH = 0x80; //??????? IAP ??
	IAP_ADDRL = 0;
}

char IapRead(int addr)
{
	char dat;
	IAP_CONTR = WT_12M; //?? IAP
	IAP_CMD = 1; //?? IAP ???
	IAP_ADDRL = addr; //?? IAP ???
	IAP_ADDRH = addr >> 8; //?? IAP ???
	IAP_TRIG = 0x5a; //?????(0x5a)
	IAP_TRIG = 0xa5; //?????(0xa5)
	_nop_();
	dat = IAP_DATA; //? IAP ??
	IapIdle(); //?? IAP ??
	return dat;
}

void IapProgram(int addr,char dat)
{
	IAP_CONTR = WT_12M; //?? IAP
	IAP_CMD = 2; //?? IAP ???
	IAP_ADDRL = addr; //?? IAP ???
	IAP_ADDRH = addr >> 8; //?? IAP ???
	IAP_DATA = dat; //? IAP ??
	IAP_TRIG = 0x5a; //?????(0x5a)
	IAP_TRIG = 0xa5; //?????(0xa5)
	_nop_();
	IapIdle(); //?? IAP ??
}

void IapErase(int addr)
{
	IAP_CONTR = WT_12M; //?? IAP
	IAP_CMD = 3; //?? IAP ????
	IAP_ADDRL = addr; //?? IAP ???
	IAP_ADDRH = addr >> 8; //?? IAP ???
	IAP_TRIG = 0x5a; //?????(0x5a)
	IAP_TRIG = 0xa5; //?????(0xa5)
	_nop_(); //
	IapIdle(); //?? IAP ??
}

void Usart() interrupt 4 using 0 
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

void UartSend(char dat)
{
    while (busy);
    busy = 1;
    SBUF = dat;
}

void Lvd_Isr() interrupt 6
{
	PCON &= ~LVDF; 
    if(one){
	table[0]=tX;table[1]=tX>>8;
	table[2]=tY;table[3]=tY>>8;
	table[4]=tZ;table[5]=tZ>>8;
	// table[6]=cX;table[7]=cX>>8;
	// table[8]=cY;table[9]=cY>>8;
	for(mm=0;mm<6;mm++){IapProgram(eep+mm,table[mm]);_nop_();}
	one=0;
}}

void reset(bit re)
{
bit x=1,y=1,m=1,n=1,ir=0;LED=1;
	if(!re){
		while(X2||Y1){
						if(X2)motor(1);else motor(2);
						if(Y1)motor(5);else motor(6);}
		motor(8);
		while(cX<CC||cY<CC){
						if(cX<CC)motor(4);else motor(6);
						if(cY<CC)motor(0);else motor(2);}
		motor(8);}
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
		motor(8);
}}

void motor(u8 t)
{
	encoder();LED=1;
	if(nFAULT){
		switch(t)
		{
			case 0:M1=0;M2=1;break;
			case 1:M1=1;M2=0;break;
			case 2:M1=1;M2=1;break;
			case 3:M1=1;M2=1;Delay200ms();M1=0;M2=0;Delay50ms();break;
			
			case 4:M3=0;M4=1;break;
			case 5:M3=1;M4=0;break;
			case 6:M3=1;M4=1;break;
			case 7:M3=1;M4=1;Delay200ms();M3=0;M4=0;Delay50ms();break;
			
			case 8:M1=1;M2=1;M3=1;M4=1;Delay200ms();M1=0;M2=0;M3=0;M4=0;Delay50ms();break;
			case 9:M1=0;M2=0;M3=0;M4=0;break;
			default:break;
			}}}

void encoder()
{
	bit xx2=0,yy1=0;
	if(X2==0) {cY=0;xx2=1;}
	if(Y1==0) {cX=0;yy1=1;}

	if(X2&&xx2) {tX++;xx2=0;}
	if(Y1&&yy1) {tY++;yy1=0;}

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
	P2M0=0x00;P2M1=0x3f;
	P3M0=0x80;P3M1=0x44;
	P4M0=0xe7;P4M1=0x00;	//????????
	P5M0=0x20;P5M1=0x00;
	M3=0;M4=0;SLEEP=0;Delay5ms();SLEEP=1;Delay5ms();	//????????????????AIN1 AIN2?????????????????????SLEEP????0-1
	M1=0;M2=0;
}

void main(void)
{
bit sw=1;u8 h=0;
IO_Init();ID = (char idata *)0xf1;
UartInit();ES = 1;EA = 1;

for(mm=0;mm<6;mm++){table[mm]=IapRead(eep+mm);}
IapErase(eep);
tX=table[1];tX<<=8;tX+=table[0];
tY=table[3];tY<<=8;tY+=table[2];
tZ=table[5];tZ<<=8;tZ+=table[4];
// cX=table[7];cX<<=8;cX+=table[6];
// cY=table[9];cY<<=8;cY+=table[8];
tZ++;
if(!RST||!K1||!K2||!K3||!K4){
	Delay10ms();if(!RST||!K1||!K2||!K3||!K4){
	LED=1;
	for(h=1000;h>0;h--){
		while(X2||Y1){
			if(X2)motor(1);else motor(2);
			if(Y1)motor(5);else motor(6);}
		motor(8);
		while(cX<C2||cY<C2){
			if(cX<C2) motor(4);else motor(6);
			if(cY<C2) motor(0);else motor(2);}
		motor(8);}}}
reset(0);
while(1){
RSTCFG = LVD3V0;ELVD = 1;

if(SBUF==0xff){
	UartSend(tX>>8);UartSend(tX);UartSend(0xaa);
	UartSend(tY>>8);UartSend(tY);UartSend(0xbb);
	UartSend(tZ>>8);UartSend(tZ);UartSend(0xcc);
	for(mm=0;mm<7;mm++)	UartSend(ID[mm]);UartSend(0xdd);
}

if(!RST&&ir) {Delay10ms();if(!RST&&ir) {reset(1);ir=0;}}
else if(!K1) {Delay10ms();if(!K1){while(!K1&&(cY<C2)) motor(0);motor(3);ir=1;}}
else if(!K2) {Delay10ms();if(!K2){while(!K2&&X2) motor(1);motor(3);ir=1;}}
else if(!K3) {Delay10ms();if(!K3){while(!K3&&Y1) motor(5);motor(7);ir=1;}}
else if(!K4) {Delay10ms();if(!K4){while(!K4&&(cX<C2)) motor(4);motor(7);ir=1;}}

while(!(K1||(cY<C2))||!(K3||Y1)) BEEP=1;
while(!(K2||X2)||!(K4||(cX<C2))) BEEP=1;
BEEP=0;LED=0;
}}