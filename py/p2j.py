# -*- coding:utf-8 -*-

from multiprocessing import Pool
# 安装fitz需要安装PyMuPDF才能使用
import fitz
#pip install pymupdf==1.18.17 -i https://pypi.tuna.tsinghua.edu.cn/simple

import os

tmp = r'C:\Users\Xu\PycharmProjects\pythonProject6'   #pdf路径

export_file = r"C:\Users\Xu\PycharmProjects\pythonProject6"

save_path = r"C:\Users\Xu\PycharmProjects\pythonProject6"
os.makedirs(export_file, exist_ok=True)
pdf_dir = [i for i in os.listdir(tmp) if os.path.splitext(i)[-1] == ".pdf"]

def pdf_to_jpg(name):
    # lock.acquire()
    # 拼接pdf的文件路径
    pwd_name = os.path.join(tmp, name)
    doc = fitz.open(pwd_name)
    # 将文件名同我们的保存路径拼接起来（保存图片的文件夹）
    dir_name = os.path.splitext(name)[0]
    pdf_name = os.path.join(export_file, dir_name)
    for pg in range(doc.pageCount):
        page = doc[pg]
        rotate = int(0)
        # 每个尺寸的缩放系数为2，这将为我们生成分辨率提高四倍的图像。
        zoom_x = 2.0
        zoom_y = 2.0
        trans = fitz.Matrix(zoom_x, zoom_y).preRotate(rotate)
        pm = page.getPixmap(matrix=trans, alpha=False)

        pic_name = '{}.jpg'.format(pdf_name)
        # 拼接生成pdf的文件路径
        pic_pwd = os.path.join(pdf_name, pic_name)
        # print(pic_name)
        pm.writePNG(pic_pwd)

if __name__ == '__main__':
    pool = Pool(10)
    for i in pdf_dir:
        res = pool.apply_async(pdf_to_jpg, (i,))
    pool.close()
    pool.join()
