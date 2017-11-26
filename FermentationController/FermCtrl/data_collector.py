#!/usr/bin/python3
import socket
import struct
import asyncio
import sys
from datetime import datetime
from accumulator import Accumulator

MCAST_GRP = '224.0.0.2'
THERMOCOUPLE_PORT = 10000
CHAMBER_TEMP_HUM_PORT = 10001
DENSITY_METER_PORT = 10002
COOLING_STATUS_PORT = 10003

wort_temperature = Accumulator(60)
chamber_temperature = Accumulator(60)
chamber_humidity = Accumulator(60)
wort_density = Accumulator(60)
cooling_status = False

loop = asyncio.get_event_loop()

lock = asyncio.Lock(loop=loop)

def create_socket(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((MCAST_GRP, port))
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY))
    return sock

def create_socket_win(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    group = socket.inet_aton(MCAST_GRP)
    iface = socket.inet_aton('192.168.0.5') # listen for multicast packets on this interface.
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, group+iface)
    #sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY))
    sock.bind(('', port))
    return sock

class GenericProtocol(asyncio.DatagramProtocol):
    def connection_made(self, transport):
        self.transport = transport

    def datagram_received(self, data, addr):
        global loop
        asyncio.ensure_future(self.__set_data(data), loop=loop)
    
    def set_the_data(self, data):
        raise NotImplementedError()

    @asyncio.coroutine
    def __set_data(self, data):
        global lock
        yield from lock
        try:
            self.set_the_data(data)
        finally:
            lock.release()

class WortTemperatureProtocol(GenericProtocol):
    def set_the_data(self, data):
        global wort_temperature
        wort_temperature.add(float(data.decode()))

class ChamberTemperatureHumidityProtocol(GenericProtocol):
    def set_the_data(self, data):
        global chamber_temperature, chamber_humidity
        split = data.decode().split(",")
        if len(split) == 2:
            chamber_temperature.add(float(split[0]))
            chamber_humidity.add(float(split[1]))

class WortDensityProtocol(GenericProtocol):
    def set_the_data(self, data):
        global wort_density
        wort_density.add(float(data.decode()))

class CoolingStatusProtocol(GenericProtocol):
    def set_the_data(self, data):
        global cooling_status
        cooling_status = data.decode() != "0"

@asyncio.coroutine
def print_data():
    while True:
        yield from lock
        try:
            print(datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "," + str(wort_temperature.get_mean()) + "," + str(chamber_temperature.get_mean()) + "," + str(chamber_humidity.get_mean()) + "," + str(wort_density.get_mean()) + "," + ("1" if cooling_status else "0"))
        finally:
            lock.release()
        sys.stdout.flush()
        yield from asyncio.sleep(1)

wort_temperature_transport = loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket_win(THERMOCOUPLE_PORT)))
chamber_temperature_humidity_transport = loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket_win(CHAMBER_TEMP_HUM_PORT)))
wort_densit_transporty = loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket_win(DENSITY_METER_PORT)))
cooling_status_transport = loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket_win(COOLING_STATUS_PORT)))
loop.run_until_complete(print_data())

try:
    loop.run_forever()
except KeyboardInterrupt:
    pass

wort_temperature_transport.close()
chamber_temperature_humidity_transport.close()
wort_densit_transporty.close()
cooling_status_transport.close()
loop.close()
