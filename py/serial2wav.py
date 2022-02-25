import serial,threading,time,winsound,os,sys
from playsound import playsound
x=serial.Serial('com2',115200,timeout=0.5)
def rev():
    while True:
        while x.inWaiting()>0:
            myout=x.read(8)
            datas = ''.join(map(lambda x: ('/x' if len(hex(x)) >= 4 else '/x0') + hex(x)[2:], myout))
            #print(datas)
            print(datas[14: 16],datas[18: 20],flush=True)
            if datas[14:16]=='10':
                #winsound.Beep(900,100)
                playsound('12.wav')
                x.flushInput()
if __name__=='__main__':
    t1=threading.Thread(target=rev)
    t1.start()
