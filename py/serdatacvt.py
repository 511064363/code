class SendDataCov():
    def __init__(self, SerId, Data1, Data2, Data3, Data4):
        self.SerId = SerId      # 0-2^16
        self.Data1 = Data1
        self.Data2 = Data2
        self.Data3 = Data3
        self.Data4 = Data4
        self.stream = []
        self.string = ''

    def Dec2String(self):
        self.stream.append(self.SerId // 256)
        self.stream.append(self.SerId % 256)
        self.stream.append(self.Data1 // 256)
        self.stream.append(self.Data1 % 256)
        self.stream.append(self.Data2 // 256)
        self.stream.append(self.Data2 % 256)
        self.stream.append(self.Data3 // 256)
        self.stream.append(self.Data3 % 256)
        self.stream.append(self.Data4 // 256)
        self.stream.append(self.Data4 % 256)
        for i in range(10):
            self.string = self.string + chr(self.stream[i])
        return self.string
    def Hex2String(self):
        return self.Dec2String()

class GetDataCov():
    def __init__(self, string):
        self.string = string
        self.Data1 = 0
        self.Data2 = 0
        self.Data3 = 0
        self.Data4 = 0
        self.num = []

    def String2Dec(self):
        self.GetID = ord(self.string[0]) * 256 + ord(self.string[1])
        self.num.append(self.GetID)
        self.Data1 = ord(self.string[2]) * 256 + ord(self.string[3])
        self.num.append(self.Data1)
        self.Data2 = ord(self.string[4]) * 256 + ord(self.string[5])
        self.num.append(self.Data2)
        self.Data3 = ord(self.string[6]) * 256 + ord(self.string[7])
        self.num.append(self.Data3)
        self.Data4 = ord(self.string[8]) * 256 + ord(self.string[9])
        self.num.append(self.Data4)
        return self.num

    def String2Hex(self):
        get_data = self.String2Dec()
        a = []
        for i in range(5):
            a.append(hex(get_data[i]))
        return a

if __name__ == '__main__':
    Txdata = SendDataCov(15987, 0x3334, 0x3536, 0x3738, 0x3930)
    send_data = Txdata.Dec2String()
    print(send_data)
    send_data = send_data[0:9]
    RxData = GetDataCov(send_data)
    b = RxData.String2Dec()
    print(b)
