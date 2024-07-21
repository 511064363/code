#include <STC32G.H>
#include <intrins.h>

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)

/*
55 AA 00 01 00 C4
55 AA 01 01 00 6F
55 AA 05 01 00 F1
55 AA 09 01 00 4A
55 AA 0C 01 00 7F
55 AA 0F 01 00 9B
55 AA 15 01 00 BB
*/

typedef unsigned char u8;
typedef unsigned int u16;

sfr AUXR = 0x8e;
sfr T2H  = 0xd6;
sfr T2L  = 0xd7;

sbit C1=P3^5;
sbit C2=P3^6;
sbit C3=P3^7;

sbit Iadc=P1^1;
sbit Uadc=P1^0;
sbit A1=P1^3;

sbit IN1=P1^7;
sbit IN2=P1^6;
sbit PT=P5^4;
sbit LED=P3^4;
sbit H1=P3^3;
sbit H2=P3^2;

bit busy;
bit rOK=0,rNG=0;
u8 wptr,rptr,buffer[16],recv_buf[16],*ID;
u8 data_length,crc;
u16 adcV=0;

//u16 U_PDget(u16 Uadc)
//{

//return Uadc*2500/4096;
//}

//void I_PDget()
//{

//return Iadc*5/4096;
//}

void IoInit()
{
	P1M0 = 0xf0; P1M1 = 0x0b; 
	P3M0 = 0xf2; P3M1 = 0x0d; 
	P5M0 = 0x10; P5M1 = 0x00; 
	PT=0;
}

void AdcInit()
{
	EAXFR = 1;
	CKCON = 0x00;
	WTST = 0x00;
	
	ADCTIM = 0x3f;
	ADCCFG = 0x2f; 
    ADC_POWER = 1;
}

//u16 AdcGet()
//{
//	ADC_START = 1;                      //启动AD转换
//	_nop_();
//	_nop_();
//	while (!ADC_FLAG);           		//查询ADC完成标志
//	ADC_FLAG = 0;           			//清完成标志
////	ADCCFG = 0x2F;
//	
//	return (ADC_RES<<8)|ADC_RESL;
////	adcV=ADC_RES;
////	adcV<<=8;
////	adcV+=ADC_RESL;
//}

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

u8 crc8_MAXIM(u8 *buffer, u8 len)
{
	u8 crc, i;
	crc = 0x00;
	while(len--)
		{
        crc ^= *buffer++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01) crc = (crc >> 1) ^ 0x8c;
            else crc >>= 1;
        }
    }
    return crc;
}

void UartSendS(u16 x)
{
	u8 a[4],i;
		
	a[0]=x/10000;
	a[1]=x%10000/1000;
	a[2]=x%1000/100;
	a[3]=x%100/10;
		
	if(a[0]!=0)
		{
			for(i=0;i<4;i++) UartSend(a[i]+0x30);
		}
		
	else if(a[1]!=0)
		{
			for(i=1;i<4;i++) UartSend(a[i]+0x30);
		}
		
	else if(a[2]!=0)
		{
			for(i=2;i<4;i++) UartSend(a[i]+0x30);
		}

	else if(a[3]!=0)
		{
			for(i=3;i<4;i++) UartSend(a[i]+0x30);
		}

	UartSend(x%10+0x30);
	UartSend(0x2c);
}

void Vr(u8 i)
{
	switch (i)
	{
		case 0x00:
			PT=0;break;
		case 0x01:
			PT=1;break;
		case 0x05:
			C1=1;C2=0;C3=0;break;
		case 0x09:
			C1=0;C2=0;C3=0;break;
		case 0x0C:
			C1=0;C2=0;C3=1;break;
		case 0x0F:
			C1=0;C2=1;C3=1;break;
		case 0x15:
			C1=0;C2=1;C3=0;break;
		default:
			break;
	}
}


void AdcChs(u8 i)	//Uadc=P10;Iadc=P11;A1=P13;
{
	ADC_CONTR &= 0xF0;
	switch (i)
	{
		case 0x00:
			break;
		case 0x01:
			ADC_CONTR |= 0x01;break;
		case 0x03:
			ADC_CONTR |= 0x03;break;
		default:
			break;
	}
}

void main(void)
{
	u8 i=0;
	
	IoInit();
	AdcInit();
	UartInit();
	EADC = 1;
	ES = 1;
	EA = 1;
	ADC_START = 1;
	
	Vr(0x05);	//上电默认请求PD5V
	AdcChs(0x00);	//上电默认ADC通道0
	
	while(1)
	{
		if(rOK)
		{
			rOK=0;
			for(i=0;i<data_length+5;i++)
				UartSend(recv_buf[i]);
			if(recv_buf[3]==0x01)
			{
				Vr(recv_buf[2]);
				AdcChs(recv_buf[4]);
			}	
		}
		if(rNG)	rNG=0;
		
		UartSendS(adcV);
		UartSend(0x0d);UartSend(0x0a);
	}
}


void UART1_Isr() interrupt 4
{
	static u8 machine_step,recv_data,recv_Cnt;
    if (TI)
    {
        TI = 0;
        busy = 0;
    }
    if (RI)
    {
        RI = 0;
		if(SBUF==0xda) IAP_CONTR = 0x60;
		recv_data=SBUF;
		switch(machine_step)
			{
			case 0:
				if(0x55 == recv_data)
				{
					machine_step = 1;
					recv_Cnt = 0;	
					recv_buf[recv_Cnt] = recv_data;	
				}			
				else
					machine_step = 0;
					break;
			case 1:
				if(0xAA == recv_data)
				{
					machine_step = 2;
					recv_Cnt++;
					recv_buf[recv_Cnt] = recv_data;
				}
				else
					machine_step = 0;
					break;
			case 2:
				recv_Cnt++;
				recv_buf[recv_Cnt] = recv_data;
				machine_step = 3;
				break;
			case 3:
				data_length = recv_data;
				recv_Cnt++;
				recv_buf[recv_Cnt] = recv_data; 
				machine_step = 4;
				break;
			case 4:
				recv_Cnt++;
				recv_buf[recv_Cnt] = recv_data;
				if(data_length < recv_Cnt-2) machine_step = 5;
				break;
			case 5:
				recv_Cnt++;
				recv_buf[recv_Cnt] = recv_data;
				crc=crc8_MAXIM(recv_buf+2,data_length+2);
				if(crc == recv_data) rOK=1;		
				else
				{
					machine_step = 0;
					rNG=1;
				}
				crc = 0;
				machine_step = 0;
				recv_Cnt = 0;
				break;				
			default:break;
			}
		}
}

void ADC_Isr() interrupt 5
{
	ADC_FLAG = 0;
	adcV=(ADC_RES<<8)|ADC_RESL;
	ADC_START = 1;
}