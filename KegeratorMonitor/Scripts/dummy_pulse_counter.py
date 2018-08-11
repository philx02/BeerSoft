#!/usr/bin/python3

import threading
import socket
import time
import argparse

PARSER = argparse.ArgumentParser(description='GPIO counter.')
PARSER.add_argument('--udp_port', help='UDP port')
ARGS = PARSER.parse_args()

current_count = 0

MCAST_GRP = "224.0.0.3"
MCAST_PORT = int(ARGS.udp_port)

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    input("Press Enter to continue...")
    current_count += 1
    udp_socket.sendto(str(current_count).encode(), (MCAST_GRP, MCAST_PORT)) 
    print(str(current_count))