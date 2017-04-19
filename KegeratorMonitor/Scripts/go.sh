#!/bin/bash
screen -d -m /home/pi/KegeratorMonitor/server.sh
screen -d -m /home/pi/KegeratorMonitor/delayed_display.sh
screen -d -m /home/pi/KegeratorMonitor/MonitorOnOffControl.sh
