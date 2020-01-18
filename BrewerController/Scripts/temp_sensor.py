#!/usr/bin/python3

from max31865 import max31865
import socket
import time

device = max31865()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

running_sum = 0
values = []

while True:
    value = device.readTemp()
    factor = 0.0933931624 * value + 17.81623932
    value += factor
    running_sum += value
    values.append(value)
    if len(values) > 10:
        running_sum -= values[0]
        values.pop(0)
    sock.sendto(bytes(str(running_sum/len(values)), "utf-8"), ("localhost", 10000))
    time.sleep(1)
