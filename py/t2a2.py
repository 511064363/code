import edge_tts
import asyncio
TEXT = ""
c=1
# with open ('1.txt','r') as f:
#     for line in f:
#         print(line.strip())
#     data = f.readline()
#     TEXT = data.decode('utf-8')
# print(TEXT)
voice = 'zh-CN-YunxiNeural'
output = '4.mp3'
rate = '-4%'
volume = '+0%'
async def my_function(t,c):
    tts = edge_tts.Communicate(text = t,voice = voice,rate = rate,volume=volume)
    await tts.save(str(c)+".mp3")
if __name__ == '__main__':
    with open('1.txt', 'r') as f:
        for line in f:
            asyncio.run(my_function(line,c))
            c+=1

# import os
#
# # 指定TXT文件路径
# file_path = "文件路径.txt"
#
# # 检查文件是否存在
# if os.path.exists(file_path):
#     # 打开文件
#     with open(file_path, "r") as file:
#         # 逐行读取文件内容
#         for line in file:
#             # 输出每一行内容
#             print(line.strip())
# else:
#     print("文件不存在")