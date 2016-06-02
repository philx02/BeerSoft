#!/bin/bash
cd /home/pi/KegeratorMonitor
sudo ./EnableI2c0.py
if [ ! -f /sys/class/gpio/gpio30/value ]; then
  echo 30 > /sys/class/gpio/export
  sleep 0.5
  echo rising > /sys/class/gpio/gpio30/edge
fi
if [ ! -f /sys/class/gpio/gpio31/value ]; then
  echo 31 > /sys/class/gpio/export
  sleep 0.5
  echo rising > /sys/class/gpio/gpio31/edge
fi
