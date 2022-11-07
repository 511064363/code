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

//模拟按键转开关
u16 a2s(u16 ad,u8 m)
{
	b[0]=ad-a[0];b[1]=a[0]-a[1];b[2]=a[1]-a[2];
	a[2]=a[1];a[1]=a[0];a[0]=ad;
		
	if(b[0]>0&&b[1]>0&&b[2]>0&&b[0]+b[1]+b[2]>m) {
			key=1;
			if(x) {
				tmin=a[2];x=0;
			}
	}

	else if(b[0]>=0&&b[1]>=0&&b[2]>=0&&(b[0]+b[1]+b[2]<=m)) {
			if(x&&!y) {
				tmin=a[2];x=0;
			}
			if(y) tmax=ad;
			if(ad-tmin>=m) {
				key=1;
			}
	}

	if(key==1) {
		i++;y=1;tmax=ad;
		if(i>200) {
			i=0;key=0;
	}}

	if(b[0]<=0&&b[1]<=0&&b[2]<=0&&b[0]+b[1]+b[2]<-m)
		key=0;

	if(key==0) {
		tmin=ad;tmax=ad;x=1;y=0;i=0;
	}

	return key;
}
