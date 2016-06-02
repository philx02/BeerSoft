#!/bin/bash
cd /home/pi/KegeratorMonitor
export LD_LIBRARY_PATH=.
export I2C_BUS_PATH=/dev/i2c-0
export FLOW_METER_0_GPIO_PATH=/sys/class/gpio/gpio30/value
export FLOW_METER_1_GPIO_PATH=/sys/class/gpio/gpio31/value
./KegeratorEmbeddedServer data/
