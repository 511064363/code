#include <STC8F.H>
#include <intrins.h>
#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)

#define WT_12M          0x83
#define ENLVR           0x40                    //RSTCFG.6
#define LVD3V0          0x03                    //LVD@3.0V
#define LVDF            0x20                    //PCON.5

typedef unsigned char u8;
typedef unsigned int u16;

sbit IN1=P1^3;
sbit IN2=P1^2;
sbit IN3=P3^6;
sbit IN4=P3^7;
sbit IN5=P1^6;
sbit IN6=P1^7;
sbit IN7=P1^4;
sbit IN8=P1^5;
sbit nSLEEP=P3^3;
sbit nFAULT=P3^2;
sbit LED=P3^4;

bit busy;
bit sw=0;
u8 wptr,rptr,recv_buf[16],*ID;
u8 it=0,Ln=50;
bit rOK,eCx,eCs;
u8 sC,sX,data_length,crc,rNG;
u16 adr=0x0000;

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

void Delay200ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 25;
	j = 90;
	k = 176;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void IO_Init()
{
	P1M0 = 0xfc; P1M1 = 0x03; 
	P3M0 = 0xca; P3M1 = 0x25; 
	P5M0 = 0x00; P5M1 = 0xff; 

	IN1=0;IN2=0;IN5=0;IN6=0;
	nSLEEP=0;Delay10ms();nSLEEP=1;
	Delay10ms();
}

void Timer4Init()
{
	T4T3M &= 0xDF;			//?????12T??
	T4L = 0xA0;				//???????
	T4H = 0x15;				//???????
	T4T3M |= 0x80;			//???4????
	IE2 = 0x40; 
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
void motor(u8 s)
{
//if(nFAULT){
	switch (s)
		{
			case 0x12:IN1=0;IN2=1;LED=0;
				break;
			case 0x21:IN1=0;IN2=1;LED=0;
				break;

			case 0x34:IN3=0;IN4=1;LED=0;
				break;
			case 0x43:IN3=1;IN4=0;LED=0;
				break;

			case 0x56:IN5=0;IN6=1;LED=0;
				break;
			case 0x65:IN5=1;IN6=0;LED=0;
				break;

			case 0x78:IN7=0;IN8=1;LED=0;
				break;
			case 0x87:IN7=1;IN8=0;LED=0;
				break;

			case 0x00:IN1=0;IN2=0;IN3=0;IN4=0;
								IN5=0;IN6=0;IN7=0;IN8=0;
				break;

			case 0x11:IN1=1;IN2=1;IN3=1;IN4=1;
								IN5=1;IN6=1;IN7=1;IN8=1;
								LED=0;Delay200ms();
				break;
			default:
				break;
		}
//	}
}

void main(void)
{
	u8 i=0;
	IO_Init();
	Timer4Init();
	UartInit();

	PCON &= ~LVDF;                              //上电需要清中断标志
	RSTCFG = LVD3V0;                            //设置LVD电压为3.0V
	ELVD = 1;                                   //使能LVD中断

	ES = 1;EA = 1;

	Ln=IapRead(adr);
	IapErase(adr);

	while(1)
		{
			if(rOK){
				rOK=0;
				for(i=0;i<data_length+5;i++)
							UartSend(recv_buf[i]);
				if(recv_buf[2]) Ln=recv_buf[2]*10;
			}
			if(rNG){
				rNG=0;
				Ln=1;
			}
		}
}

void TM4_Isr() interrupt 20
{
	AUXINTIF &= ~0x04;
	if(it++>Ln){
			it=0;
			LED = ~LED;
		}
}

void UART1_Isr() interrupt 4
{
	static u8 machine_step,recv_data,recv_Cnt;

    if (TI)
    {
        TI = 0;                                 //清中断标志
        busy = 0;                            //测试端口
    }
    if (RI)
    {
			RI = 0;
			recv_data=SBUF;
			switch(machine_step)
			{
				case 0:
					if(0x55 == recv_data)		//1.?????????+1????????ж?????????????????
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
					recv_buf[recv_Cnt] = recv_data;	//3.?????????????????
					machine_step = 3;
					break;
				
				case 3:
					data_length = recv_data;	//	???????	
					recv_Cnt++;
					recv_buf[recv_Cnt] = recv_data; 
					machine_step = 4;
					break;
				
				case 4:
					recv_Cnt++;
					recv_buf[recv_Cnt] = recv_data;
					if(data_length < recv_Cnt-2)			//?????????????????5
						machine_step = 5;
					break;
					
				case 5:
					recv_Cnt++;
					recv_buf[recv_Cnt] = recv_data;
					crc=crc8_MAXIM(recv_buf+2,data_length+2);
					if(crc == recv_data)		
						rOK=1;			//6.????,???1			
					else
					{
						machine_step = 0;			//??????,????????
						rNG=1;
					}
					crc = 0;				//???0,??????????3???
					machine_step = 0;			//++????????????0	
					recv_Cnt = 0;				//???ok,??????
					break;				

				default:break;
		}
  }
		if(SBUF==0xda) {Delay100ms();IAP_CONTR = 0x60;}
}
void LVD_Isr() interrupt 6
{
		PCON &= ~LVDF;
	  P0M0 = 0x00; P0M1 = 0xff; 
    P1M0 = 0x00; P1M1 = 0xff; 
    P2M0 = 0x00; P2M1 = 0xff; 
    P3M0 = 0x00; P3M1 = 0xff; 
    P4M0 = 0x00; P4M1 = 0xff; 
	  IapProgram(adr,Ln);
	  Delay100ms();
}