#include <STC15F2K60S2.H>
#include "intrins.h"

typedef unsigned char u8;
typedef unsigned int u16;

#define FOSC 24000000L          //????
#define BAUD 19200             //?????

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
#define S4_S0 0x04              //P_SW2.2

#define NONE_PARITY     0       //???
#define ODD_PARITY      1       //???
#define EVEN_PARITY     2       //???
#define MARK_PARITY     3       //????
#define SPACE_PARITY    4       //????

#define PARITYBIT NONE_PARITY   //?????

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4RB8 0x04              //S4CON.2
#define S4TB8 0x08              //S4CON.3

#define S4_S0 0x04              //P_SW2.2

bit busy,busy2;
bit rOK=0,rNG=0,r2OK=0,r2NG=0;
u8 recv_buf[16],recv2_buf[16],crc,crc2;
u8 machine_step,recv_data,recv_Cnt,data_length;
u8 machine2_step,recv2_data,recv2_Cnt,data2_length;

void SendData(u8 dat)
{
    while (busy);               //???????????
    ACC = dat;                  //?????P (PSW.0)
    if (P)                      //??P??????
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //??????0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //??????1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //??????1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //??????0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //????UART?????
}



void Send4Data(u8 dat)
{
    while (busy2);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        S4CON &= ~S4TB8;        //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        S4CON |= S4TB8;         //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        S4CON |= S4TB8;         //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        S4CON &= ~S4TB8;        //设置校验位为0
#endif
    }
    busy2 = 1;
    S4BUF = ACC;                //写数据到UART4数据寄存器
}
void uartInit()
{
		ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8??????
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9??????,??????1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9??????,??????0
#endif

    AUXR = 0x40;                //???1?1T??
    TMOD = 0x00;                //???1???0(16?????)
    TL1 = (65536 - (FOSC/4/BAUD));   //????????
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //???1????
    ES = 1;                     //??????
    EA = 1;
}


void uart4Init()
{
		P_SW2 &= ~0x04;            //S4_S0=0 (P0.2/RxD4, P0.3/TxD4)

#if (PARITYBIT == NONE_PARITY)
    S4CON = 0x50;               //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    S4CON = 0xda;               //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    S4CON = 0xd2;               //9位可变波特率,校验位初始为0
#endif

    T4L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T4H = (65536 - (FOSC/4/BAUD))>>8;
    T4T3M |= 0x20;              //定时器4为1T模式
    T4T3M |= 0x80;              //定时器4开始计时
    IE2 = 0x10;                 //使能串口4中断
    EA = 1;   
}



u8 crc8_MAXIM(u8 *dat, u8 len)
{
    u8 crc, i;
    crc = 0x00;

    while(len--)
    {
        crc ^= *dat++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8c;
            }
                else crc >>= 1;
        }
    }
    return crc;
}

u8 crc8_MAXIM2(u8 *dat, u8 len)
{
    u8 crc, i;
    crc = 0x00;

    while(len--)
    {
        crc ^= *dat++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8c;
            }
                else crc >>= 1;
        }
    }
    return crc;
}

void main()
{
	u8 i;
		P0M0 = 0x00;P0M1 = 0x00;
    P1M0 = 0x00;P1M1 = 0x00;
    P2M0 = 0x00;P2M1 = 0x00;
    P3M0 = 0x00;P3M1 = 0x00;
    P4M0 = 0x00;P4M1 = 0x00;
    P5M0 = 0x00;P5M1 = 0x00;
    P6M0 = 0x00;P6M1 = 0x00;
    P7M0 = 0x00;P7M1 = 0x00;
    uartInit();uart4Init();
    while(1)
		{
			if(rOK)
			{
				rOK=0;
				SendData(0XFF);
				Send4Data(recv_buf[2]);
			}
			if(r2OK)
			{
				r2OK=0;
				Send4Data(0XEE);
				SendData(recv2_buf[2]);
			}
			if(rNG)
			{
				rNG=0;
				SendData(0X00);
			}
			if(r2NG)
			{
				r2NG=0;
				Send4Data(recv2_buf[3]);
				Send4Data(crc2);
			}
		}
}

void Uart() interrupt 4
{
    if (RI)
    {
      RI = 0;                 //??RI?
			recv_data=SBUF;
			switch(machine_step)
				{
					case 0:
						if(0x55 == recv_data)		//1.帧头符合，状态+1，下一次中断进入后继续检测，否则丢弃
							machine_step = 1;		
						else
							machine_step = 0;
						break;
						
					case 1:
						if(0xAA == recv_data)
						{
							machine_step = 2;
							recv_Cnt = 0;			//2.即将进入数据区，计数启?
						}
						else
							machine_step = 0;
						break;
									
					case 2:
						recv_buf[recv_Cnt] = recv_data;	//3.数据类型，存第一个数
						machine_step = 3;
						break;
					
					case 3:
						data_length = recv_data;	//	数据长度	
						recv_Cnt++;
						recv_buf[recv_Cnt] = recv_data; 
						machine_step = 4;
						break;
					
					case 4:
						recv_Cnt++;
						recv_buf[recv_Cnt] = recv_data;
						if(data_length < recv_Cnt)			//不满足时，依旧是状态5
							machine_step = 5;
						break;
						
					case 5:
						crc=crc8_MAXIM(recv_buf,data_length+2);
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
    if (TI)
    {
        TI = 0;                 //??TI?
        busy = 0;               //????
    }
}

void Uart4() interrupt 18
{
if (S4CON  & S4RI)
	{
			S4CON &= ~S4RI;         //清除S4RI位
			recv2_data = S4BUF;             //P0显示串口数据
			switch(machine2_step)
				{
					case 0:
						if(0x55 == recv2_data)		//1.帧头符合，状态+1，下一次中断进入后继续检测，否则丢弃
							machine2_step = 1;		
						else
							machine2_step = 0;
						break;
						
					case 1:
						if(0xAA == recv2_data)
						{
							machine2_step = 2;
							recv2_Cnt = 0;			//2.即将进入数据区，计数启?
						}
						else
							machine2_step = 0;
						break;
									
					case 2:
							recv2_buf[recv2_Cnt] = recv2_data;	//3.数据类型，存第一个数
							machine2_step = 3;
						break;
					
					case 3:
						data2_length = recv2_data;	//	数据长度	
						recv2_Cnt++;
						recv2_buf[recv2_Cnt] = recv2_data; 
						machine2_step = 4;
						break;
					
					case 4:
						recv2_Cnt++;
						recv2_buf[recv2_Cnt] = recv2_data;
						if(data2_length < recv2_Cnt)			//不满足时，依旧是状态5
							machine2_step = 5;
						break;
						
					case 5:
						crc2=crc8_MAXIM(recv2_buf,data2_length+2);
						if(crc2 == recv2_data)		
							r2OK=1;			//6.????,???1			
						else
						{
							machine2_step = 0;			//??????,????????
							r2NG=1;
						}
						crc2 = 0;				//???0,??????????3???
						machine2_step = 0;			//++????????????0	
						recv2_Cnt = 0;				//???ok,??????
						break;				

					default:break;
    }
	}
     if (S4CON & S4TI)
    {
        S4CON &= ~S4TI;         //清除S4TI位
        busy2 = 0;               //清忙标志
    }
}