#include <STC8.H>
#include "intrins.h"

#define FOSC            24000000UL
#define BRT             (65536 - FOSC / 115200 / 4)

//串口1~串口4透传

bit busy,busy1;
char wptr,wptr1;
char rptr,rptr1;
char buffer[16],buffer1[16];

void Uart4Isr() interrupt 18
{
    if (S4CON & 0x02)
    {
        S4CON &= ~0x02;
        busy = 0;
    }
    if (S4CON & 0x01)
    {
        S4CON &= ~0x01;
        buffer[wptr++] = S4BUF;
        wptr &= 0x0f;
    }
}

void Uart4Init()
{
    S4CON = 0x50;
    T4L = BRT;
    T4H = BRT >> 8;
    T4T3M = 0xa0;
    wptr = 0x00;
    rptr = 0x00;
    busy = 0;
}

void Uart4Send(char dat)
{
    while (busy);
    busy = 1;
    S4BUF = dat;
}

void Uart4SendStr(char *p)
{
    while (*p)
    {
        Uart4Send(*p++);
    }
}
//**************************
void UartIsr() interrupt 4
{
    if (TI)
    {
        TI = 0;
        busy1 = 0;
    }
    if (RI)
    {
        RI = 0;
        buffer1[wptr1++] = SBUF;
        wptr1 &= 0x0f;
    }
}

void UartInit()
{
    SCON = 0x50;
    T2L = BRT;
    T2H = BRT >> 8;
    AUXR = 0x15;
    wptr1 = 0x00;
    rptr1 = 0x00;
    busy1 = 0;
}

void UartSend(char dat)
{
    while (busy1);
    busy1 = 1;
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
    Uart4Init();
	UartInit();
    IE2 = 0x10;
    EA = 1;
    ES = 1;
    Uart4SendStr("Uart Test !\r\n");

    while (1)
    {
        if (rptr != wptr)
        {
            UartSend(buffer[rptr++]);
            rptr &= 0x0f;
        }
		  if (rptr1 != wptr1)
        {
            Uart4Send(buffer1[rptr1++]);
            rptr1 &= 0x0f;
        }
    }
}

