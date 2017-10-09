#!/bin/bash
cd /home/chip/BrewerController/
export LD_LIBRARY_PATH=.
./Mcp3424Sensor -p /dev/i2c-2 -c calib.csv -m 1000 >/dev/udp/127.0.0.1/10000
