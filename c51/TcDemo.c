#include <STC8.H>
#include <intrins.h>
#include "ts2x.api.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)
typedef unsigned char u8;
typedef char s8;
typedef unsigned int u16;
typedef int s16;

sbit LED = P5^4;
bit busy;
char wptr;
char rptr;
u8 buffer[16],bf[5]={0xa0,0x00,0x11,0x22,0x0a};

ts2xDebug_t dbgInfo;
twU08 sensorState;
twS16 adcVal;

void Delay32ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 4;
	j = 230;
	k = 97;
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

void main()
{
	u16 sy;
	u8 tg,sk=1;
    UartInit();
    ES = 1;
    EA = 1;
	
		P1M0 = 0x00;                                //设置P1.7为ADC口
    P1M1 = 0x80;
		P5M0 = 0X10;																//设置P5.4为强推挽输出口
		P5M1 = 0X00;
	
    ADCCFG = 0x0f;                              //设置ADC时钟为系统时钟/2/16/16
    ADC_CONTR = 0x87;                           //使能ADC模块
		
dbgInfo.command = twCmd_StartCal; 
dbgInfo.sensitivity = 255; 
dbgInfo.command |= twCmd_SetMode; 
dbgInfo.mode = twStateWorking; //  twStateDiagnose; 
dbgInfo.command |= twCmd_Tune; 
dbgInfo.posTrigger = __THRESHOLD_TRIGGER_POSITIVE;
dbgInfo.posReleaseShort = __DERIVATIVE_RELEASE_SHORT_POS; 
dbgInfo.posReleaseLong = __DERIVATIVE_RELEASE_LONG_POS; 
dbgInfo.negTrigger = __THRESHOLD_TRIGGER_NEGATIVE; 
dbgInfo.negReleaseShort = __DERIVATIVE_RELEASE_SHORT_NEG; 
dbgInfo.negReleaseLong = __DERIVATIVE_RELEASE_LONG_NEG; 
dbgInfo.longPressDelay = 156; // 5000 ms / 32 ms
	
while (1)
{
			
ADC_CONTR |= 0x40;                      //启动AD转换
_nop_();
_nop_();
while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
ADC_CONTR &= ~0x20;                     //清完成标志
ADCCFG = 0x00;                          //设置结果左对齐
																				//A存储ADC的12位结果的高8位
																				//B[7:4]存储ADC的12位结果的低4位,B[3:0]为0
adcVal=ADC_RES<<8;
adcVal|=ADC_RESL;
adcVal=adcVal>>4;
sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
	
if(sensorState==1) LED=0;
else if(sensorState==0) LED=1;
	
if(sk)
{
UartSendS(adcVal);
UartSendS(dbgInfo.resVal);
UartSendS(sensorState);
UartSendS(dbgInfo.fwVer);
UartSendS(dbgInfo.base);
UartSendS(dbgInfo.admVal);
UartSend(0x0d);UartSend(0x0a);
}
Delay32ms();
//if(buffer[0]==bf[0]&&buffer[1]==bf[3]&&sk==0)
//{
//			sy=buffer[2];
//			sy=sy<<8;sy+=buffer[3];
//			tg=buffer[4];
//			dbgInfo.command = twCmd_StartCal; 
//			dbgInfo.sensitivity =sy;
//			dbgInfo.command |= twCmd_Tune; 
//			dbgInfo.posTrigger = (s8)tg;
//			dbgInfo.posReleaseShort = __DERIVATIVE_RELEASE_SHORT_POS; 
//			dbgInfo.posReleaseLong = __DERIVATIVE_RELEASE_LONG_POS; 
//			dbgInfo.negTrigger = __THRESHOLD_TRIGGER_NEGATIVE; 
//			dbgInfo.negReleaseShort = __DERIVATIVE_RELEASE_SHORT_NEG; 
//			dbgInfo.negReleaseLong = __DERIVATIVE_RELEASE_LONG_NEG; 
//			dbgInfo.longPressDelay = 156; // 5000 ms / 32 ms
//			sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
//			UartSendS(dbgInfo.sensitivity);
//			UartSendS(dbgInfo.posTrigger);
//			UartSend(0x0d);UartSend(0x0a);
//}
//else if(SBUF==0xff&&sk==0) sk=1;
//else if(SBUF==0x11&&sk==1) sk=0;
//else if(SBUF==0xcc&&sk==1)
//{
//sk=0;
//UartSendS(dbgInfo.sensitivity);
//UartSendS(dbgInfo.posTrigger);
//UartSend(0x0d);UartSend(0x0a);
//}


//			sk=0;
//			UartSendS(sy);UartSendS(tg);
//			UartSend(0x0d);UartSend(0x0a);
//		}
//}

}}
