#!/bin/bash
screen -d -S load_cell -m /home/pi/KegeratorMonitor/auto_restart.sh /home/pi/KegeratorMonitor/load_cell.py
screen -d -S temp_sensor -m /home/pi/KegeratorMonitor/auto_restart.sh /home/pi/KegeratorMonitor/temp_sensor.py
screen -d -S pulse_counter_0 -m /home/pi/KegeratorMonitor/auto_restart.sh /home/pi/KegeratorMonitor/pulse_counter_0.sh
screen -d -S pulse_counter_1 -m /home/pi/KegeratorMonitor/auto_restart.sh /home/pi/KegeratorMonitor/pulse_counter_1.sh
screen -d -S server -m /home/pi/KegeratorMonitor/server_py.sh
screen -d -S display -m /home/pi/KegeratorMonitor/display-kiosk.sh
screen -d -S on_off -m /home/pi/KegeratorMonitor/monitor_onoff_control.py
