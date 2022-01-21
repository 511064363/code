#!/usr/bin/env python
#-*-coding:utf-8 -*-
#读取文件数据生成折线图
import numpy as np
import matplotlib.pyplot as plt

(a,b,c,d,e,f,g,h,i)=np.loadtxt("~\sd.txt",dtype=int,skiprows=1,comments='#',delimiter=',',usecols=(0,1,2,3,4,5,6,7,8),unpack=True)
plt.title('800_8')
plt.plot(a-25,color='green',label='adc',alpha=0.50)
plt.plot(c-80,color='blue',label='adm',alpha=0.50)
plt.plot(d-120,color='black',label='base',alpha=0.50)
plt.plot(600+e*3,color='red',label='sensorState',alpha=0.50)

plt.legend()
plt.show()
