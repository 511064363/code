#!/usr/bin/python
# -*- coding:utf-8 -*-
import re
import datetime
__time = datetime.datetime.now().strftime('%g%m%d')
__hms = datetime.datetime.now().strftime('_%H%M%S')
f = open('t.txt',encoding='utf-8')
file = open('test_'+__time+__hms+'.c','w')

tt=('////////////////////////////////////////////////////////////////////////////////\n'+
    '/// <pre>\n'+
    '/// * File    : "test_'+__time+__hms+'.c"\n'+
    '/// * Author  : kensky\n'+
    '/// * Version :\n'+
    '/// * Date    : '+datetime.datetime.now().strftime('%F%p')+'\n'+
    '/// * Brief   :\n'+
    '/// * Note    :\n'+
    '/// </pre>\n'+
    '////////////////////////////////////////////////////////////////////////////////\n'+
    '\n'+
    '//*****************************************************************************\n'+
    '#ifdef  __cplusplus\n'+
    '//*****************************************************************************\n'+
    '//\n'+
    '// If building with a C++ compiler, make all of the definitions in this header\n'+
    '// have a C binding.\n'+
    '//\n'+
    '//extern "C"\n'+
    '//{\n'+
    '#endif // __cplusplus\n'+
    '\n'+
    '/**** Includes ****************************************************************/\n'+
    '#include "../../build/inc/tw21.api.config.h"\n'+
    '\n'+
    'twS16 tsArrayTest_'+__time+'[] =\n'
    '{\n'
)

tw=('\n};\n'+
    '\n'+
    '\n'+
    '#ifdef  __cplusplus\n'+
    '//*****************************************************************************\n'+
    '//\n'+
    '// Mark the end of the C bindings section for C++ compilers.\n'+
    '//\n'+
    '//*****************************************************************************\n'+
    '//}\n'+
    '#endif // __cplusplus\n'
)

file.write(tt)
i=0
while True:
    line = f.readline()

    if line :
        if re.findall(r'\d, \d,+', line):
            m = line.split(",")
            file.write(m[1] + ',')
            n = m[1]
            i = i + 1
            if i % 9 == 0:
                file.write('\n')
        else:
            continue
    else:
        break

file.write(n)
file.write(tw)
f.close()
