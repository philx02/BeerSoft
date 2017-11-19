#!/usr/bin/python

import time
import argparse
import socket

import Adafruit_GPIO.SPI as SPI
import MAX6675.MAX6675 as MAX6675

MCAST_GRP = "224.0.0.2"
MCAST_PORT = 10000

SPI_PORT   = 0
SPI_DEVICE = 0
sensor = MAX6675.MAX6675(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    temp = sensor.readTempC() - 2 # Calibrated at -2C
    message = format(temp, '.2f')
    udp_socket.sendto(message, (MCAST_GRP, MCAST_PORT))
    time.sleep(1.0)
