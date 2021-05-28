#include <STC8.H>
#include <intrins.h>
#define CLK_H  CLK = 1
#define CLK_L  CLK = 0
#define DIO_H  DIO = 1
#define DIO_L  DIO = 0
typedef unsigned char uchar;
typedef unsigned int uint;

sbit CLK = P7^7;
sbit DIO = P7^6;


void TM1650_Set(uchar add,uchar dat);


uchar CODE[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9��ʾ����

void main()
{

P_SW2 = 0x20; 

TM1650_Set(0x48,0x21);//Ϊ5�����ȣ�����ʾ�˶���ʾ��ʽ����һ���ֽ��и�λ�������ȣ���λ�����߶λ�˶���ʾ��ʽ�Ϳ�����ʾ
                                   //�磺0x71Ϊ�߼����ȣ��˶���ʾ��ʽ������ʾ��0x79ΪΪ�߼����ȣ��߶���ʾ��ʽ������ʾ
//TM1650_Set(0x68,CODE[0]);
//TM1650_Set(0x6A,CODE[5]);
TM1650_Set(0x6C,CODE[1]);
TM1650_Set(0x6E,CODE[4]);
while(1)
{

TM1650_Set(0x6C,CODE[6]);
TM1650_Set(0x6E,CODE[7]);
	

}
}
void Delay_us(uint i) //us��ʱ
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
void I2CStart(void)//��ʼ�ź�
{
CLK_H;
DIO_H;
Delay_us(5);
DIO_L;

}

void I2Cask(void) //ACK�ź�
{
uchar timeout = 1;
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

void I2CStop(void) //ֹͣ�ź�
{
CLK_H;
DIO_L;
Delay_us(5);
DIO_H;
}

void I2CWrByte(uchar oneByte) //дһ���ֽڸ�λ��ǰ����λ�ں�
{
uchar i;
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

void TM1650_Set(uchar add,uchar dat) //�������ʾ
{
//д�Դ����Ӹߵ�ַ��ʼд
I2CStart();
I2CWrByte(add); //��һ���Դ��ַ
I2Cask();
I2CWrByte(dat);
I2Cask();
I2CStop();
}
