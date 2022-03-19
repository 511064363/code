import serial  # 导入serial包
import time  # 导入time包
import pymysql  # 导入pymysql包

log = 0  # 设一个log变量用于记录单次接收次数
s = serial.Serial('com3', 115200, timeout=6)  # 打开串口，配置串口参数和你设备通讯参数一致。
db = pymysql.connect(host="localhost", user="root", password="123456", database="py2sql")  # 打开数据库，配置数据库
cursor = db.cursor()  # 数据库操作

cursor.execute("DROP TABLE IF EXISTS Monitor_Data")  # 如果存在表则重新创建
creatTab = """CREATE TABLE Monitor_Data( # 创建表
    LOG_ID INT NOT NULL,
    D_ID CHAR(20) NOT NULL,
    TIME CHAR(50),
    T_DATA CHAR(20) NOT NULL ,
    H_DATA CHAR(20) NOT NULL ,
    L_DATA CHAR(20) NOT NULL )"""
cursor.execute(creatTab)  # 执行数据库语句

while True:  # 无限循环读取数据
    localtime = time.asctime(time.localtime(time.time()))  # time包操作，打印本地时间
    n = s.readline().decode("utf-8")  # 读取串口一行数据
    m=n.split(",")
    print(m)  # 打印结果在控制台
    log += 1  # 传输次数记录+1
    # data = str(m)  # 强制用字符串格式
    # data = data_pre[2:6]  # 取部分数据
    local_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())  # 规整本地时间的格式
    did = m[1]  # 分类取有效数据
    td = m[2]
    hd = m[3]
    ld = m[4]
    print(local_time, did, td, hd, ld)  # 打印结果在控制台

    sql = "INSERT INTO Monitor_Data(LOG_ID,D_ID,TIME,T_DATA,H_DATA,L_DATA)VALUES('%d','%s','%s','%s','%s','%s')" % (
        log, did, local_time, td, hd, ld)  # 存入数据库
    cursor.execute(sql)  # 执行数据库语句
    db.commit()  # 提交

cursor.close()
db.close()