#!/bin/bash
export DISPLAY=:0
while ! xset q &>/dev/null; do
  sleep 1
done
echo starting...
/home/pi/KegeratorMonitor/display.sh
