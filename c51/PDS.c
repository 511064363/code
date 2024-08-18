#include <STC32G.H>
#include <intrins.h>

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)

/*
55 AA 00 01 00 C4
55 AA 01 01 00 6F

55 AA 03 01 64 24
PT=1,限流1A
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
bit rOK=0,rNG=0,tw=0;
u8 wptr,rptr,buffer[16],recv_buf[16],*ID;
u8 data_length,crc;
u16 adcV=0,Addr=0x0000;
u8 ch=0x00,ic=0xFF,i1=0,i2=0,v1=0,v2=0;

void Delay200ms(void)	//@24.000MHz
{
	unsigned long edata i;

	_nop_();
	_nop_();
	i = 1199998UL;
	while (i) i--;
}

void IoInit(void)
{
	P1M0 = 0xf0; P1M1 = 0x0b; 
	P3M0 = 0xf2; P3M1 = 0x0d; 
	P5M0 = 0x10; P5M1 = 0x00; 
	PT = 0;LED = 1;
}

void AdcInit(void)
{
	EAXFR = 1;
	CKCON = 0x00;
	WTST = 0x00;
	
	ADCTIM = 0x3f;
	ADCCFG = 0x2f; 
    ADC_POWER = 1;
}

void UartInit(void)
{
	SCON = 0x50;
	T2L = BRT;
	T2H = BRT >> 8;
	AUXR = 0x15;
	wptr = 0x00;
	rptr = 0x00;
	busy = 0;
}

void Timer3_Init(void)		//100毫秒@24.000MHz
{
	TM3PS = 0x24;			//设置定时器时钟预分频 ( 注意:并非所有系列都有此寄存器,详情请查看数据手册 )
	T4T3M |= 0x02;			//定时器时钟1T模式
	T3L = 0x9F;				//设置定时初始值
	T3H = 0x02;				//设置定时初始值
	T4T3M |= 0x08;			//定时器3开始计时
}

void IapIdle(void)
{
    IAP_CONTR = 0;                              //关闭IAP功能
    IAP_CMD = 0;                                //清除命令寄存器
    IAP_TRIG = 0;                               //清除触发寄存器
    IAP_ADDRE = 0x00;
	IAP_ADDRH = 0x00;                           //将地址设置到非IAP区域
    IAP_ADDRL = 0x00;
}

char IapRead(int addr)
{
    char dat;

    IAP_CONTR = 0x80;                           //使能IAP
    IAP_TPS = 12;                               //设置等待参数12MHz
    IAP_CMD = 1;                                //设置IAP读命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_ADDRE = addr >> 16;
	IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();
    dat = IAP_DATA;                             //读IAP数据
    IapIdle();                                  //关闭IAP功能

    return dat;
}

void IapProgram(int addr, char dat)
{
    IAP_CONTR = 0x80;                           //使能IAP
    IAP_TPS = 12;                               //设置等待参数12MHz
    IAP_CMD = 2;                                //设置IAP写命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_ADDRE = addr >> 16;
	IAP_DATA = dat;                             //写IAP数据
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();
    IapIdle();                                  //关闭IAP功能
}

void IapErase(int addr)
{
    IAP_CONTR = 0x80;                           //使能IAP
    IAP_TPS = 12;                               //设置等待参数12MHz
    IAP_CMD = 3;                                //设置IAP擦除命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_ADDRE = addr >> 16;
	IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();									
    IapIdle();                                  //关闭IAP功能
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

u16 adcFilter()
{
	u8 j;
	u16 k[12],imin,imax,isum;
	for(j=0;j<12;j++) k[j]=adcV;
	imax=k[0];imin=imax;isum=imax;
	for(j=1;j<12;j++)
	{
		 isum+=k[j];
		 if(k[j]>imax) imax=k[j];
		 if(k[j]<imin) imin=k[j];
	}
	return ((isum-imax-imin)/10);
}

void ICC()
{
	u16 i=0;
	adcV=adcFilter();
	UartSend(v1);UartSend(ic);
	UartSend(ch);
	switch (ch)
	{
		case 0x00://V
			i=adcV*0.611;
			break;
		case 0x01://i
			i=adcV*1.221;
			if(i>ic*10)
			{
				PT=0;
				LED=1;
			}
			break;
		case 0x03://Mi
			i=adcV*0.0244;
			break;
		default:
			break;
	}
	UartSend((i>>8)&0xFF);UartSend(i&0xFF);
	ch++;
	if(ch>=0x04) ch=0x00;
}
	
void Vr(u8 i)
{
	switch (i)
	{
		case 0x00:
			PT = 0;LED = 1;break;
		case 0x01:
			PT = 1;LED = 0;break;
		case 0x05:
			v2=i;C1 = 1;C2 = 0;C3 = 0;break;
		case 0x09:
			v2=i;C1 = 0;C2 = 0;C3 = 0;break;
		case 0x0C:
			v2=i;C1 = 0;C2 = 0;C3 = 1;break;
		case 0x0F:
			v2=i;C1 = 0;C2 = 1;C3 = 1;break;
		case 0x14:
			v2=i;C1 = 0;C2 = 1;C3 = 0;break;
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
	
	EAXFR = 0x00;
	CKCON = 0x00;
	WTST=0x00;
	
	IoInit();
	AdcInit();
	UartInit();
	
	EADC = 1;
	ES = 1;
	EA = 1;
	ADC_START = 1;
	
	i1=IapRead(Addr);
	v1=IapRead(Addr+1);
	ic=i1;Vr(v1);
	i2 = i1;
	v2 = v1;

	while(1)
	{
		if(rOK)
		{
			rOK=0;
			for(i=0;i<data_length+5;i++)
				UartSend(recv_buf[i]);
			if(recv_buf[3]==0x01)
			{
				if(recv_buf[2]==0x03)
					{
						if(recv_buf[4])
						{
							ic = recv_buf[4];	//限流
							i2 = recv_buf[4];
						}
					}
				else
					{
						Vr(recv_buf[2]);	//PDS
					}
				if(i1!=i2||v1!=v2)
				{	
					i1=i2;v1=v2;
					IapErase(Addr);
					IapProgram(Addr, i2);
					IapProgram(Addr+1, v2);
				}
			}
		}
		if(rNG)	rNG=0;
		Delay200ms();
		ICC();
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
	switch (ch)
	{
		case 0x00:
			AdcChs(0x00);break;
		case 0x01:
			AdcChs(0x01);break;
		case 0x02:
			AdcChs(0x03);break;
		default:
			break;
	}
	ADC_FLAG = 0;
	adcV=(ADC_RES<<8)|ADC_RESL;
	ADC_START = 1;
}

