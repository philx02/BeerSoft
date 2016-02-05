#!/bin/bash
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
export LD_LIBRARY_PATH=.
export I2C_BUS_PATH=/dev/i2c-0
export FLOW_METER_0_GPIO_PATH=/sys/class/gpio/gpio30/value
export FLOW_METER_1_GPIO_PATH=/sys/class/gpio/gpio31/value
./KegaratorMonitor

