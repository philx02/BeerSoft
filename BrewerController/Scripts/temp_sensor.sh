#!/bin/bash
cd /home/chip/BrewerController/
export LD_LIBRARY_PATH=.
while true; do
  ./Mcp3424Sensor -p /dev/i2c-2 -c calib.csv -m 1000 -r 127.0.0.1 -o 10000
done
