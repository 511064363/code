import pyautogui,time
pyautogui.FAILSAFE =False
pyautogui.PAUSE = 1
print(pyautogui.size())   # 返回所用显示器的分辨率； 输出：Size(width=1920, height=1080)
width,height = pyautogui.size()
n=0
while n<160:
    n=n+1
    pyautogui.click(334,517)
    time.sleep(25)
