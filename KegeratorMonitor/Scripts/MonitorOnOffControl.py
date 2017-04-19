import socket
import struct
import sys

if len(sys.argv) != 2:
  print "Missing GPIO path."
  sys.exit(1)

gpio = open(sys.argv[1], "w+")

MCAST_GRP = '224.0.0.1'
MCAST_PORT = 14500

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((MCAST_GRP, MCAST_PORT))  # use MCAST_GRP instead of '' to listen only
                             # to MCAST_GRP, not all groups on MCAST_PORT
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

hysteresis = 10
gpio_value = -1

while True:
  frame = sock.recv(10240)
  if len(frame) > 0:
    if frame[0] == '1':
      hysteresis += 1
      if hysteresis >= 10:
        if gpio_value != 1:
          gpio_value = 1
          gpio.seek(0)
          gpio.write('0')
          gpio.flush
        hysteresis = 10 
    elif frame[0] == '0':
      hysteresis -= 1
      if hysteresis <= 0:
        if gpio_value != 0:
          gpio_value = 0
          gpio.seek(0)
          gpio.write('1')
          gpio.flush
        hysteresis = 0
