import edge_tts
import asyncio
TEXT = ""
with open ('1.txt','rb') as f:
    data = f.read()
    TEXT = data.decode('utf-8')
print(TEXT)
voice = 'zh-CN-YunxiNeural'
output = '4.mp3'
rate = '-4%'
volume = '+0%'
async def my_function():
    tts = edge_tts.Communicate(text = TEXT,voice = voice,rate = rate,volume=volume)
    await tts.save(output)
if __name__ == '__main__':
    asyncio.run(my_function())