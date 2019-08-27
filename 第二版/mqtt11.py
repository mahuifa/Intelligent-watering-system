import paho.mqtt.client as mqtt
import time
import smbus

address = 0x48
A0 = 0x40
A1 = 0x41
A2 = 0x42
A3 = 0x43
P4 = 0x44
P5 = 0x45
bus = smbus.SMBus(1)

HOST = "222.198.208.190"
PORT = 1883
client = mqtt.Client()
client.connect(HOST, PORT, 60)


def get_data():
    while True:
        bus.write_byte(address, P4)
        value4 = 255 - bus.read_byte(address)
        #print ('P4:' + str(value4))

        bus.write_byte(address, A1)
        value1 = 255 - bus.read_byte(address)
        a1 = '.'
        a2 = ('0x01','2016042276', str(value1))
        a3 = a1.join(a2)
        client.publish("data1", a3)
        print ('L:'+a3)
        time.sleep(0.5)

        bus.write_byte(address,A2)
        value2 = 255 - bus.read_byte(address)
        a4 = ('0x02','2016042276',str(value2))
        a5 = a1.join(a4)
        client.publish("data1", a5)
        print('C:'+a5)
        time.sleep(0.5)




if __name__ == '__main__':
    get_data()
    client.loop_forever()