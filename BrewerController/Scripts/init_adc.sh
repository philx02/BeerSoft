device_path=/sys/bus/i2c/devices/i2c-1/1-0068/iio:device0/in_voltage3_scale

echo mcp3424 0x68 > /sys/bus/i2c/devices/i2c-1/new_device

while [ ! -f "$device_path" ]
do
  sleep 0.1
done

echo 0.000125000 > /sys/bus/i2c/devices/i2c-1/1-0068/iio:device0/in_voltage3_scale
