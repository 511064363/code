import os
import glob

current_path = os.getcwd()
log_files = glob.glob(current_path + '/**/*.log', recursive=True)
log_files.sort()    #排序

output_file = 'output.txt'
keywords = ['PSR_Arm4:Y','Cnl Magnet Detector Pressed','Behavior Task/Match Grip(17): Task Started Active','Cnl Magnet Detector Released','Enter Power Off Countdown','is dead detected by AppLivelinessMonitor in']

def find_lines_with_keywords(input_file, output_file, keywords):
    with open(input_file, 'r') as infile, open(output_file, 'a') as outfile:
        for line in infile:
            if any(keyword in line for keyword in keywords):
                outfile.write(line)
        outfile.write("=======================" + "\n")

for file in log_files:
    # 调用函数查找包含关键词的行并写入新文件
    find_lines_with_keywords(file, output_file, keywords)
