import smtplib
from email.mime.text import MIMEText
from email.header import Header

# 你的邮箱账号和密码
from_addr = 'via2024@163.com'
password = ''

# 收件人邮箱
to_addr = '511064363@qq.com'

# 邮件内容
subject = 'Hpc'
content = 'kj'

# 设置邮件服务器地址和端口
mail_host = 'smtp.163.com'
mail_port = 465

msg = MIMEText(content, 'plain', 'utf-8')
msg['From'] = from_addr
msg['To'] = to_addr
msg['Subject'] = Header(subject, 'utf-8')

# 连接邮件服务器并发送邮件
server = smtplib.SMTP_SSL(mail_host, mail_port)
server.login(from_addr, password)
server.sendmail(from_addr, to_addr, msg.as_string())
server.quit()