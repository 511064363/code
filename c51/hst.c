#include <STC8.H>
#include "intrins.h"

#define FOSC            24000000UL
#define BRT             (65536 - FOSC / 9600 / 4)

#define CLK_H  CLK = 1
#define CLK_L  CLK = 0
#define DIO_H  DIO = 1
#define DIO_L  DIO = 0
typedef unsigned char u8;
typedef unsigned int u16;
sbit LED = P5^4;
sbit CLK = P1^5;
sbit DIO = P1^4;
u8 code CO[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71}; //0~FÏÔÊ¾´úÂë
u8 code UP[3]={0x02,0x00,0x06},DOWN[3]={0x08,0x00,0x0C},LEFT[3]={0x20,0x00,0x24},RIGHT[3]={0x04,0x00,0x08},CEN[3]={0x10,0x00,0x14};
u8 code FOCUSL[3]={0x00,0x40,0x44},FOCUSR[3]={0x00,0x20,0x24},ZOOML[3]={0x01,0x00,0x05},ZOOMR[3]={0x00,0x80,0x84};
u8 code LS[3]={0x00,0x01,0x05},LA[3]={0x00,0x02,0x06},LB[3]={0x00,0x04,0x08},CC[3]={0x00,0x08,0x0C};DHD[3]={0x00,0x00,0x00};
bit busy,busy1;
u8 wptr,wptr1;
u8 rptr,rptr1;
u8 buffer[16],buffer1[16];
char code hc[5]={0xA5,0x5A,0x05,0xB5,0x5B};

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
        if(SBUF==0xdf){Delay100ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        if(wptr>=8) wptr=0;
    }
}

void UartInit()
{
    SCON = 0x50;
    TMOD = 0x00;
    TL1 = BRT;
    TH1 = BRT >> 8;
    TR1 = 1;
    AUXR = 0x40;
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

void Uart2Isr() interrupt 8
{
    if (S2CON & 0x02)
    {
        S2CON &= ~0x02;
        busy1 = 0;
    }
    if (S2CON & 0x01)
    {
        S2CON &= ~0x01;
        buffer1[wptr1++] = S2BUF;
        if(wptr1>=8) wptr1=0;
    }
}

void Uart2Init()
{
    S2CON = 0x10;
    T2L = BRT;
    T2H = BRT >> 8;
    AUXR |= 0x14;
    wptr1 = 0x00;
    rptr1 = 0x00;
    busy1 = 0;
}

void Uart2Send(char dat)
{
    while (busy1);
    busy1 = 1;
    S2BUF = dat;
}

void Delay_us(u16 i) //usÑÓÊ±
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

void I2CStart(void)//¿ªÊ¼ÐÅºÅ
{
	CLK_H;
	DIO_H;
	Delay_us(5);
	DIO_L;
}

void I2Cask(void) //ACKÐÅºÅ
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

void I2CStop(void) //Í£Ö¹ÐÅºÅ
{
	CLK_H;
	DIO_L;
	Delay_us(5);
	DIO_H;
}

void I2CWrByte(u8 oneByte) //Ð´Ò»¸ö×Ö½Ú¸ßÎ»ÔÚÇ°£¬µÍÎ»ÔÚºó
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

void TM1650_Set(u8 add,u8 dat) //ÊýÂë¹ÜÏÔÊ¾
{
	I2CStart();
	I2CWrByte(add); //µÚÒ»¸öÏÔ´æµØÖ·
	I2Cask();
	I2CWrByte(dat);
	I2Cask();
	I2CStop();
}

void main()
{
	 LED=0;
    UartInit();
	 Uart2Init();
	 IE2 = 0x01;
    ES = 1;
    EA = 1;
	 WDT_CONTR = 0x24;
	 P_SW2 = 0x00;                               //SCL/P1.5, SDA/P1.4
	 TM1650_Set(0x48,0x81);
	 TM1650_Set(0x68,CO[11]);
	 TM1650_Set(0x6A,CO[11]);
    while (1)
    {
		  WDT_CONTR |= 0x10;
        if (rptr != wptr)
        {
            UartSend(buffer[rptr++]);
			   if(rptr>=8) rptr=0;
        }
		  if (rptr1 != wptr1)
        {
            Uart2Send(buffer1[rptr1++]);
			   if(rptr1>=8) rptr1=0;
        }
		if(buffer[0]==hc[0]&&buffer[1]==hc[1]&&buffer[2]==hc[2]&&buffer[6]==hc[3]&&buffer[7]==hc[4]) 
		  {
			  if(buffer[3]==UP[0]&&buffer[4]==UP[1]&&buffer[5]==UP[2]) TM1650_Set(0x68,CO[1]);
			  else if(buffer[3]==DOWN[0]&&buffer[4]==DOWN[1]&&buffer[5]==DOWN[2]) TM1650_Set(0x68,CO[2]);
			  else if(buffer[3]==LEFT[0]&&buffer[4]==LEFT[1]&&buffer[5]==LEFT[2]) TM1650_Set(0x68,CO[3]);
			  else if(buffer[3]==RIGHT[0]&&buffer[4]==RIGHT[1]&&buffer[5]==RIGHT[2]) TM1650_Set(0x68,CO[4]);
			  else if(buffer[3]==FOCUSL[0]&&buffer[4]==FOCUSL[1]&&buffer[5]==FOCUSL[2]) TM1650_Set(0x68,CO[5]);
			  else if(buffer[3]==FOCUSR[0]&&buffer[4]==FOCUSR[1]&&buffer[5]==FOCUSR[2]) TM1650_Set(0x68,CO[6]);
			  else if(buffer[3]==ZOOML[0]&&buffer[4]==ZOOML[1]&&buffer[5]==ZOOML[2]) TM1650_Set(0x68,CO[7]);
			  else if(buffer[3]==ZOOMR[0]&&buffer[4]==ZOOMR[1]&&buffer[5]==ZOOMR[2]) TM1650_Set(0x68,CO[8]);
			  else if(buffer[3]==DHD[0]&&buffer[4]==DHD[1]&&buffer[5]==DHD[2]) TM1650_Set(0x68,CO[9]);			  
			  else if(buffer[3]==LS[0]&&buffer[4]==LS[1]&&buffer[5]==LS[2]) TM1650_Set(0x68,CO[10]);
			  else if(buffer[3]==LA[0]&&buffer[4]==LA[1]&&buffer[5]==LA[2]) TM1650_Set(0x68,CO[11]);
			  else if(buffer[3]==LB[0]&&buffer[4]==LB[1]&&buffer[5]==LB[2]) TM1650_Set(0x68,CO[12]);
			  else if(buffer[3]==CC[0]&&buffer[4]==CC[1]&&buffer[5]==CC[2]) TM1650_Set(0x68,CO[13]);	  
			  LED=~LED;
		  }
		  if(buffer1[0]==hc[0]&&buffer1[1]==hc[1]&&buffer1[2]==hc[2]&&buffer1[6]==hc[3]&&buffer1[7]==hc[4]) 
		  {
			  if(buffer1[3]==UP[0]&&buffer1[4]==UP[1]&&buffer1[5]==UP[2]) TM1650_Set(0x6A,CO[1]);
			  else if(buffer1[3]==DOWN[0]&&buffer1[4]==DOWN[1]&&buffer1[5]==DOWN[2]) TM1650_Set(0x6A,CO[2]);
			  else if(buffer1[3]==LEFT[0]&&buffer1[4]==LEFT[1]&&buffer1[5]==LEFT[2]) TM1650_Set(0x6A,CO[3]);
			  else if(buffer1[3]==RIGHT[0]&&buffer1[4]==RIGHT[1]&&buffer1[5]==RIGHT[2]) TM1650_Set(0x6A,CO[4]);
			  else if(buffer1[3]==FOCUSL[0]&&buffer1[4]==FOCUSL[1]&&buffer1[5]==FOCUSL[2]) TM1650_Set(0x6A,CO[5]);
			  else if(buffer1[3]==FOCUSR[0]&&buffer1[4]==FOCUSR[1]&&buffer1[5]==FOCUSR[2]) TM1650_Set(0x6A,CO[6]);
			  else if(buffer1[3]==ZOOML[0]&&buffer1[4]==ZOOML[1]&&buffer1[5]==ZOOML[2]) TM1650_Set(0x6A,CO[7]);
			  else if(buffer1[3]==ZOOMR[0]&&buffer1[4]==ZOOMR[1]&&buffer1[5]==ZOOMR[2]) TM1650_Set(0x6A,CO[8]);
			  else if(buffer1[3]==LS[0]&&buffer1[4]==LS[1]&&buffer1[5]==LS[2]) TM1650_Set(0x6A,CO[10]);
			  else if(buffer1[3]==LA[0]&&buffer1[4]==LA[1]&&buffer1[5]==LA[2]) TM1650_Set(0x6A,CO[11]);
			  else if(buffer1[3]==LB[0]&&buffer1[4]==LB[1]&&buffer1[5]==LB[2]) TM1650_Set(0x6A,CO[12]);
			  else if(buffer1[3]==CC[0]&&buffer1[4]==CC[1]&&buffer1[5]==CC[2]) TM1650_Set(0x6A,CO[13]);	  
			  LED=~LED;
		  }
    }
}