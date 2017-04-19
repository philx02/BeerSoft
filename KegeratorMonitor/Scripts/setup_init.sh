#!/bin/bash
cd /home/pi/KegeratorMonitor
if [ ! -f /sys/class/gpio/gpio17/value ]; then
  echo 17 > /sys/class/gpio/export
  sleep 0.5
  echo rising > /sys/class/gpio/gpio17/edge
fi
if [ ! -f /sys/class/gpio/gpio27/value ]; then
  echo 27 > /sys/class/gpio/export
  sleep 0.5
  echo rising > /sys/class/gpio/gpio27/edge
fi
if [ ! -f /sys/class/gpio/gpio21/value ]; then
  echo 21 > /sys/class/gpio/export
  sleep 0.5
  echo 1 > /sys/class/gpio/gpio21/active_low
  echo 0 > /sys/class/gpio/gpio21/value
  echo out > /sys/class/gpio/gpio21/direction
fi
