import webbrowser, os, time, sys, watchdog, fitz, re
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

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

        tmp = os.getcwd()  # pdf路径
        pdf_dir = []
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
                pdf_dir.append(i.name)
        # 输入要合并的PDF文件路径

        input_p1 = [j for j in pdf_dir if kw1 in j]
        input_p1.sort(reverse=True)

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

        if self.i == 2:
            time.sleep(6)
            y.merge_pdfs(y.get_pwdsort(), 'P.pdf')
            webbrowser.open(os.getcwd() + '/P.pdf')
            os._exit(0)

def wd(i, k):
    path = i  # 当前目录
    event_handler = FileChangeHandler(k)
    observer = Observer()
    observer.schedule(event_handler, path, recursive=False)
    observer.start()

    try:
        while True:
            time.sleep(1)
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

    wd(os.getcwd(), wjm)