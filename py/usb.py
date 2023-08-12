# -*- coding: utf-8 -*-
import serial
from PySide6 import QtWidgets
################################################################################
## Form generated from reading UI file 'uI.ui'
##
## Created by: Qt User Interface Compiler version 6.5.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################
import serial.tools.list_ports
from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QDialog, QHBoxLayout, QLabel,
                               QPushButton, QSizePolicy, QSpacerItem, QWidget, QMessageBox)

import eder

class Ui_Dialog(object):
    def messageDialog(self):
        msg_box = QMessageBox(QMessageBox.Warning, '警告', '请检查USB连接')
        msg_box.exec()
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(300, 450)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Dialog.sizePolicy().hasHeightForWidth())
        Dialog.setSizePolicy(sizePolicy)
        Dialog.setMinimumSize(QSize(300, 450))
        Dialog.setMaximumSize(QSize(300, 450))
        icon = QIcon()
        icon.addFile(u":/eder/USB.png", QSize(), QIcon.Normal, QIcon.Off)
        Dialog.setWindowIcon(icon)
        self.label = QLabel(Dialog)
        self.label.setObjectName(u"label")
        self.label.setGeometry(QRect(40, 10, 241, 91))
        self.label.setMaximumSize(QSize(500, 200))
        self.label.setPixmap(QPixmap(u":/eder/log.png"))
        self.label.setScaledContents(True)
        self.label.setWordWrap(False)
        self.pushButton = QPushButton(Dialog)
        self.pushButton.setObjectName(u"pushButton")
        self.pushButton.setGeometry(QRect(31, 111, 60, 60))
        sizePolicy1 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(60)
        sizePolicy1.setVerticalStretch(60)
        sizePolicy1.setHeightForWidth(self.pushButton.sizePolicy().hasHeightForWidth())
        self.pushButton.setSizePolicy(sizePolicy1)
        self.pushButton.setMinimumSize(QSize(60, 60))
        self.pushButton.setMaximumSize(QSize(60, 60))
        icon1 = QIcon()
        icon1.addFile(u":/eder/\u677e\u6811.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton.setIcon(icon1)
        self.pushButton.setIconSize(QSize(50, 50))
        self.pushButton_2 = QPushButton(Dialog)
        self.pushButton_2.setObjectName(u"pushButton_2")
        self.pushButton_2.setGeometry(QRect(210, 111, 60, 60))
        sizePolicy1.setHeightForWidth(self.pushButton_2.sizePolicy().hasHeightForWidth())
        self.pushButton_2.setSizePolicy(sizePolicy1)
        self.pushButton_2.setMinimumSize(QSize(60, 60))
        self.pushButton_2.setMaximumSize(QSize(60, 60))
        icon2 = QIcon()
        icon2.addFile(u":/eder/tree_A_3px.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_2.setIcon(icon2)
        self.pushButton_2.setIconSize(QSize(40, 40))
        self.pushButton_3 = QPushButton(Dialog)
        self.pushButton_3.setObjectName(u"pushButton_3")
        self.pushButton_3.setGeometry(QRect(31, 180, 60, 60))
        sizePolicy1.setHeightForWidth(self.pushButton_3.sizePolicy().hasHeightForWidth())
        self.pushButton_3.setSizePolicy(sizePolicy1)
        self.pushButton_3.setMinimumSize(QSize(60, 60))
        self.pushButton_3.setMaximumSize(QSize(60, 60))
        icon3 = QIcon()
        icon3.addFile(u":/eder/25B\u5149\u5708\uff0c\u5feb\u95e8.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_3.setIcon(icon3)
        self.pushButton_3.setIconSize(QSize(50, 50))
        self.pushButton_4 = QPushButton(Dialog)
        self.pushButton_4.setObjectName(u"pushButton_4")
        self.pushButton_4.setGeometry(QRect(210, 180, 60, 60))
        sizePolicy1.setHeightForWidth(self.pushButton_4.sizePolicy().hasHeightForWidth())
        self.pushButton_4.setSizePolicy(sizePolicy1)
        self.pushButton_4.setMinimumSize(QSize(60, 60))
        self.pushButton_4.setMaximumSize(QSize(60, 60))
        icon4 = QIcon()
        icon4.addFile(u":/eder/\u5149\u5708 (1).png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_4.setIcon(icon4)
        self.pushButton_4.setIconSize(QSize(40, 40))
        self.label_3 = QLabel(Dialog)
        self.label_3.setObjectName(u"label_3")
        self.label_3.setGeometry(QRect(120, 150, 71, 18))
        font = QFont()
        font.setPointSize(14)
        self.label_3.setFont(font)
        self.label_3.setStyleSheet(u"color: rgb(166,166,166);")
        self.label_4 = QLabel(Dialog)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setGeometry(QRect(130, 220, 41, 18))
        self.label_4.setFont(font)
        self.label_4.setStyleSheet(u"color: rgb(166,166,166);")
        self.label_5 = QLabel(Dialog)
        self.label_5.setObjectName(u"label_5")
        self.label_5.setGeometry(QRect(130, 120, 51, 21))
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)
        self.label_5.setPixmap(QPixmap(u":/eder/B.png"))
        self.label_5.setScaledContents(True)
        self.label_6 = QLabel(Dialog)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setGeometry(QRect(130, 190, 51, 21))
        sizePolicy.setHeightForWidth(self.label_6.sizePolicy().hasHeightForWidth())
        self.label_6.setSizePolicy(sizePolicy)
        self.label_6.setPixmap(QPixmap(u":/eder/B.png"))
        self.label_6.setScaledContents(True)
        self.layoutWidget = QWidget(Dialog)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(31, 260, 241, 62))
        self.horizontalLayout = QHBoxLayout(self.layoutWidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.pushButton_5 = QPushButton(self.layoutWidget)
        self.pushButton_5.setObjectName(u"pushButton_5")
        sizePolicy1.setHeightForWidth(self.pushButton_5.sizePolicy().hasHeightForWidth())
        self.pushButton_5.setSizePolicy(sizePolicy1)
        self.pushButton_5.setMinimumSize(QSize(60, 60))
        self.pushButton_5.setMaximumSize(QSize(60, 60))
        icon5 = QIcon()
        icon5.addFile(u":/eder/311\u5411\u4e0a\u3001\u6536\u8d77-\u7ebf\u6027\u5706\u6846.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_5.setIcon(icon5)
        self.pushButton_5.setIconSize(QSize(50, 50))

        self.horizontalLayout.addWidget(self.pushButton_5)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer_2)

        self.layoutWidget1 = QWidget(Dialog)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(34, 380, 241, 62))
        self.horizontalLayout_2 = QHBoxLayout(self.layoutWidget1)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_3)

        self.pushButton_6 = QPushButton(self.layoutWidget1)
        self.pushButton_6.setObjectName(u"pushButton_6")
        sizePolicy1.setHeightForWidth(self.pushButton_6.sizePolicy().hasHeightForWidth())
        self.pushButton_6.setSizePolicy(sizePolicy1)
        self.pushButton_6.setMinimumSize(QSize(60, 60))
        self.pushButton_6.setMaximumSize(QSize(60, 60))
        icon6 = QIcon()
        icon6.addFile(u":/eder/Honeyview_311\u5411\u4e0a\u3001\u6536\u8d77-\u7ebf\u6027\u5706\u6846.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_6.setIcon(icon6)
        self.pushButton_6.setIconSize(QSize(50, 50))

        self.horizontalLayout_2.addWidget(self.pushButton_6)

        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_4)

        self.layoutWidget2 = QWidget(Dialog)
        self.layoutWidget2.setObjectName(u"layoutWidget2")
        self.layoutWidget2.setGeometry(QRect(31, 320, 241, 62))
        self.horizontalLayout_3 = QHBoxLayout(self.layoutWidget2)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(0, 0, 0, 0)
        self.pushButton_7 = QPushButton(self.layoutWidget2)
        self.pushButton_7.setObjectName(u"pushButton_7")
        sizePolicy1.setHeightForWidth(self.pushButton_7.sizePolicy().hasHeightForWidth())
        self.pushButton_7.setSizePolicy(sizePolicy1)
        self.pushButton_7.setMinimumSize(QSize(60, 60))
        self.pushButton_7.setMaximumSize(QSize(60, 60))
        icon7 = QIcon()
        icon7.addFile(u":/eder/Honeyview_rt311\u5411\u4e0a\u3001\u6536\u8d77-\u7ebf\u6027\u5706\u6846.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_7.setIcon(icon7)
        self.pushButton_7.setIconSize(QSize(50, 50))

        self.horizontalLayout_3.addWidget(self.pushButton_7)

        self.label_2 = QLabel(self.layoutWidget2)
        self.label_2.setObjectName(u"label_2")
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setFont(font)
        self.label_2.setStyleSheet(u"color: rgb(166,166,166);")

        self.horizontalLayout_3.addWidget(self.label_2)

        self.pushButton_8 = QPushButton(self.layoutWidget2)
        self.pushButton_8.setObjectName(u"pushButton_8")
        sizePolicy1.setHeightForWidth(self.pushButton_8.sizePolicy().hasHeightForWidth())
        self.pushButton_8.setSizePolicy(sizePolicy1)
        self.pushButton_8.setMinimumSize(QSize(60, 60))
        self.pushButton_8.setMaximumSize(QSize(60, 60))
        icon8 = QIcon()
        icon8.addFile(u":/eder/Honeyvi_rt311\u5411\u4e0a\u3001\u6536\u8d77-\u7ebf\u6027\u5706\u6846.png", QSize(), QIcon.Normal, QIcon.Off)
        self.pushButton_8.setIcon(icon8)
        self.pushButton_8.setIconSize(QSize(50, 50))

        self.horizontalLayout_3.addWidget(self.pushButton_8)


        self.retranslateUi(Dialog)
        self.pushButton.pressed.connect(self.ffa)
        self.pushButton.released.connect(self.st)
        self.pushButton_2.pressed.connect(self.ffb)
        self.pushButton_2.released.connect(self.st)
        self.pushButton_3.pressed.connect(self.iia)
        self.pushButton_3.released.connect(self.st)
        self.pushButton_4.pressed.connect(self.iib)
        self.pushButton_4.released.connect(self.st)

        self.pushButton_5.pressed.connect(self.uup)
        self.pushButton_5.released.connect(self.st)
        self.pushButton_6.pressed.connect(self.ddo)
        self.pushButton_6.released.connect(self.st)
        self.pushButton_7.pressed.connect(self.lle)
        self.pushButton_7.released.connect(self.st)
        self.pushButton_8.pressed.connect(self.rri)
        self.pushButton_8.released.connect(self.st)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"\u7535\u52a8\u89c6\u9891\u63a5\u53e3\u63a7\u5236\u7cfb\u7edf", None))
        self.label.setText("")
#if QT_CONFIG(tooltip)
        self.pushButton.setToolTip(QCoreApplication.translate("Dialog", u"F", None))
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        self.pushButton.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.pushButton.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton.setShortcut(QCoreApplication.translate("Dialog", u"F", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.pushButton_2.setToolTip(QCoreApplication.translate("Dialog", u"G", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_2.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_2.setShortcut(QCoreApplication.translate("Dialog", u"G", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.pushButton_3.setToolTip(QCoreApplication.translate("Dialog", u"I", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_3.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_3.setShortcut(QCoreApplication.translate("Dialog", u"I", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.pushButton_4.setToolTip(QCoreApplication.translate("Dialog", u"O", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_4.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_4.setShortcut(QCoreApplication.translate("Dialog", u"O", None))
#endif // QT_CONFIG(shortcut)
        self.label_3.setText(QCoreApplication.translate("Dialog", u"FOCUS", None))
        self.label_4.setText(QCoreApplication.translate("Dialog", u"IRIS", None))
        self.label_5.setText("")
        self.label_6.setText("")
#if QT_CONFIG(tooltip)
        self.pushButton_5.setToolTip(QCoreApplication.translate("Dialog", u"\u2191", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_5.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_5.setShortcut(QCoreApplication.translate("Dialog", u"Up", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.pushButton_6.setToolTip(QCoreApplication.translate("Dialog", u"\u2193", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_6.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_6.setShortcut(QCoreApplication.translate("Dialog", u"Down", None))
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        self.pushButton_7.setToolTip(QCoreApplication.translate("Dialog", u"\u2190", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_7.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_7.setShortcut(QCoreApplication.translate("Dialog", u"Left", None))
#endif // QT_CONFIG(shortcut)
        self.label_2.setText(QCoreApplication.translate("Dialog", u" X - Y", None))
#if QT_CONFIG(tooltip)
        self.pushButton_8.setToolTip(QCoreApplication.translate("Dialog", u"\u2192", None))
#endif // QT_CONFIG(tooltip)
        self.pushButton_8.setText("")
#if QT_CONFIG(shortcut)
        self.pushButton_8.setShortcut(QCoreApplication.translate("Dialog", u"Right", None))
#endif // QT_CONFIG(shortcut)
    # retranslateUi


    def jy(self):
        self.pushButton.setEnabled(False)
        self.pushButton_2.setEnabled(False)
        self.pushButton_3.setEnabled(False)
        self.pushButton_4.setEnabled(False)
        self.pushButton_5.setEnabled(False)
        self.pushButton_6.setEnabled(False)
        self.pushButton_7.setEnabled(False)
        self.pushButton_8.setEnabled(False)
        Form.setStyleSheet(
            "QPushButton{color:#a6a6a6;background-color: rgb(225, 225, 225);border:1px groove gray;border-radius:10px;padding:1px 4px;border-style: outset;}\n"
            "QPushButton:hover{color:#4286F3;background-color:rgb(229, 241, 251);}\n"
            "QPushButton:pressed{color:#4286F3;background-color:rgb(204, 228, 247);border-style: inset;}"
            )

    def ser(self):
        port_list = serial.tools.list_ports.comports()
        # if len(port_list) <= 0:
        #     print("无串口设备。")
        # else:
        #     print("可用的串口设备如下：")
        for com in port_list:
            if 'CH340' in list(com)[1]:    #win
            # if 'wch' in list(com)[0]:     # mac
                xc = com[0]
                try:
                    s = serial.Serial(xc, 115200, timeout=0.5)
                except Exception as e:
                    return None
                i=100
                while (i>0):
                    i-=1
                    s.write(bytes.fromhex('bb'))
                    n = s.inWaiting()
                    if n:
                        # if (str(s.read())[4:6]) == "cc":
                        if ("cc" in str(s.read())):
                            # print(s.read())
                            return xc
        return None

    def se(self,sp):
        # cp = configparser.ConfigParser()
        # try:
        #     cp.read('sp0.ini')
        #     conn = cp.get('com', 'serialPort')
        # except Exception as e:
        #     print(e)
        global s
        d=bytes.fromhex('00')
        try:
            s= serial.Serial(sp, 115200, timeout=0.5)
            # s = serial.Serial("/dev/tty.usbserial-1710", 9600, timeout=0.5)
            s.write(d)
        except Exception as e:
            print(e)
            self.wt(d)

    def wt(self, n):
        try:
            s.write(n)
        except Exception as e:
            self.jy()
            #print("could not open serial port!", e)

    def st(self):
        d = bytes.fromhex('00')
        self.wt(d)

    def ffa(self):
        d = bytes.fromhex('56')
        self.wt(d)

    def ffb(self):
        d = bytes.fromhex('65')
        self.wt(d)

    def iia(self):
        d = bytes.fromhex('34')
        self.wt(d)

    def iib(self):
        d = bytes.fromhex('43')
        self.wt(d)

    def uup(self):
        d = bytes.fromhex('12')
        self.wt(d)

    def ddo(self):
        d = bytes.fromhex('21')
        self.wt(d)

    def lle(self):
        d = bytes.fromhex('78')
        self.wt(d)

    def rri(self):
        d = bytes.fromhex('87')
        self.wt(d)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Dialog()

    ui.setupUi(Form)
    sp = ui.ser()
    if sp==None:
        ui.messageDialog()
    Form.show()
    ui.se(sp)
    sys.exit(app.exec())