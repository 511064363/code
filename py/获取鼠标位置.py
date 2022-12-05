import pyautogui
import time


def search_positon():
    time.sleep(1)
    x1, y1 = pyautogui.position()
    print("您当前位置坐标为:", "x:" + str(x1) + '', "y:" + str(y1))


if __name__ == "__main__":
    while True:
        search_positon()
        pass
