# -*- coding:utf-8 -*-

from multiprocessing import Pool
# 安装fitz需要安装PyMuPDF才能使用
import fitz
#pip install pymupdf==1.18.17 -i https://pypi.tuna.tsinghua.edu.cn/simple

import os

tmp = os.getcwd()   #pdf路径


#os.makedirs(tmp, exist_ok=True)
pdf_dir = [i for i in os.listdir(tmp) if os.path.splitext(i)[-1] == ".pdf"]

def pdf_to_jpg(name):
    # lock.acquire()
    # 拼接pdf的文件路径
    pwd_name = os.path.join(tmp, name)
    doc = fitz.open(pwd_name)
    # 将文件名同我们的保存路径拼接起来（保存图片的文件夹）
    dir_name = os.path.splitext(name)[0]
    pdf_name = os.path.join(tmp, dir_name)
    for pg in range(doc.page_count):
        page = doc[pg]
        rotate = int(0)
        # 每个尺寸的缩放系数为5，这将为我们生成分辨率提高四倍的图像。
        zoom_x = 5.0
        zoom_y = 5.0
        # trans = fitz.Matrix(zoom_x, zoom_y).preRotate(rotate)
        trans = fitz.Matrix(zoom_x,zoom_y).prerotate(rotate)
        pm = page.get_pixmap(matrix=trans, alpha=False)
        if doc.page_count==1:
            pic_name = '{}.jpg'.format(pdf_name)
        else:
            pic_name = '{}.jpg'.format(pdf_name+str(pg))
        pic_pwd = os.path.join(pdf_name, pic_name)

        # pm.writePNG(pic_pwd)
        # pm.save("page-%i.png" % pg)
        pm.save(pic_pwd)


if __name__ == '__main__':
    pool = Pool(10)
    for i in pdf_dir:
        res = pool.apply_async(pdf_to_jpg, (i,))
    pool.close()
    pool.join()