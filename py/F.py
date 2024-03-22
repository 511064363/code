import os
import re
import fitz

def get_pwdsort(kw1,kw2):
    tmp = os.getcwd()  # pdf路径
    pdf_dir = []  # i for i in os.listdwf os.path.splitext(i)[-1] == ".pdf"]
    for i in os.scandir(tmp):
        if (i.is_file() and i.name.endswith(".pdf")):
            pdf_dir.append(i.name)
    # 输入要合并的PDF文件路径
    input_paths = [j for j in pdf_dir if kw1 in j]
    input_p = [k for k in pdf_dir if kw2 in k]
    input_paths.sort()
    input_p.sort()

    sum = 0
    for i in input_paths:
        sum += (float(get_amount_before_yuan(i)))

    return input_paths,input_p,sum
	
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

def get_amount_before_yuan(text):
    pattern = r"(\d+(\.\d+)?)"  # 匹配数字，可以有小数点和小数部分
    match = re.search(pattern + r"元", text)
    if match:
        return float(match.group(1))
    else:
        return None  # 如果没有找到匹配的金额，则返回None

if __name__ == '__main__':
    sz = get_pwdsort('订','顺')

    # 调用合并函数
    merge_pdfs(sz[0], '发票' +str(len(sz[0]))+ '共' +str(sz[2])+'元' + '.pdf')
    merge_pdfs(sz[1], '明细' +str(len(sz[1]))+ '.pdf')