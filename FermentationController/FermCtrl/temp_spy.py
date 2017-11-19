#!/usr/bin/python2
import socket
import struct
from datetime import datetime
import sys

UDP_IP = "0.0.0.0"
UDP_PORT = 10000

MCAST_GRP = '224.0.0.1'
MCAST_PORT = 10000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((MCAST_GRP, MCAST_PORT))  # use MCAST_GRP instead of '' to listen only
                             # to MCAST_GRP, not all groups on MCAST_PORT
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

#sock = socket.socket(socket.AF_INET, # Internet
#                     socket.SOCK_DGRAM) # UDP
#sock.bind((UDP_IP, UDP_PORT))

while True:
    data = sock.recv(1024) # buffer size is 1024 bytes
    print "[" + datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "] " + data
    sys.stdout.flush()
