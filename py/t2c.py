#!/usr/bin/python
# -*- coding:utf-8 -*-
import os
import sys
import re
import datetime
__time = datetime.datetime.now().strftime('%g%m%d%H')

def t2c(x,y,z):
    tt = ('////////////////////////////////////////////////////////////////////////////////\n' +
          '/// <pre>\n' +
          '/// * File    : "test_' + __time +'_' + x + '_'+str(z) + '.c"\n' +
          '/// * Author  : kensky\n' +
          '/// * Version :\n' +
          '/// * Date    : ' + datetime.datetime.now().strftime('%F%p') + '\n' +
          '/// * Brief   :\n' +
          '/// * Note    :\n' +
          '/// </pre>\n' +
          '////////////////////////////////////////////////////////////////////////////////\n' +
          '\n' +
          '//*****************************************************************************\n' +
          '#ifdef  __cplusplus\n' +
          '//*****************************************************************************\n' +
          '//\n' +
          '// If building with a C++ compiler, make all of the definitions in this header\n' +
          '// have a C binding.\n' +
          '//\n' +
          '//extern "C"\n' +
          '//{\n' +
          '#endif // __cplusplus\n' +
          '\n' +
          '/**** Includes ****************************************************************/\n' +
          '#include "../../build/inc/tw21.api.config.h"\n' +
          '\n' +
          'twS16 tsArrayTest_' + datetime.datetime.now().strftime('%g%m%d') + '[] =\n'
          '{\n'
          )

    tw = ('\n};\n' +
          '\n' +
          '\n' +
          '#ifdef  __cplusplus\n' +
          '//*****************************************************************************\n' +
          '//\n' +
          '// Mark the end of the C bindings section for C++ compilers.\n' +
          '//\n' +
          '//*****************************************************************************\n' +
          '//}\n' +
          '#endif // __cplusplus\n'
          )
    f = open(y,encoding='utf-8')
    file = open('test_' + __time +'_' + x + '_'+str(z) + '.c','w',encoding='utf-8')
    file.write(tt)
    i=0
    while True:
        line = f.readline()
        if line :
            if re.findall(r"^\d, \d+\b", line):
                m = line.split(",")
                file.write(m[1] + ',')
                n = m[1]
                i += 1
                if i % 9 == 0:
                    file.write('\n')
            else:
                continue
        else:
            break
    file.write(n)
    file.write(tw)
    f.close()

path = os.getcwd()  # ??????????????????
listDir = os.listdir(path)  # ????????????????????????????????????
z=0
for j in listDir:
    if os.path.splitext(j)[1]==".txt":
        if os.path.getsize(j) and re.findall(r"^\d, \d+\b", open(j, encoding='utf-8').readline()):
                z += 1
                t2c(os.path.splitext(j)[0], j, z)
