#include <STC32G.H>
#include <intrins.h>

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)

/*
241013

55 AA 00 01 00 C4
55 AA 01 01 00 6F

55 AA 03 01 64 24
PT=1,����1A
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
sbit H1=P3^3;	//INT1
sbit H2=P3^2;	//INT0

bit busy;
bit rOK=0,rNG=0,tw=0;
u8 wptr,rptr,buffer[16],recv_buf[16],*ID;
u8 data_length,crc;
u16 adcV=0,Addr=0x0000,PwmArr=0x0200,PwmCcr1=0x0100;
u8 h1z=0,h1f=0,h2z=0,h2f=0;
u8 ch=0x00,ic=0xFF,i1=0,i2=0,v1=0,v2=0;

void Delay60ms(void)	//@24.000MHz
{
	unsigned long edata i;

	_nop_();
	_nop_();
	i = 359998UL;
	while (i) i--;
}

void IoInit(void)
{
	P1M0 = 0xf0; P1M1 = 0x0b; 
	P3M0 = 0xf2; P3M1 = 0x01; 
	P5M0 = 0x10; P5M1 = 0x00; 
	PT = 0;LED = 1;
}

void AdcInit(void)
{
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

void PwmInit(void)
{
	PWMA_CCER1 = 0x00;           //дCCMRxǰ����������CCERx�ر�ͨ��
    PWMA_CCMR1 = 0x60;           //����CC1ΪPWMA���ģʽ
    PWMA_CCER1 = 0x01;           //ʹ��CC1ͨ��
}

void PwmSet(u8 i)
{
	PWMA_CCR1H = (u8)(PwmCcr1>>8);      //0x0100����ռ�ձ�ʱ��
    PWMA_CCR1L = (u8)(PwmCcr1);
	PWMA_ARRH = (u8)(PwmArr>>8);        //0x0200��������ʱ��
    PWMA_ARRL = (u8)(PwmArr);
	PWMA_ENO = i;             	  //ʹ��PWM4P(IN2>0x40),PWM4N(IN1>0x80)�˿����
    PWMA_BKR = 0x80;              //ʹ�������
    PWMA_CR1 = 0x01;              //��ʼ��ʱ
}

void IapIdle(void)
{
    IAP_CONTR = 0;                              //�ر�IAP����
    IAP_CMD = 0;                                //�������Ĵ���
    IAP_TRIG = 0;                               //��������Ĵ���
    IAP_ADDRE = 0x00;
	IAP_ADDRH = 0x00;                           //����ַ���õ���IAP����
    IAP_ADDRL = 0x00;
}

char IapRead(int addr)
{
    char dat;

    IAP_CONTR = 0x80;                           //ʹ��IAP
    IAP_TPS = 12;                               //���õȴ�����12MHz
    IAP_CMD = 1;                                //����IAP������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_ADDRE = addr >> 16;
	IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();
    dat = IAP_DATA;                             //��IAP����
    IapIdle();                                  //�ر�IAP����

    return dat;
}

void IapProgram(int addr, char dat)
{
    IAP_CONTR = 0x80;                           //ʹ��IAP
    IAP_TPS = 12;                               //���õȴ�����12MHz
    IAP_CMD = 2;                                //����IAPд����
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_ADDRE = addr >> 16;
	IAP_DATA = dat;                             //дIAP����
    IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();
    IapIdle();                                  //�ر�IAP����
}

void IapErase(int addr)
{
    IAP_CONTR = 0x80;                           //ʹ��IAP
    IAP_TPS = 12;                               //���õȴ�����12MHz
    IAP_CMD = 3;                                //����IAP��������
    IAP_ADDRL = addr;                           //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;                      //����IAP�ߵ�ַ
    IAP_ADDRE = addr >> 16;
	IAP_TRIG = 0x5a;                            //д��������(0x5a)
    IAP_TRIG = 0xa5;                            //д��������(0xa5)
    _nop_();
	_nop_();
	_nop_();
	_nop_();									
    IapIdle();                                  //�ر�IAP����
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
		case 0x00:	//V=2.5/4095*1000
			i=adcV*0.6105;
			break;
		case 0x01:	//i=2.5/4095/0.01*200/10
			i=adcV*1.221;
			if(i>ic*10)
			{
				PT=0;
				LED=1;
			}
			break;
		case 0x03:	//im=2.5/4095/0.01
			i=adcV*0.06105;	//0.0244;
			break;
		default:
			break;
	}
	UartSend((i>>8)&0xFF);UartSend(i&0xFF);	//ADC
	UartSend(h1z);UartSend(h1f);	//������
	UartSend(h2z);UartSend(h2f);
	UartSend(PwmArr>>8);UartSend(PwmArr);	//����
	UartSend(PwmCcr1>>8);UartSend(PwmCcr1);	//ռ�ձ�
	ch++;
	if(ch>=0x04) ch=0x00;
}
	
void motor(u8 x)
{
	switch (x)
	{
		case 0x00:
			PWMA_ENO &= ~0x40;	//��ֹPWM�������ͨ��I/O��������ߵ͵�ƽ
			PWMA_ENO &= ~0x80;
			IN1 = 0;IN2 = 0;
			break;
		case 0x11:
			PWMA_ENO &= ~0x40;
			PWMA_ENO &= ~0x80;
			IN1 = 1;IN2 = 1;
			break;
		case 0x01:
			PWMA_ENO &= ~0x80;
			PwmSet(0x40);
			IN1 = 0;	//IN2 = 1;
			break;
		case 0x10:
			PWMA_ENO &= ~0x40;
			PwmSet(0x80);
			IN2 = 0;	//IN1 = 1;
			break;
		
		case 0x23:
			PWMA_ENO &= ~0x40;
			PWMA_ENO &= ~0x80;
			IN1 = 0;IN2 = 1;
			break;
		case 0x32:
			PWMA_ENO &= ~0x40;
			PWMA_ENO &= ~0x80;
			IN1 = 1;IN2 = 0;
			break;	
		
		default:
			break;
	}
}

void Vr(u8 i)
{
	switch (i)
	{
		case 0x00:	//��
			PT = 0;LED = 1;
			break;
		case 0x01:	//ͨ
			PT = 1;LED = 0;
			break;
		case 0x02:	//MOTOR
			motor(recv_buf[4]);
			break;
		case 0x03:	//��������
			ic = recv_buf[4];
			i2 = recv_buf[4];
			break;
		case 0x05:	//PDS
			v2=i;C1 = 1;C2 = 0;C3 = 0;
			break;
		case 0x09:
			v2=i;C1 = 0;C2 = 0;C3 = 0;
			break;
		case 0x0C:
			v2=i;C1 = 0;C2 = 0;C3 = 1;
			break;
		case 0x0F:
			v2=i;C1 = 0;C2 = 1;C3 = 1;
			break;
		case 0x14:
			v2=i;C1 = 0;C2 = 1;C3 = 0;
			break;
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
	
	EAXFR = 0x01;
	CKCON = 0x00;
	WTST = 0x00;
	
	IoInit();
	AdcInit();
	UartInit();
	PwmInit();
	
	EADC = 1;
	
	IT0 = 1;	//INT0�½����ж�
	EX0 = 1;
	
	IT1 = 1;	//INT1�½����ж�
	EX1 = 1;
	
	ES = 1;
	EA = 1;
	ADC_START = 1;
	
	i1=IapRead(Addr);	//��ȡ����
	v1=IapRead(Addr+1);	//��ȡPDS
	
	PwmArr = (IapRead(Addr+512)<<8)|IapRead(Addr+513);	//��ȡPWM����
	PwmCcr1 = (IapRead(Addr+514)<<8)|IapRead(Addr+515);	//��ȡPWMռ�ձ�
	
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
			if(recv_buf[3]==0x01)	//����PDS������
			{
				Vr(recv_buf[2]);	//PDS
				if(i1!=i2||v1!=v2)
				{	
					i1=i2;v1=v2;
					IapErase(Addr);
					IapProgram(Addr, i2);
					IapProgram(Addr+1, v2);
				}
			}
			if(recv_buf[3]==0x02)	//����PWM���ں�ռ�ձ�
			{
				if(recv_buf[2]==0x00)	//PwmArr
				{
					PwmArr = (recv_buf[4]<<8)|recv_buf[5];
					if(PwmArr!=(IapRead(Addr+512)<<8)|IapRead(Addr+513))
					{
						IapErase(Addr+512);
						IapProgram(Addr+512, recv_buf[4]);
						IapProgram(Addr+513, recv_buf[5]);
						IapProgram(Addr+514, (u8)(PwmCcr1>>8));
						IapProgram(Addr+515, (u8)(PwmCcr1));
					}
				}
				if(recv_buf[2]==0x01)	//PwmCcr1
				{
					PwmCcr1 = (recv_buf[4]<<8)|recv_buf[5];
					if(PwmCcr1!=(IapRead(Addr+514)<<8)|IapRead(Addr+515))
					{
						IapErase(Addr+512);
						IapProgram(Addr+512, (u8)(PwmArr>>8));
						IapProgram(Addr+513, (u8)(PwmArr));
						IapProgram(Addr+514, recv_buf[4]);
						IapProgram(Addr+515, recv_buf[5]);
					}
				}
			}	
		}
		if(rNG)	rNG=0;
		Delay60ms();
		ICC();
	}
}

void INT0_Isr() interrupt 0
{
	if(H1) h1z++;
	else h1f++;
}

void INT1_Isr() interrupt 2
{
	if(H2) h2z++;
	else h2f++;
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
		case 0x03:
			AdcChs(0x03);break;
		default:
			break;
	}
	ADC_FLAG = 0;
	adcV=(ADC_RES<<8)|ADC_RESL;
	ADC_START = 1;
}
