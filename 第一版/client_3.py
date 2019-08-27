# _*_ coding.uft-8 _# _
#开发人 :马慧发
import socket
import time
import threading
from queue import Queue
detail_url_queue_1 = Queue(maxsize=1000)
# 创建 socket 对象
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 连接服务，指定主机和端口
s.connect(('192.168.43.192', 8002))

def recv_data(q1):
    """接收数据"""
    while True:
            a = s.recv(1024)
            data = a.decode('utf-8')
            q1.put(data)
            #print(data)


def send_data():
    """发送数据"""
    while True:
        data = str(input('输入:'))
        s.send(data.encode('utf-8'))

def print_data(q1):
    """打印数据"""
    while True:
        for i in range(4):
            b1 = q1.get()
            b2 = q1.get()
            if b1 == '0x01':
                b_1 =b2
                #print('数字：%s'% b_1,end='-')
            elif b1 == '0x02':
                b_2 =b2
                #print('字母：%s'% b_2,end='-')
            elif b1 == '0x03':
                b_3 = b2
                #print('小写：%s'% b_3,end='-')
            elif b1 == '0x04':
                b_4 = b2
        print('温度:%s--湿度：%s--光照：%s--下限：%s' % (b_1,b_2,b_3,b_4))


if __name__ == '__main__':
    try:
        thread_send = threading.Thread(target=send_data, )
        thread_send.start()
        thread_recv = threading.Thread(target=recv_data, args=(detail_url_queue_1,))
        thread_recv.start()
        thread_print = threading.Thread(target=print_data, args=(detail_url_queue_1,))
        thread_print.start()

    except KeyboardInterrupt:
        print('关闭')
        s.close()