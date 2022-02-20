#include <STC8.H>
#include <intrins.h>
#include "ts2x.api.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)
#define CLK_H  CLK = 1
#define CLK_L  CLK = 0
#define DIO_H  DIO = 1
#define DIO_L  DIO = 0
typedef unsigned char u8;
typedef char s8;
typedef unsigned int u16;
typedef int s16;

sbit LED = P1^0;
sbit CLK = P3^2;
sbit DIO = P3^3;

sbit KA	 = P7^0;
sbit KB	 = P4^1;
sbit KC	 = P3^7;
sbit KD	 = P3^6;
sbit KE	 = P5^1;


bit busy;
char wptr;
char rptr;
u8 buffer[16],Bf[8];
u8 code CO[17] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00}; //0~F??????o???????2??

u8 sk=1,j=0;
u8 Dat,Fg;
twU16 Sy;
twU08 Tg;
ts2xDebug_t dbgInfo;
twU08 sensorState;
u16 adcVal;
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

void Timer4Init(void)		//32??@24.000MHz
{
	T4T3M &= 0xDF;		//?????12T??
	T4L = 0x00;		//???????
	T4H = 0x06;		//???????
	T4T3M |= 0x80;		//???4????
	IE2 |= 0x40;
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
	ADC_CONTR |= 0x40;                      //??AD??
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //??ADC????
	ADC_CONTR &= ~0x20;                     //?????
	ADCCFG = 0x2f;                          //???????
																					
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
void tc(twU16 sx,twS08 tx)
{
	dbgInfo.command = twCmd_StartCal; 
	dbgInfo.sensitivity = sx; 
	dbgInfo.command |= twCmd_SetMode; 
	dbgInfo.mode = twStateWorking; //  twStateDiagnose; 
	dbgInfo.command |= twCmd_Tune; 
	dbgInfo.posTrigger = tx;
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
	UartSendS(dbgInfo.sensitivity);
	UartSendS(dbgInfo.posTrigger);
	UartSend(0x0d);UartSend(0x0a);
}


void Delay_us(u16 i) //us??????????o???????
{
	for(;i>0;i--)
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}

void I2CStart(void)//?a????o?D?o?
{
	CLK_H;
	DIO_H;
	Delay_us(5);
	DIO_L;
}

void I2Cask(void) //ACKD?o?
{
	u8 timeout = 1;
	CLK_H;
	Delay_us(5);
	CLK_L;
	while((DIO)&&(timeout<=100))
	{
		timeout++;
	}
	Delay_us(5);
	CLK_L;
}

void I2CStop(void) //????a????o?1D?o?
{
	CLK_H;
	DIO_L;
	Delay_us(5);
	DIO_H;
}

void I2CWrByte(u8 oneByte) //D????????????????????????2?????????2???????o?|????????a???????2o?????
{
	u8 i;
	CLK_L;
	Delay_us(1);
	for(i=0;i<8;i++)
	{
		oneByte = oneByte<<1;
		DIO = CY;
		CLK_L;
		Delay_us(5);
		CLK_H;
		Delay_us(5);
		CLK_L;
	}
}

void TM1650_Set(u8 add,u8 dat) //????oy??1????1??????o?
{
	I2CStart();
	I2CWrByte(add); //|????????2??????????????|????????????
	I2Cask();
	I2CWrByte(dat);
	I2Cask();
	I2CStop();
}
//void tsTc()
//{
//u16 sy;
//s8 tg;

//if(sk)
//{
//	uartPrint();
//}

//if(buffer[0]==bf[0]&&buffer[1]==bf[3]&&sk==0)
//{
//	sy=buffer[2];
//	sy=sy<<8;sy+=buffer[3];
//	tg=buffer[4];
//	dbgInfo.command = twCmd_StartCal; 
//	dbgInfo.sensitivity =sy;
//	dbgInfo.command |= twCmd_Tune; 
//	dbgInfo.posTrigger = tg;
//	dbgInfo.posReleaseShort = __DERIVATIVE_RELEASE_SHORT_POS; 
//	dbgInfo.posReleaseLong = __DERIVATIVE_RELEASE_LONG_POS; 
//	dbgInfo.negTrigger = __THRESHOLD_TRIGGER_NEGATIVE; 
//	dbgInfo.negReleaseShort = __DERIVATIVE_RELEASE_SHORT_NEG; 
//	dbgInfo.negReleaseLong = __DERIVATIVE_RELEASE_LONG_NEG; 
//	dbgInfo.longPressDelay = 156; // 5000 ms / 32 ms
//	sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
//	UartSendS(dbgInfo.sensitivity);
//	UartSendS(dbgInfo.posTrigger);
//	UartSend(0x0d);UartSend(0x0a);
//}
//if(SBUF==0xff&&sk==0) sk=1;
//else if(SBUF==0x11&&sk==1) sk=0;
//else if(SBUF==0xcc&&sk==1)
//{
//	sk=0;
//	UartSendS(dbgInfo.sensitivity);
//	UartSendS(dbgInfo.posTrigger);
//	UartSend(0x0d);UartSend(0x0a);
//}}

void main()
{
UartInit();
//Timer3Init();
Timer4Init();

ES = 1;
EA = 1;

P1M0 = 0x01;                  //??P1.7>ADC,P1.0>LED
P1M1 = 0x80;
P3M0 = 0x0C;									//1100 1100
P3M1 = 0xC0;
P4M0 = 0x00;
P4M1 = 0x02;
P5M0 = 0x00;
P5M1 = 0x02;
P7M0 = 0x00;
P7M1 = 0x01;
	
ADCCFG = 0x0f;          	    //??ADC???????/2/16/16
ADC_CONTR = 0x87;             //??ADC??

tsInit();
	
TM1650_Set(0x48,0x51);

while (1)
{
	uartPrint();
	Delay32ms();
}}
//void TM3_Isr() interrupt 19
//{
//	u8 j,a[4];

//	a[0]=adcVal%10000/1000;
//	a[1]=adcVal%1000/100;
//	a[2]=adcVal%100/10;
//	a[3]=adcVal%10;
//	
//	if(j++>3)
//		{
//			TM1650_Set(0x6C,CO[a[0]]);
//			TM1650_Set(0x6E,CO[a[1]]);
//			TM1650_Set(0x68,CO[a[2]]);
//			TM1650_Set(0x6A,CO[a[3]]);			
//			j=0;
//		}
//	
//  AUXINTIF &= ~0x02;                          //?????
//}
void TM4_Isr() interrupt 20
{
	u8 b[4];
	adcVal=adcGet();
	sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);
	
	if(sensorState==1) LED=0;
	else if(sensorState==0) LED=1;
	
	if(j++>3)
	{
		b[0]=adcVal%10000/1000;
		b[1]=adcVal%1000/100;
		b[2]=adcVal%100/10;
		b[3]=adcVal%10;
		
		TM1650_Set(0x6C,CO[b[0]]);
		TM1650_Set(0x6E,CO[b[1]]);
		TM1650_Set(0x68,CO[b[2]]);
		TM1650_Set(0x6A,CO[b[3]]);
		
		j=0;
	}

	AUXINTIF &= ~0x04;                          //?????
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
				Dat=SBUF;
        RI = 0;
//				if(Fg==0&&Dat==0x5A){Bf[Fg]=Dat;Fg++;Dat=0;}
//				else if(Fg==1&&Dat==0xA5){Bf[Fg]=Dat;Fg++;Dat=0;}
//				else if(Fg>1&&Fg<5){Bf[Fg]=Dat;Fg++;Dat=0;}
//				else if(Fg==5&&Dat==0xab)
//					{
//						Fg=0;
//						Sy=Bf[2];Sy<<=8;Sy+=Bf[3];Tg=Bf[4];
//						tc(Sy,Tg);
//					}
//				else Fg=0;
				if(Fg==0&&Dat=='#') {Fg++;Dat=0;}
				else if(Fg>0&&Fg<8&&Dat!='#') {Bf[Fg]=Dat;Fg++;Dat=0;}
				else if(Fg==6&&Dat=='#'&&Bf[4]==',') 
								{
									Fg=0;Dat=0;
									Sy=(Bf[1]-'0')*100+(Bf[2]-'0')*10+(Bf[3]-'0');
									Tg=Bf[5]-'0';
									if(Sy&&Tg) tc(Sy,Tg);
								}
				else if(Fg==7&&Dat=='#') 
								{
									Fg=0;Dat=0;
									if(Bf[4]==',')
										{
											Sy=(Bf[1]-'0')*100+(Bf[2]-'0')*10+(Bf[3]-'0');
											Tg=(Bf[5]-'0')*10+(Bf[6]-'0');
										}
									else if(Bf[5]==',')
												{
													Sy=(Bf[1]-'0')*1000+(Bf[2]-'0')*100+(Bf[3]-'0')*10+(Bf[4]-'0');
													Tg=Bf[6]-'0';
												}
									if(Sy&&Tg) tc(Sy,Tg);
								}
				else if(Fg==8&&Dat=='#'&&Bf[5]==',') 
								{
									Fg=0;Dat=0;
									Sy=(Bf[1]-'0')*1000+(Bf[2]-'0')*100+(Bf[3]-'0')*10+(Bf[4]-'0');
									Tg=(Bf[6]-'0')*10+(Bf[7]-'0');
									if(Sy&&Tg) tc(Sy,Tg);
								}
				else Fg=0;
				
				if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
}
