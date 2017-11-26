#!/usr/bin/python2
import socket
import struct
from datetime import datetime
import sys

MCAST_GRP = '224.0.0.2'
MCAST_PORT = 10000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((MCAST_GRP, MCAST_PORT))
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

while True:
    data = sock.recv(1024) # buffer size is 1024 bytes
    print "[" + datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "] " + data
    sys.stdout.flush()
