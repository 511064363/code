#include <STC8.H>
#include <intrins.h>
#include "ts2x.api.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)
typedef unsigned char u8;
typedef unsigned int u16;

sbit LED = P5^4;
bit busy;
char wptr;
char rptr;
char buffer[16];
u8 hc[3]={0xab,0xcd,0xef};



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

void main()
{
    UartInit();
    ES = 1;
    EA = 1;
	
		P1M0 = 0x00;                                //设置P1.7为ADC口
    P1M1 = 0x80;
		P5M0|=0x10;
		P5MI1|=0x00;
    ADCCFG = 0x0f;                              //设置ADC时钟为系统时钟/2/16/16
    ADC_CONTR = 0x87;                           //使能ADC模块
		
dbgInfo.command = twCmd_StartCal; 
dbgInfo.sensitivity = 80; 
dbgInfo.command |= twCmd_SetMode; 
dbgInfo.mode = twStateWorking; // */ twStateDiagnose; 
dbgInfo.command |= twCmd_Tune; 
dbgInfo.posTrigger = 50;
dbgInfo.posReleaseShort = 0; 
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
ADCCFG = 0x00;                              //设置结果左对齐
                             //A存储ADC的12位结果的高8位
                            //B[7:4]存储ADC的12位结果的低4位,B[3:0]为0
adcVal=(int)ADC_RES<<8;
adcVal|=(int)ADC_RESL;
adcVal=adcVal>>4;
sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
if(sensorState==1)
	LED=0;
else LED=1;

UartSend(adcVal/10000+0x30);
UartSend(adcVal%10000/1000+0x30);
UartSend(adcVal%1000/100+0x30);
UartSend(adcVal%100/10+0x30);
UartSend(adcVal%10+0x30);
UartSend(0x2c);
UartSend(dbgInfo.resVal/10000+0x30);
UartSend(dbgInfo.resVal%10000/1000+0x30);
UartSend(dbgInfo.resVal%1000/100+0x30);
UartSend(dbgInfo.resVal%100/10+0x30);
UartSend(dbgInfo.resVal%10+0x30);
UartSend(0x2c);
UartSend(sensorState+0x30);
UartSend(0x2c);
UartSend(dbgInfo.fwVer/1000+0x30);
UartSend(dbgInfo.fwVer%100/10+0x30);
UartSend(dbgInfo.fwVer%10+0x30);
UartSend(0x2c);
UartSend(dbgInfo.base/10000+0x30);
UartSend(dbgInfo.base%10000/1000+0x30);
UartSend(dbgInfo.base%1000/100+0x30);
UartSend(dbgInfo.base%100/10+0x30);
UartSend(dbgInfo.base%10+0x30);
UartSend(0x2c);
UartSend(dbgInfo.admVal/10000+0x30);
UartSend(dbgInfo.admVal%10000/1000+0x30);
UartSend(dbgInfo.admVal%1000/100+0x30);
UartSend(dbgInfo.admVal%100/10+0x30);
UartSend(dbgInfo.admVal%10+0x30);

UartSend(0x0d);
UartSend(0x0a);

Delay32ms();

}
}
