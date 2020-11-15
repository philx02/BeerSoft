#!/bin/bash
cd /home/pi/KegeratorMonitor
while true
do
  /home/pi/KegeratorMonitor/server.py
  sleep 1
done
