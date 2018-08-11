#!/usr/bin/python2

import threading
import socket
import time
import argparse
import RPi.GPIO as gpio

PARSER = argparse.ArgumentParser(description='GPIO counter.')
PARSER.add_argument('--udp_port', help='UDP port')
PARSER.add_argument('--gpio', help='GPIO pin number.')
ARGS = PARSER.parse_args()

pin = int(ARGS.gpio)

gpio.setmode(gpio.BCM)
gpio.setup(pin, gpio.IN, pull_up_down=gpio.PUD_UP)

current_count = 0
lock = threading.Lock()

MCAST_GRP = "224.0.0.3"
MCAST_PORT = int(ARGS.udp_port)

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def tick(channel):
    global current_count
    with lock:
        current_count += 1

gpio.add_event_detect(pin, gpio.RISING, callback=tick, bouncetime=300)

while True:
    with lock:
        udp_socket.sendto(str(current_count), (MCAST_GRP, MCAST_PORT)) 
    print(str(current_count))
    time.sleep(1)