#include <STC8.H>

#include "intrins.h"

#define FOSC            24000000UL
#define BRT             (65536 - FOSC / 115200 / 4)
#define LVD3V0          0x03 

#define WT_30M          0x80
#define WT_24M          0x81
#define WT_20M          0x82
#define WT_12M          0x83
#define WT_6M           0x84
#define WT_3M           0x85
#define WT_2M           0x86
#define WT_1M           0x87

typedef unsigned char u8;
sbit LR=P2^0;
sbit LB=P2^1;
sbit LG=P2^2;
#define LVDF            0x20                    //PCON.5
bit busy;
char wptr;
char rptr;
char buffer[16];
char BB[6]={0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
void Delay50ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 7;
	j = 23;
	k = 105;
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
void Lvd_Isr() interrupt 6
{
	u8 im;u8 ix=1;
    PCON &= ~LVDF;                              //清中断标志
    if(ix) 
		 {
			 ix=0;
	 IapProgram(0x0400,buffer[0]);_nop_();
		IapProgram(0x0401,buffer[1]);_nop_();	 
			IapProgram(0x0402,buffer[2]);_nop_();	 
	 }}

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
		 if(SBUF==0xda){Delay50ms();IAP_CONTR = 0x60;}
        buffer[wptr++] = SBUF;
        if(wptr>=3) wptr=0;
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
	u8 x,y,z;
    UartInit();
	x=IapRead(0x0400);
	y=IapRead(0x0401);
	z=IapRead(0x0402);
	IapErase(0x0400);
    ES = 1;
    EA = 1;
LR=0;LB=0;LG=0;
	 PCON &= ~LVDF;                              //测试端口
//  RSTCFG = ENLVR | LVD3V0;                    //使能3.0V时低压复位,不产生LVD中断
    RSTCFG = LVD3V0;                            //使能3.0V时低压中断
    ELVD = 1;                                   //使能LVD中断
    EA = 1;
    while (1)
    {
        if (rptr != wptr)
        {
            UartSend(buffer[rptr++]);
            if(rptr>=3) rptr=0;
			  UartSend(x);
			  UartSend(y);
			  UartSend(z);
        }

		  
    }
}

