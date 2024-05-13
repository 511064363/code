import os
import re
import fitz

def get_pwdsort():
    kw1 = '【M'

    tmp = os.getcwd()  # pdf路径
    pdf_dir = []
    for i in os.scandir(tmp):
        if i.is_file() and i.name.endswith(".pdf"):
            pdf_dir.append(i.name)
    # 输入要合并的PDF文件路径

    input_p1 = [j for j in pdf_dir if kw1 in j]
    input_p1.sort()


    return input_p1

def merge_pdfs(input_paths, output_path):
    # 创建PDF文档对象
    merged_doc = fitz.open()
    # 逐个打开并合并输入的PDF文件
    for path in input_paths:
        try:
            doc = fitz.open(path)
        except:
            continue
        else:
            doc = fitz.open(path)
            merged_doc.insert_pdf(doc)
            doc.close()
    try:
        # 保存合并后的文档
        merged_doc.save(output_path)
    except:
        pass
    else:
        merged_doc.close()


if __name__ == '__main__':
    sz = get_pwdsort()

    # 调用合并函数
    merge_pdfs(sz, 'P.pdf')
