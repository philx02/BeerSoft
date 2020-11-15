#!/bin/bash
while [ ! -f data/high_threshold.txt ]
do
  echo "waiting for mount"
  sleep 1
done
screen -S thermocouple -d -m ./thermocouple.py
screen -S temp_sensor -d -m ./temp_sensor.py -g 17
high_threshold=`cat data/high_threshold.txt`
screen -S ctrl -d -m ./ctrl.py -g 16 -t $high_threshold
screen -S server -d -m ./server.py
