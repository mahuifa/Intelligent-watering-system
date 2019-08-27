# _*_ coding.uft-8 _# _
#开发人 :马慧发
import socket
import time
import threading
import smbus
import socket
from queue import Queue
import RPi.GPIO as GPIO

time.sleep(10)

# BOARD编号方式，基于插座引脚编号
GPIO.setmode(GPIO.BOARD)
GPIO.setup(18, GPIO.OUT)

# 变量
a1 = 15
clientsocket = ''
msg = 0
x = 1

q1 = ''
q2 = ''
q3 = ''
q11 = ''
q21 = ''
q31 = ''
q4 = ''
q41 = ''

address = 0x48  ## address  ---> 器件的地址(硬件地址 由器件决定)
A0 = 0x40  ##  A0    ----> 器件某个端口的地址（数据存储的寄存器）
A1 = 0x41
A2 = 0x42
A3 = 0x43
P4 = 0x44
P5 = 0x45
bus = smbus.SMBus(1)  ## 开启总线

# 创建 socket 对象
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 绑定端口号
serversocket.bind(('192.168.43.192', 8002))
# 设置最大连接数，超过后排队
serversocket.listen(5)


def send_data():  #q11,q21,q31
    """发送信息"""
    global  clientsocket
    global  q1, q2, q11, q21, q3 , q31,q4,q41,a1
    print('send')
    while True:
        for i in range(8):
            # b = q3.gebt()
            if i == 0:
                clientsocket.send(q1.encode('utf-8'))
            elif i == 1:
                clientsocket.send(q11.encode('utf-8'))
            elif i == 2:
                clientsocket.send(q2.encode('utf-8'))
            elif i == 3:
                clientsocket.send(q21.encode('utf-8'))
            elif i == 4:
                clientsocket.send(q3.encode('utf-8'))
            elif i == 5:
                clientsocket.send(q31.encode('utf-8'))
            elif i == 6:
                a3 = '0x04'
                clientsocket.send(a3.encode('utf-8'))
            elif i == 7:
                a2 = str(a1)
                clientsocket.send(a2.encode('utf-8'))
            time.sleep(0.1)


def recv_data():
    """接收数据"""
    global a1,clientsocket
    while True:
        data = clientsocket.recv(1024)
        data_decode = data.decode('utf-8')
        j = int(data_decode)
        if (j > 10):
            a1 = j
        print(a1)
        if j == 0:
            GPIO.output(18, GPIO.HIGH)
            print ('浇水')
            time.sleep(1)
            GPIO.output(18, GPIO.LOW)
        else:
            GPIO.output(18, GPIO.LOW)


def get_data():  # q1,q11
    """获取数据"""
    global q1,q2,q3,q11,q21,q31,q4,q41
    while True:
        bus.write_byte(address, A2)  ## 告诉树莓派 你想获取那个器件的那个端口的数据
        q1 = '0x01'
        q_11 = 255 - bus.read_byte(address)  ## 获得数据
        q11 = str(q_11)
        bus.write_byte(address, P4)  ## 告诉树莓派 你想获取那个器件的那个端口的数据
        q2 = '0x02'
        q_21 = 255 - bus.read_byte(address)  ## 获得数据
        q21 = str(q_21)
        bus.write_byte(address, P5)  ## 告诉树莓派 你想获取那个器件的那个端口的数据
        q3 = '0x03'
        q_31 = 255 - bus.read_byte(address)  ## 获得数据
        q31 = str(q_31)
        time.sleep(0.8)



def print_data():  # q1,q2,q3
    """打印数据"""
    global data,q1,q2,q3,q11,q21,q31,q4,q41,a1
    #q_a = Queue(maxsize=1000)
    while True:
        a = q11
        b = q21
        c = q31
        print('温度:%s--湿度：%s--光照：%s--下限：%d' % (a,b,c,a1))
        time.sleep(0.8)

def output():                 # 浇水
    global q21,a1
    while True:
        a = int(q21)
        if (a <a1):
            GPIO.output(18, GPIO.HIGH)
            print ('浇水')
            time.sleep(1)
            GPIO.output(18, GPIO.LOW)


if __name__ == '__main__':
    try:
        thread_get = threading.Thread(target=get_data, )  # args=(q_1, q_11)
        thread_get.start()

        thread_print = threading.Thread(target=print_data,)  #,args=(q_1,q_2,q_3)
        thread_print.start()
        thread_output = threading.Thread(target=output, )  # ,args=(q_1,q_2,q_3)
        thread_output.start()

        while True:

            print('xxxx')
            clientsocket, addr = serversocket.accept()

            print("连接地址: %s" % str(addr))
            print(123)
            thread_send = threading.Thread(target=send_data, )  #  args=(q_11, q_21, q_31)
            thread_send.start()
            thread_recv = threading.Thread(target=recv_data, )
            thread_recv.start()




    except :
        print('断开')