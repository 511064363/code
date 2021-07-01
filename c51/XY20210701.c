#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)
#define WT_30M          0x80
#define WT_24M          0x81
#define WT_20M          0x82
#define WT_12M          0x83
#define WT_6M           0x84
#define WT_3M           0x85
#define WT_2M           0x86
#define WT_1M           0x87
#define ENLVR           0x40                    //RSTCFG.6
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
u16 CC=0x2400,C2=0x4800,cX=0x0000,cY=0x0000;
bit xx=0,yy=0,ir=1,jj=1,busy,one=1;
u8 mm=0x00,table[10],wptr,rptr,buffer[16],*ID;
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

void UartSend(char dat)
{
    while (busy);
    busy = 1;
    SBUF = dat;
}
void IapIdle()
{
    IAP_CONTR = 0;                              //关闭IAP功能
    IAP_CMD = 0;                                //清除命令寄存器
    IAP_TRIG = 0;                               //清除触发寄存器
    IAP_ADDRH = 0x80;                           //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

char IapRead(int addr)
{
    char dat;

    IAP_CONTR = WT_12M;                         //使能IAP
    IAP_CMD = 1;                                //设置IAP读命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
    dat = IAP_DATA;                             //读IAP数据
    IapIdle();                                  //关闭IAP功能

    return dat;
}

void IapProgram(int addr, char dat)
{
    IAP_CONTR = WT_12M;                         //使能IAP
    IAP_CMD = 2;                                //设置IAP写命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_DATA = dat;                             //写IAP数据
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
    IapIdle();                                  //关闭IAP功能
}

void IapErase(int addr)
{
    IAP_CONTR = WT_12M;                         //使能IAP
    IAP_CMD = 3;                                //设置IAP擦除命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();                                    //
    IapIdle();                                  //关闭IAP功能
}

void reset(bit re)
{
bit x=1,y=1,m=1,n=1;
	if(!re)
		{
			while(m||n)
			{
				if(x&&X2) motor(5);
				else if(!X2) {x=0;motor(6);}
				else if(!x&&cX<CC) motor(4);
				else if(!x&&cX>=CC) {m=0;motor(6);}
				
				if(y&&Y1) motor(1);
				else if(!Y1) {y=0;motor(2);}
				else if(!y&&cY<CC) motor(0);
				else if(!y&&cY>=CC) {n=0;motor(2);}
			}
			motor(8);
		
//		while(X2||Y1){
//						if(X2)motor(1);else motor(2);
//						if(Y1)motor(5);else motor(6);}
//		motor(8);
//		while(cX<CC||cY<CC){
//						if(cX<CC)motor(4);else motor(6);
//						if(cY<CC)motor(0);else motor(2);}
		}
	else if(re)
		{
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
		}
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
			case 3:M1=1;M2=1;Delay200ms();M1=0;M2=0;Delay200ms();break;
			
			case 4:M3=0;M4=1;break;
			case 5:M3=1;M4=0;break;
			case 6:M3=1;M4=1;break;
			case 7:M3=1;M4=1;Delay200ms();M3=0;M4=0;Delay200ms();break;
			
			case 8:M1=1;M2=1;M3=1;M4=1;Delay200ms();M1=0;M2=0;M3=0;M4=0;Delay200ms();break;
			case 9:M1=0;M2=0;M3=0;M4=0;break;
			default:break;
		}}
}

void encoder()
{	
	bit xx2=0,yy1=0;
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
		else if(B2==0) cX++;}
}

void IO_Init()
{
	BEEP=0;
	P1M0=0x00;P1M1=0xff;	//????????
	P2M0=0x00;P2M1=0x3f;
	P3M0=0x80;P3M1=0x44;
	P4M0=0xe7;P4M1=0x00;	//????????
	P5M0=0x20;P5M1=0x00;
	M3=0;M4=0;SLEEP=0;Delay5ms();SLEEP=1;Delay5ms();	//????????????????AIN1 AIN2?????????????????????SLEEP????0-1
	M1=0;M2=0;
}

void Timer0Init(void)		//30毫秒@24.000MHz
{
	AUXR |= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xA0;		//设置定时初始值
	TH0 = 0x15;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void main(void)
{
u16 h=0;
IO_Init();Timer0Init();
ID = (char idata *)0xf1;
UartInit();
PCON &= ~LVDF;RSTCFG = LVD3V0;ELVD = 1;
ET0 = 1;
ES = 1;EA = 1;
cX=IapRead(0x0401);cX<<=8;cX+=IapRead(0x0400);
cY=IapRead(0x0403);cY<<=8;cY+=IapRead(0x0402);

reset(1);
while(1)
	{
		if(SBUF==0xff)
			{
				SBUF=0x00;
				for(mm=0;mm<7;mm++)
				UartSend(ID[mm]);UartSend(0xaa);UartSend(0xaa);
				UartSend(cX>>8);UartSend(cX);UartSend(0xbb);UartSend(0xbb);
				UartSend(cY>>8);UartSend(cY);UartSend(0xcc);UartSend(0xcc);
			}

		if(!RST&&ir) {Delay10ms();if(!RST&&ir) {reset(1);ir=0;}}
		else if(!K1) {Delay10ms();if(!K1){while(!K1&&(cY<C2)) motor(0);motor(3);ir=1;}}
		else if(!K2) {Delay10ms();if(!K2){while(!K2&&X2) motor(1);motor(3);ir=1;}}
		else if(!K3) {Delay10ms();if(!K3){while(!K3&&Y1) motor(5);motor(7);ir=1;}}
		else if(!K4) {Delay10ms();if(!K4){while(!K4&&(cX<C2)) motor(4);motor(7);ir=1;}}

		while(!(K1||(cY<C2))||!(K3||Y1)) BEEP=1;
		while(!(K2||X2)||!(K4||(cX<C2))) BEEP=1;
		BEEP=0;
	}
}

void TM0_Isr() interrupt 1
{
    LED = !LED;                                 //测试端口
}

void Lvd_Isr() interrupt 6
{
	bit LL=1;
   PCON &= ~LVDF;        //清中断标志
	if(LL)
	{
	 LL=0;
    IapErase(0x0400);
	 IapProgram(0x0400,cX);IapProgram(0x0401,cX>>8);
	 IapProgram(0x0402,cY);IapProgram(0x0403,cY>>8);
	}
}