import datetime
import glob
import os

# 指定关键词
keywords = ['kw1','kw2','kw3']


def get_time():
    # 获取当前日期和时间
    now = datetime.datetime.now()
    year = str(now.year % 100)
    month = str(now.month).zfill(2)
    day = str(now.day).zfill(2)
    hour = str(now.hour).zfill(2)
    minute = str(now.minute).zfill(2)
    return year + month + day + '_' + hour + minute


def get_logs():
    # 获取当前路径/含子目录的log文件
    current_path = os.getcwd()
    log = glob.glob(current_path + '/**/*.log', recursive=True)
    log.sort()  #排序
    return log


def find_lines_with_keywords(input_file, output_file, keywords):
    # 提取包含关键词的所在行，写入新文件
    with open(input_file, 'r', encoding="utf-8") as infile, open(output_file, 'a') as outfile:
        lines = infile.readlines()
        for i, line in enumerate(lines):
            # for line in infile:
            if any(keyword in line for keyword in keywords):
                if "PSR_Arm4:Y" in line:
                    for j in range(max(0, i - 6), i + 2):
                        if lines[j].strip() != "":
                            outfile.write(lines[j])
                else:
                    outfile.write(line)
                outfile.write('\n')
        outfile.write("------" + input_file + "------" + "\n")


if __name__ == '__main__':
    log_files = get_logs()
    outfile = get_time() + '.dlg'
    for file in log_files:
        find_lines_with_keywords(file, outfile, keywords)
