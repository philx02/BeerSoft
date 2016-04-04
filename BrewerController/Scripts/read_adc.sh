while true; do
cat /sys/bus/i2c/devices/1-0068/iio:device0/in_voltage3_raw
sleep 1
done
