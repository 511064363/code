#include <STC8.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 9600 / 4)
#define CLK_H  CLK = 1
#define CLK_L  CLK = 0
#define DIO_H  DIO = 1
#define DIO_L  DIO = 0
typedef unsigned char u8;
typedef unsigned int u16;
bit busy;
sbit CLK = P3^2;
sbit DIO = P3^3;
sbit LED=P1^0;
sbit K2=P0^3;sbit K3=P0^4;sbit K4=P0^5;sbit K1=P0^6;sbit K0=P0^7;
sbit A1=P5^2;sbit A2=P5^3;sbit A3=P1^1;sbit A4=P1^2;sbit A5=P1^3;sbit A6=P6^0;sbit A7=P6^1;sbit A8=P6^2;sbit A9=P6^3;sbit A10=P1^4;sbit A11=P4^4;sbit A12=P1^5;
sbit B1=P1^6;sbit B2=P1^7;sbit B3=P5^4;sbit B4=P5^5;sbit B5=P4^0;sbit B6=P6^4;sbit B7=P6^5;sbit B8=P6^6;sbit B9=P6^7;sbit B10=P3^0;sbit B11=P3^1;sbit B12=P3^4;
sbit C1=P3^5;sbit C2=P5^0;sbit C3=P5^1;sbit C4=P3^6;sbit C5=P3^7;sbit C6=P4^1;sbit C7=P7^0;sbit C8=P7^1;sbit C9=P7^2;sbit C10=P7^3;sbit C11=P2^0;sbit C12=P4^2;
sbit D1=P2^1;sbit D2=P2^2;sbit D3=P2^3;sbit D4=P2^4;sbit D5=P2^5;sbit D6=P2^6;sbit D7=P2^7;sbit D8=P7^4;sbit D9=P7^5;sbit D10=P7^6;sbit D11=P7^7;sbit D12=P4^3;
u8 code CO[17] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00}; //0~F???¡ìo????¡ì2??
u8 m=0;
u8 wptr,rptr,buffer[16];
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
void Delay_us(u16 i) //us??¡ì??¡ìo?¡§¡è
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

void IOInit(void)
{
P0M1=0xf8;P0M0=0x00;//1111 1000
P1M1=0x00;P1M0=0x01;//0000 0001
P2M1=0x00;P2M0=0x00;
P3M1=0x00;P3M0=0x00;
P4M1=0x00;P4M0=0x00;
P5M1=0x00;P5M0=0x00;
P6M1=0x00;P6M0=0x00;
P7M1=0x00;P7M0=0x00;
}

void Timer0Init(void)		//30o¡§¡é??@24.000MHz
{
	AUXR &= 0x7F;		//??¡ì¡§o?¨¤???¡§o?¨¤?¡§?12T??¨º¡§o?
	TMOD &= 0xF0;		//¡§|¡§¡§????¡ì¡§o?¨¤?????¨º¡§o?
	TL0 = 0xA0;		//¡§|¡§¡§????¡ì¡§o?¨¤3?¡§o??|¨¬
	TH0 = 0x15;		//¡§|¡§¡§????¡ì¡§o?¨¤3?¡§o??|¨¬
	TF0 = 0;		//??3yTF0?¨¤¡§o??
	TR0 = 1;		//??¡ì¡§o?¨¤???0?a¡§o???¡§o?¨¤
}

void I2CStart(void)//?a?¡ìo?D?o?
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

void I2CStop(void) //?¡ìa?¡§o?1D?o?
{
	CLK_H;
	DIO_L;
	Delay_us(5);
	DIO_H;
}

void I2CWrByte(u8 oneByte) //D???¡ì??????¡§¡é???¡ì2??????¡ì2????¡§o?|¡§??¡ìa????¡ì2o?¡ì?
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

void TM1650_Set(u8 add,u8 dat) //?¡ìoy??1?¡ì1???¡ìo?
{
	I2CStart();
	I2CWrByte(add); //|¡§??¡ì2?¡ì??????????|¡§?????¨¨
	I2Cask();
	I2CWrByte(dat);
	I2Cask();
	I2CStop();
}

bit tda()
{
A1=0;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A2&&A3&&A4&&A5&&A6&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[1]);return 0;}
A1=1;A2=0;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A3&&A4&&A5&&A6&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[2]);return 0;}
A1=1;A2=1;A3=0;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A4&&A5&&A6&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[3]);return 0;}
A1=1;A2=1;A3=1;A4=0;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A5&&A6&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[4]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=0;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A6&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[5]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=0;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A7&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[6]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=0;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A8&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[7]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=0;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A7&&A9&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[8]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=0;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A7&&A8&&A10&&A11&&A12==0) {TM1650_Set(0x68,CO[9]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=0;A11=1;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A7&&A8&&A9&&A11&&A12==0) {TM1650_Set(0x68,CO[10]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=0;A12=1;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A7&&A8&&A9&&A10&&A12==0) {TM1650_Set(0x68,CO[11]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=0;_nop_();_nop_();
if(A1&&A2&&A3&&A4&&A5&&A6&&A7&&A8&&A9&&A10&&A11==0) {TM1650_Set(0x68,CO[12]);return 0;}
TM1650_Set(0x68,CO[0]);return 1;
}

bit tdb()
{
B1=0;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B2&&B3&&B4&&B5&&B6&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[1]);return 0;}
B1=1;B2=0;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B3&&B4&&B5&&B6&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[2]);return 0;}
B1=1;B2=1;B3=0;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B4&&B5&&B6&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[3]);return 0;}
B1=1;B2=1;B3=1;B4=0;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B5&&B6&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[4]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=0;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B6&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[5]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=0;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B7&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[6]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=0;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B8&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[7]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=0;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B7&&B9&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[8]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=0;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B7&&B8&&B10&&B11&&B12==0) {TM1650_Set(0x6A,CO[9]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=0;B11=1;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B7&&B8&&B9&&B11&&B12==0) {TM1650_Set(0x6A,CO[10]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=0;B12=1;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B7&&B8&&B9&&B10&&B12==0) {TM1650_Set(0x6A,CO[11]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=0;_nop_();_nop_();
if(B1&&B2&&B3&&B4&&B5&&B6&&B7&&B8&&B9&&B10&&B11==0) {TM1650_Set(0x6A,CO[12]);return 0;}
TM1650_Set(0x6A,CO[0]);return 1;
}

bit tdc()
{
C1=0;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C2&&C3&&C4&&C5&&C6&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[1]);return 0;}
C1=1;C2=0;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C3&&C4&&C5&&C6&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[2]);return 0;}
C1=1;C2=1;C3=0;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C4&&C5&&C6&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[3]);return 0;}
C1=1;C2=1;C3=1;C4=0;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C5&&C6&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[4]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=0;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C6&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[5]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=0;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C7&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[6]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=0;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C8&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[7]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=0;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C7&&C9&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[8]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=0;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C7&&C8&&C10&&C11&&C12==0) {TM1650_Set(0x6C,CO[9]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=0;C11=1;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C7&&C8&&C9&&C11&&C12==0) {TM1650_Set(0x6C,CO[10]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=0;C12=1;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C7&&C8&&C9&&C10&&C12==0) {TM1650_Set(0x6C,CO[11]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=0;_nop_();_nop_();
if(C1&&C2&&C3&&C4&&C5&&C6&&C7&&C8&&C9&&C10&&C11==0) {TM1650_Set(0x6C,CO[12]);return 0;}
TM1650_Set(0x6C,CO[0]);return 1;
}

bit tdd()
{
D1=0;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D2&&D3&&D4&&D5&&D6&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[1]);return 0;}
D1=1;D2=0;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D3&&D4&&D5&&D6&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[2]);return 0;}
D1=1;D2=1;D3=0;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D4&&D5&&D6&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[3]);return 0;}
D1=1;D2=1;D3=1;D4=0;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D5&&D6&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[4]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=0;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D6&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[5]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=0;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D7&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[6]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=0;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D8&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[7]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=0;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D7&&D9&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[8]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=0;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D7&&D8&&D10&&D11&&D12==0) {TM1650_Set(0x6E,CO[9]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=0;D11=1;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D7&&D8&&D9&&D11&&D12==0) {TM1650_Set(0x6E,CO[10]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=0;D12=1;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D7&&D8&&D9&&D10&&D12==0) {TM1650_Set(0x6E,CO[11]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=0;_nop_();_nop_();
if(D1&&D2&&D3&&D4&&D5&&D6&&D7&&D8&&D9&&D10&&D11==0) {TM1650_Set(0x6E,CO[12]);return 0;}
TM1650_Set(0x6E,CO[0]);return 1;
}

bit tka()
{
A1=0;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A2||A3||A4||A5||A6||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[1]);return 0;}
A1=1;A2=0;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A3||A4||A5||A6||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[2]);return 0;}
A1=1;A2=1;A3=0;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A4||A5||A6||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[3]);return 0;}
A1=1;A2=1;A3=1;A4=0;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A5||A6||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[4]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=0;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A6||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[5]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=0;A7=1;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A7||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[6]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=0;A8=1;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A8||A9||A10||A11||A12) {TM1650_Set(0x68,CO[7]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=0;A9=1;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A7||A9||A10||A11||A12) {TM1650_Set(0x68,CO[8]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=0;A10=1;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A7||A8||A10||A11||A12) {TM1650_Set(0x68,CO[9]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=0;A11=1;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A7||A8||A9||A11||A12) {TM1650_Set(0x68,CO[10]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=0;A12=1;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A7||A8||A9||A10||A12) {TM1650_Set(0x68,CO[11]);return 0;}
A1=1;A2=1;A3=1;A4=1;A5=1;A6=1;A7=1;A8=1;A9=1;A10=1;A11=1;A12=0;_nop_();_nop_();
if(A1||A2||A3||A4||A5||A6||A7||A8||A9||A10||A11) {TM1650_Set(0x68,CO[12]);return 0;}
TM1650_Set(0x68,CO[15]);return 1;
}
bit tkb()
{
B1=0;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B2||B3||B4||B5||B6||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[1]);return 0;}
B1=1;B2=0;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B3||B4||B5||B6||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[2]);return 0;}
B1=1;B2=1;B3=0;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B4||B5||B6||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[3]);return 0;}
B1=1;B2=1;B3=1;B4=0;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B5||B6||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[4]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=0;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B6||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[5]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=0;B7=1;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B7||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[6]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=0;B8=1;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B8||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[7]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=0;B9=1;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B7||B9||B10||B11||B12) {TM1650_Set(0x6A,CO[8]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=0;B10=1;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B7||B8||B10||B11||B12) {TM1650_Set(0x6A,CO[9]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=0;B11=1;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B7||B8||B9||B11||B12) {TM1650_Set(0x6A,CO[10]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=0;B12=1;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B7||B8||B9||B10||B12) {TM1650_Set(0x6A,CO[11]);return 0;}
B1=1;B2=1;B3=1;B4=1;B5=1;B6=1;B7=1;B8=1;B9=1;B10=1;B11=1;B12=0;_nop_();_nop_();
if(B1||B2||B3||B4||B5||B6||B7||B8||B9||B10||B11) {TM1650_Set(0x6A,CO[12]);return 0;}
TM1650_Set(0x6A,CO[15]);return 1;
}

bit tkc()
{
C1=0;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C2||C3||C4||C5||C6||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[1]);return 0;}
C1=1;C2=0;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C3||C4||C5||C6||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[2]);return 0;}
C1=1;C2=1;C3=0;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C4||C5||C6||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[3]);return 0;}
C1=1;C2=1;C3=1;C4=0;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C5||C6||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[4]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=0;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C6||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[5]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=0;C7=1;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C7||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[6]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=0;C8=1;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C8||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[7]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=0;C9=1;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C7||C9||C10||C11||C12) {TM1650_Set(0x6C,CO[8]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=0;C10=1;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C7||C8||C10||C11||C12) {TM1650_Set(0x6C,CO[9]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=0;C11=1;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C7||C8||C9||C11||C12) {TM1650_Set(0x6C,CO[10]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=0;C12=1;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C7||C8||C9||C10||C12) {TM1650_Set(0x6C,CO[11]);return 0;}
C1=1;C2=1;C3=1;C4=1;C5=1;C6=1;C7=1;C8=1;C9=1;C10=1;C11=1;C12=0;_nop_();_nop_();
if(C1||C2||C3||C4||C5||C6||C7||C8||C9||C10||C11) {TM1650_Set(0x6C,CO[12]);return 0;}
TM1650_Set(0x6C,CO[15]);return 1;
}

bit tkd()
{
D1=0;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D2||D3||D4||D5||D6||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[1]);return 0;}
D1=1;D2=0;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D3||D4||D5||D6||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[2]);return 0;}
D1=1;D2=1;D3=0;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D4||D5||D6||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[3]);return 0;}
D1=1;D2=1;D3=1;D4=0;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D5||D6||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[4]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=0;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D6||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[5]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=0;D7=1;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D7||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[6]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=0;D8=1;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D8||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[7]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=0;D9=1;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D7||D9||D10||D11||D12) {TM1650_Set(0x6E,CO[8]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=0;D10=1;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D7||D8||D10||D11||D12) {TM1650_Set(0x6E,CO[9]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=0;D11=1;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D7||D8||D9||D11||D12) {TM1650_Set(0x6E,CO[10]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=0;D12=1;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D7||D8||D9||D10||D12) {TM1650_Set(0x6E,CO[11]);return 0;}
D1=1;D2=1;D3=1;D4=1;D5=1;D6=1;D7=1;D8=1;D9=1;D10=1;D11=1;D12=0;_nop_();_nop_();
if(D1||D2||D3||D4||D5||D6||D7||D8||D9||D10||D11) {TM1650_Set(0x6E,CO[12]);return 0;}
TM1650_Set(0x6E,CO[15]);return 1;
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


void main()
{
	bit z=0;
	IOInit();
	Timer0Init();
	UartInit();
	P_SW2 = 0x30;
	ET0 = 1;                       //¨º1?¨¹?¡§¨º¡À?¡Â?D??
   ES = 1;
	EA = 1;
	WDT_CONTR = 0x24;
	TM1650_Set(0x48,0x81);
	TM1650_Set(0x68,CO[16]);
	TM1650_Set(0x6A,CO[16]);
	TM1650_Set(0x6C,CO[16]);
	TM1650_Set(0x6E,CO[16]);
while (1)
 {
	 WDT_CONTR |= 0x10;
//	 if(!K0) 
//		{
//			Delay10ms();
//			if(K0) z=!z;
//		}
	 if(!K1)
		{
			TM1650_Set(0x68,CO[0]);
			TM1650_Set(0x6A,CO[0]);
			TM1650_Set(0x6C,CO[0]);
			TM1650_Set(0x6E,CO[0]);
			Delay10ms();
			if(K1)
				{
					LED=0;
					TM1650_Set(0x68,CO[16]);
					TM1650_Set(0x6A,CO[16]);
					TM1650_Set(0x6C,CO[16]);
					TM1650_Set(0x6E,CO[16]);
					tda();tdb();tdc();tdd();
					tda();tdb();tdc();tdd();
					tda();tdb();tdc();tdd();
				}
		}
	 else if(!K4)
		 {
			TM1650_Set(0x68,CO[15]);
			TM1650_Set(0x6A,CO[15]);
			TM1650_Set(0x6C,CO[15]);
			TM1650_Set(0x6E,CO[15]);
			Delay10ms();
			if(K4)
				{
					LED=0;
					TM1650_Set(0x68,CO[16]);
					TM1650_Set(0x6A,CO[16]);
					TM1650_Set(0x6C,CO[16]);
					TM1650_Set(0x6E,CO[16]);
					 tka();tkb();tkc();tkd();
					 tka();tkb();tkc();tkd();
					 tka();tkb();tkc();tkd();
				}
		}
 }
}

void TM0_Isr() interrupt 1
{
	m++;
	if(m==30)
	{
		LED = ~LED;
		m=0;
	}
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
        wptr &=0x0f;
    }
}