//仿真数组
twU16 ix=0;
twU16 iy=sizeof(adcArr)/sizeof(adcArr[0]);
ksS16 adcVal=adcArr[ix++];
if(ix>=iy-1) ix=0;

//中位值平均滤波算法
for(ia=0;ia<10;ia++) tA[ia]=tArr[ix++];   
imax=tA[0];imin=imax;sum=imax;
for(ia=1;ia<10;ia++)
{
 sum+=tA[ia];
 if(tA[ia]>imax) imax=tA[ia];
 if(tA[ia]<imin) imin=tA[ia];
}
adcVal=(ksS16)(sum-imax-imin/8);

//噪声
adcVal+=rand()%20;

//滑动平均滤波
int moveAverageFilter(int ad)
{
    if(curNum < M)
    {
        value_buf[curNum] = ad;
        sum += value_buf[curNum];
			  curNum++;
        return sum/curNum;
    }
    else
    {
        sum -= sum/M;
        sum += ad;
        return sum/M;
    }
}

//crc
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

//uart_crc
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
		if(SBUF==0xda){Delay100ms();IAP_CONTR = 0x60;}
   }
}

//模拟按键转开关
bit a2s(u16 ad,u8 m)
{
	b[0]=ad-a[0];
	b[1]=a[0]-a[1];
	b[2]=a[1]-a[2];
	
	a[2]=a[1];
	a[1]=a[0];
	a[0]=ad;

	if(b[0]>0&&b[1]>0&&b[2]>0&&b[0]+b[1]+b[2]>m)
	{
		key=1;
		if(!x)
		{
			tmin=a[2];
			x=1;
		}
	}

	else if(b[0]>=0&&b[1]>=0&&b[2]>=0&&(b[0]+b[1]+b[2]<=m))
	{
		if(!x&&!y)
		{
			tmin=a[2];
			x=1;
		}
		if(y) tmax=ad;
		if(ad-tmin>=m)
		{
			key=1;
		}
	}

	if(key==1)
	{
		i++;
		y=1;
		tmax=ad;
		if(i>200)
		{
			i=0;
			key=0;
		}
	}

	if(b[0]<=0&&b[1]<=0&&b[2]<=0&&b[0]+b[1]+b[2]<-m)
		key=0;

	if(key==0)
	{
		tmin=ad;
		tmax=ad;
		x=0;
		y=0;
		i=0;
	}

	return key;
}

bit a1s(u16 ad,u8 m)
{
	b[0]=ad-a[0];
	b[1]=a[0]-a[1];
	b[2]=a[1]-a[2];
	
	a[2]=a[1];
	a[1]=a[0];
	a[0]=ad;

	if(b[0]<0&&b[1]<0&&b[2]<0&&b[0]+b[1]+b[2]<-m)
	{
		key=1;
		if(!x)
		{
			tmax=a[2];
			x=1;
		}
	}

	else if(b[0]<0&&b[1]<0&&b[2]<0&&(b[0]+b[1]+b[2]>-m))
	{
		if(!x&&!y)
		{
			tmax=a[2];
			x=1;
		}
		if(y) tmin=ad;
		if(ad-tmax<=-m)
		{
			key=1;
		}
	}

	if(key==1)
	{
		i++;
		y=1;
		tmin=ad;
		if(i>200)
		{
			i=0;
			key=0;
		}
	}

	if(b[0]>=0&&b[1]>=0&&b[2]>=0&&b[0]+b[1]+b[2]>m)
		key=0;

	if(key==0)
	{
		tmin=ad;
		tmax=ad;
		x=0;
		y=0;
		i=0;
	}

	return key;
}
