import os
import glob

# 批量提取当前路径下含关键字的所在行，写入output.txt文件
keywords = ['PSR_Arm4:Y','Cnl Magnet Detector Released','Behavior Task/Match Grip(17): Task Started Active','Cnl Magnet Detector Released','Enter Power Off Countdown','is dead detected by AppLivelinessMonitor in']

# 获取当前路径/含子目录的log文件
def get_logs():
    current_path = os.getcwd()
    log = glob.glob(current_path + '/**/*.log', recursive=True)
    log.sort()    #排序
    return log

# 调用函数查找包含关键词的行并写入新文件
def find_lines_with_keywords(input_file, output_file, keywords):
    with open(input_file, 'r', encoding="utf-8") as infile, open(output_file, 'a') as outfile:
        lines = infile.readlines()
        # print(content[8])
        for i,line in enumerate(lines):
        # for line in infile:
            if any(keyword in line for keyword in keywords):
                if "PSR_Arm4:Y" in line:
                    for j in range(max(0, i - 6), i+2):
                        if lines[j].strip() != "":
                            outfile.write(lines[j])
                else:
                    outfile.write(line)
                outfile.write('\n')
        outfile.write("---"+input_file+"---" + "\n")

if __name__ == '__main__':
    log_files=get_logs()
    for file in log_files:
        find_lines_with_keywords(file, 'output.txt', keywords)