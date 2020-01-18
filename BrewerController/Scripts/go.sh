#!/bin/bash
screen -S temp_sensor -d -m /home/chip/BrewerController/temp_sensor.sh
screen -S main -d -m /home/chip/BrewerController/main.sh
