#!/bin/bash
cd /home/pi/KegeratorMonitor
export LD_LIBRARY_PATH=.
export I2C_BUS_PATH=/dev/i2c-1
export FLOW_METER_0_GPIO_PATH=/sys/class/gpio/gpio17/value
export FLOW_METER_1_GPIO_PATH=/sys/class/gpio/gpio27/value
export CO2_METER_MOVING_AVERAGE_WINDOW=300
./KegeratorEmbeddedServer -d data/ -p 8011
