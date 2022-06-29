仿真数组
twU16 ix=0;
twU16 iy=sizeof(adcArr)/sizeof(adcArr[0]);
ksS16 adcVal=adcArr[ix++];
if(ix>=iy-1) ix=0;

中位值平均滤波算法
for(ia=0;ia<10;ia++) tA[ia]=tArr[ix++];   
imax=tA[0];imin=imax;sum=imax;
for(ia=1;ia<10;ia++)
{
 sum+=tA[ia];
 if(tA[ia]>imax) imax=tA[ia];
 if(tA[ia]<imin) imin=tA[ia];
}
adcVal=(ksS16)(sum-imax-imin/8);

噪声
adcVal+=rand()%20;
