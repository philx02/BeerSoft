#!/usr/bin/python3

import socket
import struct
import sys
import os

MCAST_GRP = '224.0.0.1'
MCAST_PORT = 14500

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((MCAST_GRP, MCAST_PORT))  # use MCAST_GRP instead of '' to listen only
                             # to MCAST_GRP, not all groups on MCAST_PORT
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

hysteresis = 10

while True:
  frame = sock.recv(10240)
  if len(frame) > 0:
    if frame[0] == '1':
      hysteresis += 1
      if hysteresis >= 10:
        os.system('vcgencmd display_power 1')
        hysteresis = 10 
    elif frame[0] == '0':
      hysteresis -= 1
      if hysteresis <= 0:
        os.system('vcgencmd display_power 0')
        hysteresis = 0
