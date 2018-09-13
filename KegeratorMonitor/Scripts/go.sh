#!/bin/bash
screen -d -S server -m /home/pi/KegeratorMonitor/server.sh
screen -d -S display -m /home/pi/KegeratorMonitor/display-kiosk.sh
screen -d -S on_off -m /home/pi/KegeratorMonitor/MonitorOnOffControl.sh
