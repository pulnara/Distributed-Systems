# according to http://formacdownload.com/questions/4459/multicast-w-pythonie
import socket
import sys
import datetime
import struct

multicast_port = 9999
multicast_ip = '224.1.1.1'

logger_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
logger_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
logger_socket.bind((multicast_ip, multicast_port))

mreq = struct.pack("4sl", socket.inet_aton(multicast_ip), socket.INADDR_ANY)
logger_socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

logfile_base = 'token_ring_log'

if len(sys.argv) < 2:
    print("Provide log file index")
    sys.exit()

file_id = sys.argv[1]

while True:
    buff, address = logger_socket.recvfrom(1024)
    message = buff.decode('utf-8')
    print(buff)
    print("Received: {} from {}".format(message, address))

    with open(logfile_base + file_id, 'a+') as f:
        f.write("{} {}\n".format(datetime.datetime.now(), message))