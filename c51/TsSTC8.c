#include <STC8.H>
#include <intrins.h>
#include "ts2x.api.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)
#define CLK_H  CLK = 1
#define CLK_L  CLK = 0
#define DIO_H  DIO = 1
#define DIO_L  DIO = 0
#define WT_24M  0x81
#define N 			16
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
u8 ia=0;
u16 idata tArr[N],imin,imax,sum;
	
u8 sk=1;
u8 Dat=0,Fg=0;
twU16 Sy;
twS08 Tg;
ts2xDebug_t dbgInfo;
twU08 sensorState=0;
twS16 adcVal=0;

void IapIdle()
{
    IAP_CONTR = 0;                              //�ر�IAP����
    IAP_CMD = 0;                                //�������Ĵ���
    IAP_TRIG = 0;                               //��������Ĵ���
    IAP_ADDRH = 0x80;                           //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

char IapRead(int addr)
{
    char dat;

    IAP_CONTR = WT_24M;                         //ʹ��IAP
    IAP_CMD = 1;                                //����IAP������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
    dat = IAP_DATA;                             //��IAP����
    IapIdle();                                  //�ر�IAP����

    return dat;
}

void IapProgram(int addr, char dat)
{
    IAP_CONTR = WT_24M;                         //ʹ��IAP
    IAP_CMD = 2;                                //����IAPд����
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_DATA = dat;                             //дIAP����
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
    IapIdle();                                  //�ر�IAP����
}

void IapErase(int addr)
{
    IAP_CONTR = WT_24M;                         //ʹ��IAP
    IAP_CMD = 3;                                //����IAP��������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();                                    //
    IapIdle();                                  //�ر�IAP����
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



void Delay600ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 74;
	j = 14;
	k = 24;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Timer3Init(void)		//32ms@24.000MHz
{
	T4T3M &= 0xFD;		//��ʱ��ʱ��12Tģʽ
	T3L = 0x00;		//���ö�ʱ��ʼֵ
	T3H = 0x06;		//���ö�ʱ��ʼֵ
	T4T3M |= 0x08;		//��ʱ��3��ʼ��ʱ
	IE2 |= 0x20;
}

void Timer4Init(void)		//2ms@24.000MHz
{
	T4T3M &= 0xDF;		//?????12T??
	T4L = 0x60;		//???????
	T4H = 0xF0;		//???????
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
	ADCCFG = 0x2f;                          //0x2f
																					
	adcV=ADC_RES;
	adcV<<=8;
	adcV+=ADC_RESL;
	return adcV;
}

void tc(twU16 sx,twS08 tx)
{
	if((sx>=200)&&(sx<=2500)&&(tx>0&&tx<50))
		{
			if(Sy>>8!=IapRead(0x0000)||Sy!=IapRead(0x0001)||Tg!=IapRead(0x0002))
			{
				IapErase(0x0000);
				IapProgram(0x0000,Sy>>8);
				IapProgram(0x0001,Sy);
				IapProgram(0x0002,Tg);
			}
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
			dbgInfo.longPressDelay = 312; 											// 5000 ms / 32 ms
		}
	else Delay600ms();
}
void uartPrint()
{	
	UartSendS(sensorState);
	UartSendS(adcVal);
	//UartSendS(dbgInfo.resVal);
	//UartSendS(dbgInfo.fwVer);
	UartSendS(dbgInfo.base);
	UartSendS(dbgInfo.admVal);
	UartSendS(dbgInfo.sensitivity);
	UartSendS(dbgInfo.posTrigger);	
	
	UartSend(0x0d);
	UartSend(0x0a);
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


void IoInit()
{
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
}
void Led4(u16 aD)
{
	u8 b[4];
	
	b[0]=aD%10000/1000;
	b[1]=aD%1000/100;
	b[2]=aD%100/10;
	b[3]=aD%10;
	
	if(b[0]==0) TM1650_Set(0x6C,CO[16]);
	else TM1650_Set(0x6C,CO[b[0]]);
	TM1650_Set(0x6E,CO[b[1]]);
	TM1650_Set(0x68,CO[b[2]]);
	TM1650_Set(0x6A,CO[b[3]]);
}

void main()
{
u8 ib,j=0;
UartInit();
Timer3Init();
Timer4Init();

ES = 1;
EA = 1;

IoInit();
	
ADCCFG = 0x0f;          	    //??ADC???????/2/16/16
ADC_CONTR = 0x87;             //??ADC??
	
WDT_CONTR = 0x23;

Sy=0;Tg=0;
Sy=IapRead(0x0000)*256+IapRead(0x0001);
Tg=IapRead(0x0002);
tc(Sy,Tg);
	
TM1650_Set(0x48,0x51);

while (1)
{
	WDT_CONTR |= 0x10;
	
	if(ia==N||ia>30) 
		{
			ia=0;
			imax=tArr[0];
			imin=imax;
			sum=imax;
			for(ib=1;ib<N;ib++)
				{
				 sum+=tArr[ib];
				 if(tArr[ib]>imax) imax=tArr[ib];
				 if(tArr[ib]<imin) imin=tArr[ib];
				}
			adcVal=(twS16)((sum-imax-imin)/(N-2));

			uartPrint();
			if(j++>2) {Led4(adcVal);j=0;}
		}
}}

void TM3_Isr() interrupt 19
{
    sensorState=ts2xSensorUpdate(adcVal,&dbgInfo);  
		if(sensorState==1) LED=0;
		else if(sensorState==0) LED=1;
		
    AUXINTIF &= ~0x02;                          //���жϱ�־
}

void TM4_Isr() interrupt 20
{
	adcVal=adcGet();
	if(adcVal>0&&adcVal<2048) tArr[ia++]=adcVal;

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

				if(Fg==0&&Dat=='#') {Fg++;Dat=0;}
				else if(Fg>0&&Fg<8&&Dat!='#') {Bf[Fg]=Dat;Fg++;Dat=0;}
				else if(Fg==6&&Dat=='#'&&Bf[4]==',') 
								{
									Fg=0;Dat=0;Sy=0;Tg=0;
									Sy=(Bf[1]-'0')*100+(Bf[2]-'0')*10+(Bf[3]-'0');
									Tg=Bf[5]-'0';
									
									tc(Sy,Tg);
								}
				else if(Fg==7&&Dat=='#') 
								{
									Fg=0;Dat=0;Sy=0;Tg=0;
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
											
									tc(Sy,Tg);
								}
				else if(Fg==8&&Dat=='#'&&Bf[5]==',') 
								{
									Fg=0;Dat=0;Sy=0;Tg=0;
									Sy=(Bf[1]-'0')*1000+(Bf[2]-'0')*100+(Bf[3]-'0')*10+(Bf[4]-'0');
									Tg=(Bf[6]-'0')*10+(Bf[7]-'0');
									
									tc(Sy,Tg);
								}
				else Fg=0;
				
				if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
}