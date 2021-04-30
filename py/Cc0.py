# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Cc0.ui'
#
# Created by: PyQt5 UI code generator 5.14.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets
import  serial,binascii,string
def swc():

    from PyQt5.QtWidgets import QApplication, QMainWindow
    try:
        # 端口，GNU / Linux上的/ dev / ttyUSB0 等 或 Windows上的 COM3 等
        # portx = "/dev/tty.usbserial-1710"
        portx="COM3"
        # 波特率，标准值之一：50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200
        bps = 9600
        # 超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
        timex = 5
        ser = serial.Serial(portx, bps, timeout=timex)
        Hex_str = bytes.fromhex('10 11 12 34 3f')
        # 写数据
        #ser.open()
        ser.write(Hex_str)
        n = ser.inWaiting()
        if n:
            data = str(binascii.b2a_hex(ser.read(n)))[2:-1]
            print(data)

        ser.close()  # 关闭串口

    except Exception as e:
        print("---异常---：", e)


def swo():
  #  import serial  # 导入模块
    from PyQt5.QtWidgets import QApplication, QMainWindow
    try:
        # 端口，GNU / Linux上的/ dev / ttyUSB0 等 或 Windows上的 COM3 等
        portx = "com3"
        # 波特率，标准值之一：50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200
        bps = 9600
        # 超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
        timex = 5
        # 打开串口，并得到串口对象
        ser = serial.Serial(portx, bps, timeout=timex)
        #ser.open()
        Hex_str=bytes.fromhex('10 11 12 34 3f')
        # 写数据
        ser.write(Hex_str)
        n=ser.inWaiting()
        if n:
            data=str(binascii.b2a_hex(ser.read(n)))[2:-1]
            print(data)


        ser.close()  # 关闭串口

    except Exception as e:
        print("---异常---：", e)



class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(400, 300)
        self.gridLayout = QtWidgets.QGridLayout(Form)
        self.gridLayout.setObjectName("gridLayout")
        self.left = QtWidgets.QPushButton(Form)
        self.left.setEnabled(True)
        self.left.setMinimumSize(QtCore.QSize(100, 60))
        self.left.setObjectName("left")
        self.gridLayout.addWidget(self.left, 2, 0, 1, 1)
        self.up = QtWidgets.QPushButton(Form)
        self.up.setEnabled(True)
        self.up.setMinimumSize(QtCore.QSize(100, 60))
        self.up.setObjectName("up")
        self.gridLayout.addWidget(self.up, 0, 3, 1, 1)
        self.down = QtWidgets.QPushButton(Form)
        self.down.setEnabled(True)
        self.down.setMinimumSize(QtCore.QSize(100, 60))
        self.down.setObjectName("down")
        self.gridLayout.addWidget(self.down, 4, 3, 1, 1)
        self.right = QtWidgets.QPushButton(Form)
        self.right.setEnabled(True)
        self.right.setMinimumSize(QtCore.QSize(100, 60))
        self.right.setObjectName("right")
        self.gridLayout.addWidget(self.right, 2, 4, 1, 1)

        self.retranslateUi(Form)
        self.up.pressed.connect(swo)
        self.up.released.connect(swc)
        self.left.clicked.connect(swo)
        self.right.clicked.connect(swc)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "CenterControl"))
        self.left.setText(_translate("Form", "←"))
        self.up.setText(_translate("Form", "↑&u"))
        self.down.setText(_translate("Form", "↓"))
        self.right.setText(_translate("Form", "→"))

if __name__=="__main__":
    import sys
    app=QtWidgets.QApplication(sys.argv)
    MainWindow=QtWidgets.QWidget()
    ui=Ui_Form()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())