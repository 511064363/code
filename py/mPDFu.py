import webbrowser, os, time, fitz, re,shutil
from datetime import datetime
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
#pip install mypdf
current_date = datetime.now().strftime('%Y%m%d')

class X:
    def get_amount_before_yuan(self, text):
        pattern = r"(\d+(\.\d+)?)"  # 匹配数字，可以有小数点和小数部分
        match = re.search(pattern + r"元", text)
        if match:
            return float(match.group(1))
        else:
            return None  # 如果没有找到匹配的金额，则返回None

    def get_pwdsort(self):
        kw1 = '发票'
        kw3 = 'CST'
        input_p2 = []
        # 获取当前的年月日
        # current_date = datetime.now().strftime('%Y%m%d')

        tmp = os.getcwd()  # pdf路径
        pdf_dir = []
        ex=0
        for filename in os.listdir(tmp):
            # 检查文件是否为PDF
            if filename.endswith('.pdf') or filename.endswith('.jpg') or filename.endswith('.png') or filename.endswith('.jpeg'):
                if '_' not in filename:
                    if not filename.startswith('【M'):
                    # 构造新的文件名
                        new_filename = f"{current_date}_{filename}"
                        # 构造完整的旧文件路径
                        old_filepath = os.path.join(tmp, filename)
                        # 构造完整的新文件路径
                        new_filepath = os.path.join(tmp, new_filename)
                        # 重命名文件
                        shutil.move(old_filepath, new_filepath)
                        # print(f"文件 {filename} 已被重命名为 {new_filename}")
                        ex = 1

        if ex:
            exit()

        tmp = os.getcwd()  # pdf路径

        for i in os.scandir(tmp):
            if i.is_file() and i.name.endswith(".pdf"):
                pdf_dir.append(i.name)
        # 输入要合并的PDF文件路径
        if [x for x in pdf_dir if '明细' in x]:
            kw2 = '明细'
        elif [x for x in pdf_dir if '行程单' in x]:
            kw2 = '行程单'
        else:
            kw2 = None

        input_p1 = [j for j in pdf_dir if kw1 in j]
        input_p1.sort()

        input_p3 = [l for l in pdf_dir if kw3 in l]

        if kw2:
            input_p2 = [k for k in pdf_dir if kw2 in k]

        input_p2 += input_p3
        input_p2.sort(reverse=True)

        sum = 0
        for i in input_p1:
            try:
                sum += float(self.get_amount_before_yuan(i))
                sum = round(sum,2)
            except:
                sum = 0

        return input_p1, input_p2, sum

    def merge_pdfs(self, input_paths, output_path):
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

class Y:
    def get_pwdsort(self):
        kw1 = '【M'

        tmp = os.getcwd()  # pdf路径
        pdf_dir = []
        for i in os.scandir(tmp):
            if i.is_file() and i.name.endswith(".pdf"):
                if not i.name.startswith('Tim'):
                    pdf_dir.append(i.name)
        # 输入要合并的PDF文件路径

        input_p1 = [j for j in pdf_dir if kw1 in j]
        input_p1.sort()

        return input_p1

    def merge_pdfs(self, input_paths, output_path):
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

class FileChangeHandler(FileSystemEventHandler):
    def __init__(self, path):
        self.i = 0
        self.path = path

    def on_modified(self, event):
        if event.src_path.endswith(self.path):
            self.i += 1

        if self.i == 1:
            time.sleep(2)
            dir=f'Tim_{current_date}_{count_yuan()}.pdf'
            y.merge_pdfs(y.get_pwdsort(), dir)
            webbrowser.open(os.getcwd() + '/'+dir)
            # 删除缓存文件
            for filename in os.listdir(os.getcwd()):
                if '【M' in filename:
                    delete_file(filename)
            os._exit(0)

def delete_file(filename):
    if os.path.exists(filename):
        os.remove(filename)
        # print(f"文件 {filename} 删除成功！")
    else:
        pass
        # print(f"文件 {filename} 不存在。")

def count_yuan():
    sum = 0
    c = 0

    tmp = os.getcwd()  # pdf路径
    pdf_dir = []
    for i in os.scandir(tmp):
        if i.is_file():
            if '发票' in i.name:
                pattern = r"(\d+(\.\d+)?)"  # 匹配数字，可以有小数点和小数部分
                match = re.search(pattern + r"元", i.name)
                if match:
                    sum += float(match.group(1))
                    c += 1
    return str(sum)+'_'+str(c)

def wd(i, k):
    path = i  # 当前目录
    event_handler = FileChangeHandler(k)
    observer = Observer()
    observer.schedule(event_handler, path, recursive=False)
    observer.start()
    xx=0
    try:
        while True:
            time.sleep(1)
            xx+=1
            # print(xx)
            if xx==2:
                # 删除缓存文件
                for filename in os.listdir(os.getcwd()):
                    if '【M' in filename and '共' in filename and '元' in filename:
                        delete_file(filename)

    except KeyboardInterrupt:
        observer.stop()

    observer.join()


if __name__ == '__main__':
    x = X()
    y = Y()
    sz = x.get_pwdsort()
    wjm = '【M' + str(len(sz[0])) + '】共' + str(sz[2]) + '元' + '.pdf'
    x.merge_pdfs(sz[0], wjm)
    x.merge_pdfs(sz[1], '【M' + str(len(sz[1])) + '】' + '.pdf')
    time.sleep(2)
    webbrowser.open(os.getcwd() + '/' + wjm)
    time.sleep(2)

    wd(os.getcwd(), wjm)