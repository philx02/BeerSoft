#!/bin/bash
export DISPLAY=:0
while ! xset q &>/dev/null; do
  sleep 1
done
#sleep 100
echo starting...
iceweasel http://spoluck/kegerator?server=ws://localhost:8000
