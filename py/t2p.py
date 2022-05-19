#!/usr/bin/python
# -*- coding:utf-8 -*-
import os
import sys
import re
import datetime
import numpy as np
import matplotlib.pyplot as plt

__time = datetime.datetime.now().strftime('%g%m%d%H')

path = os.getcwd()  # 获取当前路径
listDir = os.listdir(path)  # 获取当前目录下的所有内容
z=0
for j in listDir:
    if os.path.splitext(j)[1]==".txt":
        z += 1
        (a, b, c, d, e) = np.loadtxt(j, dtype=int, skiprows=1, comments='#', delimiter=',',
                                     usecols=(0, 1, 2, 3, 4), unpack=True)

        xadm = int((max(b) - min(b)) / (max(c) - min(c)))
        xkey = int(((max(b) - min(b)) / (max(e) - min(e))) / 2)
        yadm = min(b) - min(c) * xadm - (max(b) - min(b))
        if 64 not in e:
            ykey = min(c) * xadm + yadm
        else:
            ykey = min(c) * xadm + yadm - max(e) * xadm * 1.5

        plt.rcParams['font.sans-serif'] = ['SimHei']
        plt.rcParams['axes.unicode_minus'] = False
        plt.title(os.path.splitext(j)[0])
        plt.plot(b, color='green', label='adc', alpha=1, linewidth=0.8)
        plt.plot((c * xadm + yadm), color='blue', label='adm', alpha=1, linewidth=0.8)
        plt.plot(d * xadm + yadm, color='gray', label='base', alpha=1, linewidth=0.8)
        plt.plot(e * xkey + ykey, color='red', label='key', alpha=1, linewidth=0.8)
        
        plt.legend()
        plt.draw()
        plt.gcf().set_size_inches(60, 12)
        plt.savefig(str(z)+'_'+os.path.splitext(j)[0]+".png", dpi=300)
        # plt.pause(1)
        plt.close()
