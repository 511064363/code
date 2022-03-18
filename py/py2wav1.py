# 日期：2021/5/30
# 作者：Leo
# 功能：用于发送short int类型数据到单片机，每次可以发送四个数
# 数据发送类型：设备ID, Data1, Data2, Data3, Data4

import serial,winsound,threading,time,os,sys  #导入模块
from playsound import playsound
from serdatacvt import SendDataCov, GetDataCov

# 打开串口，并得到串口对象
ser = serial.Serial("COM3", 115200, timeout=1)

def rev():
    vV = 0
    while True:
        # 写数据
        while ser.inWaiting() > 0:
            # TxData = SendDataCov(0x200, 0x3233, 0x3435, 0x3637, 0x3839)
            # ser.write(TxData.Hex2String().encode("utf-8"))
            # print("Send OK!")
            # 读数据
            RxData = ser.read(23).decode("utf-8")  #23
            getdata = GetDataCov(RxData)
            # print(getdata.String2Hex())
            cc=RxData[0]
            print(RxData)
            if(cc=='0' and vV==0):
                vV=1
                winsound.Beep(500, 200)
            elif(cc=='1' and vV==1):
                winsound.Beep(800, 200)
                vV = 0

            ser.flushInput()

# ser.close()# 关闭串口
if __name__=='__main__':
    t1=threading.Thread(target=rev)
    t1.start()

