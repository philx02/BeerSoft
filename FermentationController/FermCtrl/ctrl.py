#!/usr/bin/python2

import socket
import struct
import argparse

import RPi.GPIO as GPIO
from temp_control import TemperatureControl

parser = argparse.ArgumentParser(description='Temperature sensing.')
parser.add_argument('-l', '--low_threshold', help='Low threshold', required=True)
parser.add_argument('-i', '--high_threshold', help='High threshold', required=True)

args = parser.parse_args()

GPIO.setmode(GPIO.BCM)

TEMPERATURE_CONTROL = TemperatureControl(gpio=16, low_threshold=args.low_threshold, high_threshold=args.high_threshold)

MCAST_GRP = '224.0.0.2'
MCAST_PORT = 10000

SOCK = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
SOCK.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
SOCK.bind((MCAST_GRP, MCAST_PORT))
MREQ = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
SOCK.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, MREQ)

while True:
    data = SOCK.recv(32)
    TEMPERATURE_CONTROL.update(float(data))
