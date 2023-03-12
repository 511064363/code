#!/usr/bin/python
# -*- coding:utf-8 -*-
import os
import datetime
import numpy as np
import matplotlib.pyplot as plt
apa=1
__time = datetime.datetime.now().strftime('%g%m%d')+'_'+datetime.datetime.now().strftime('%H%M%S')

path = os.getcwd()  # 获取当前路径
listDir = os.listdir(path)  # 获取当前目录下的所有内容
z = 0
for j in listDir:
    if os.path.splitext(j)[1] == ".txt":
        z += 1
        (a, b, c, d, e) = np.loadtxt(j, dtype=int, skiprows=1, delimiter=',',comments='#',usecols=(0, 1, 2, 3, 4), converters=None,unpack=True)
        ax=np.ptp(c)/64
        ay=np.mean(d)-16*ax-60

        bx=np.ptp(c)/np.ptp(b)
        by=np.mean(b)*bx-np.mean(c)

        plt.rcParams['font.sans-serif'] = ['SimHei']
        plt.rcParams['axes.unicode_minus'] = False
        plt.title(os.path.splitext(j)[0]+'_'+__time,fontsize=30)
        plt.plot(b*bx-by+np.ptp(c)+60, color='green', label='adc', alpha=apa, linewidth=0.8)
        plt.plot(c, color='blue', label='adm', alpha=apa, linewidth=0.8)
        plt.plot(d, color='orange', label='base', alpha=apa, linewidth=0.8)
        plt.plot(a*ax+ay, color='red', label='key', alpha=apa, linewidth=0.8)

        plt.xlim(-10, len(a) + 26)
        plt.legend(frameon=False)
        plt.draw()
        plt.gcf().set_size_inches(50, 8)
        plt.savefig(str(z) + '_' + os.path.splitext(j)[0] + ".png",transparent=True,dpi=200,bbox_inches="tight")
        plt.show()
        plt.close()
