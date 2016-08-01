import socket
import OSC


ip = '127.0.0.1'
port = 9996
buffer_size = 1024
do_loop = True
 
client = OSC.OSCClient()
# Connect
my_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
 
try:
    my_socket.bind((ip, port))
    my_socket.setblocking(0)
    my_socket.settimeout(0.002)
    my_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, buffer_size)
    print 'Plug : IP = ', ip,  'Port = ', port,  'Buffer Size =', buffer_size
except:
    print 'No connected'

def handleData(data):
    if data[0] == "myDevice?namespace":
        msg = OSC.OSCMessage()
        if data[2] == '/':
            msg.setAddress('myDevice:namespace')
            msg.append('/')
            msg.append('Application')
            msg.append('nodes={')
            msg.append('filter')
            msg.append('}')
        
        if data[2] == '/filter':
            msg.setAddress('myDevice:namespace')
            msg.append('/filter')
            msg.append('Data')
            msg.append('attributes={')
            msg.append('type')
            msg.append('value')
            msg.append('}')
            
        client.sendto(msg, ('127.0.0.1', 9997))
        msg.clearData()
         
# If connected
while do_loop:
    try:
        raw_data = my_socket.recv(buffer_size)
        data = OSC.decodeOSC(raw_data)
        print(data)
        handleData(data)
    except socket.error:
        pass
