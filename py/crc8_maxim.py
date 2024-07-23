import binascii
import crcmod.predefined

def crc(hexData):
    crc8 = crcmod.predefined.Crc('crc-8-maxim')
    hexData =binascii.unhexlify(hexData)
    crc8.update(hexData)
    return hex(crc8.crcValue)

# 打开文件
with open('1.txt', 'r') as file:
    lines = file.readlines()

with open('2.txt', 'w') as file:
    for line in lines:
        file.write(line.strip()+crc(line.strip())[-2:]+'\n')
