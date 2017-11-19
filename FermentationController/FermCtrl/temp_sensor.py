import Adafruit_DHT
import argparse
import socket

MCAST_GRP = "224.0.0.2"
MCAST_PORT = 10001

parser = argparse.ArgumentParser(description='Temperature sensing.')
parser.add_argument('-g', '--gpio', help='Gpio pin', required=True)

args = parser.parse_args()

sensor = Adafruit_DHT.DHT22

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    humidity, temperature = Adafruit_DHT.read_retry(sensor, args.gpio)
    if humidity is not None and temperature is not None:
        message = format(temperature, '.2f') + "," + format(humidity, '.2f')
        udp_socket.sendto(message, (MCAST_GRP, MCAST_PORT))
