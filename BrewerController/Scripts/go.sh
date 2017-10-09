#!/bin/bash
screen -d -m /home/chip/BrewerController/main.sh
sleep 5
screen -d -m /home/chip/BrewerController/temp_sensor.sh
