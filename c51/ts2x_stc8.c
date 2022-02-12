#include <STC8.H>
#include <intrins.h>
#include "ts2x.api.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)
typedef unsigned char u8;
typedef char s8;
typedef unsigned int u16;
typedef int s16;

sbit LED = P1^0;
bit busy;
char wptr;
char rptr;
u8 buffer[16],bf[5]={0xa0,0x00,0x11,0x22,0x0a};
u8 sk=1,tx0=0;

ts2xDebug_t dbgInfo;
twU08 sensorState;
u16 adcVal;
void Delay15ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 212;
	k = 132;
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


void Timer4Init(void)		//32毫秒@24.000MHz
{
	T4T3M &= 0xDF;		//定时器时钟12T模式
	T4L = 0x00;		//设置定时初始值
	T4H = 0x06;		//设置定时初始值
	T4T3M |= 0x80;		//定时器4开始计时
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

void UartSendS(u16 x)
{
s8 a[4];
s8 i;
	
a[0]=x/10000;
a[1]=x%10000/1000;
a[2]=x%1000/100;
a[3]=x%100/10;
	
if(a[0]!=0)
	{
		for(i=0;i<4;i++)
			{
				UartSend(a[i]+0x30);
			}
	}
	
else if(a[1]!=0)
	{
		for(i=1;i<4;i++)
			{
				UartSend(a[i]+0x30);
			}
	}
	
else if(a[2]!=0)
	{
		for(i=2;i<4;i++)
			{
				UartSend(a[i]+0x30);
			}
	}

else if(a[3]!=0)
	{
		for(i=3;i<4;i++)
			{
				UartSend(a[i]+0x30);
			}
	}

UartSend(x%10+0x30);
UartSend(0x2c);
}


u16 adcGet()
{
	u16 adcV;
	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	ADCCFG = 0x2f;                          //设置结果右对齐
																					
	adcV=ADC_RES;
	adcV<<=8;
	adcV+=ADC_RESL;
	
	return adcV;
}

void tsInit()
{
	dbgInfo.command = twCmd_StartCal; 
	dbgInfo.sensitivity = __CAL_BASE_TARGET_DEFAULT; 
	dbgInfo.command |= twCmd_SetMode; 
	dbgInfo.mode = twStateWorking; //  twStateDiagnose; 
	dbgInfo.command |= twCmd_Tune; 
	dbgInfo.posTrigger = __THRESHOLD_TRIGGER_POSITIVE;
	dbgInfo.posReleaseShort = __DERIVATIVE_RELEASE_SHORT_POS; 
	dbgInfo.posReleaseLong = __DERIVATIVE_RELEASE_LONG_POS; 
	dbgInfo.negTrigger = __THRESHOLD_TRIGGER_NEGATIVE; 
	dbgInfo.negReleaseShort = __DERIVATIVE_RELEASE_SHORT_NEG; 
	dbgInfo.negReleaseLong = __DERIVATIVE_RELEASE_LONG_NEG; 
	dbgInfo.longPressDelay = 156; 											// 5000 ms / 32 ms
}

void uartPrint()
{
	UartSendS(adcVal);
	UartSendS(dbgInfo.resVal);
	UartSendS(sensorState);
	UartSendS(dbgInfo.fwVer);
	UartSendS(dbgInfo.base);
	UartSendS(dbgInfo.admVal);
//	UartSendS(dbgInfo.sensitivity);
//	UartSendS(dbgInfo.posTrigger);
	UartSend(0x0d);UartSend(0x0a);
}

void tsTc()
{
u16 sy;
u8 tg;

if(sk)
{
	uartPrint();
}
//Delay32ms();

if(buffer[0]==bf[0]&&buffer[1]==bf[3]&&sk==0)
{
	sy=buffer[2];
	sy=sy<<8;sy+=buffer[3];
	tg=buffer[4];
	dbgInfo.command = twCmd_StartCal; 
	dbgInfo.sensitivity =sy;
	dbgInfo.command |= twCmd_Tune; 
	dbgInfo.posTrigger = (s8)tg;
	dbgInfo.posReleaseShort = __DERIVATIVE_RELEASE_SHORT_POS; 
	dbgInfo.posReleaseLong = __DERIVATIVE_RELEASE_LONG_POS; 
	dbgInfo.negTrigger = __THRESHOLD_TRIGGER_NEGATIVE; 
	dbgInfo.negReleaseShort = __DERIVATIVE_RELEASE_SHORT_NEG; 
	dbgInfo.negReleaseLong = __DERIVATIVE_RELEASE_LONG_NEG; 
	dbgInfo.longPressDelay = 156; // 5000 ms / 32 ms
	sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
	UartSendS(dbgInfo.sensitivity);
	UartSendS(dbgInfo.posTrigger);
	UartSend(0x0d);UartSend(0x0a);
}
if(SBUF==0xff&&sk==0) sk=1;
else if(SBUF==0x11&&sk==1) sk=0;
else if(SBUF==0xcc&&sk==1)
{
	sk=0;
	UartSendS(dbgInfo.sensitivity);
	UartSendS(dbgInfo.posTrigger);
	UartSend(0x0d);UartSend(0x0a);
}}



void main()
{
UartInit();
Timer4Init();
IE2=0x40;
ES = 1;
EA = 1;

P1M0 = 0x01;                  //设置P1.7>ADC,P1.0>LED
P1M1 = 0x80;

ADCCFG = 0x0f;          	    //设置ADC时钟为系统时钟/2/16/16
ADC_CONTR = 0x87;             //使能ADC模块

tsInit();
Delay100ms();
while (1)
{
	uartPrint();
	Delay15ms();
}}

void TM4_Isr() interrupt 20
{
	adcVal=adcGet();
	sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
	
	if(sensorState==1) LED=0;
	else if(sensorState==0) LED=1;
	
	AUXINTIF &= ~0x04;                          //清中断标志
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
				if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
}
