import asyncio
import socket
import struct

from accumulator import Accumulator

MCAST_GRP = '224.0.0.2'
THERMOCOUPLE_PORT = 10000
CHAMBER_TEMP_HUM_PORT = 10001
DENSITY_METER_PORT = 10002
COOLING_STATUS_PORT = 10003

class FermData:
    def __init__(self):
        self.wort_temperature = Accumulator(60)
        self.chamber_temperature = Accumulator(60)
        self.chamber_humidity = Accumulator(60)
        self.wort_density = Accumulator(60)
        self.cooling_status = False

ferm_data = FermData()

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
        with (yield from lock):
            self.set_the_data(data)

class WortTemperatureProtocol(GenericProtocol):
    def set_the_data(self, data):
        global ferm_data
        ferm_data.wort_temperature.add(float(data.decode()))

class ChamberTemperatureHumidityProtocol(GenericProtocol):
    def set_the_data(self, data):
        global ferm_data
        split = data.decode().split(",")
        if len(split) == 2:
            ferm_data.chamber_temperature.add(float(split[0]))
            ferm_data.chamber_humidity.add(float(split[1]))

class WortDensityProtocol(GenericProtocol):
    def set_the_data(self, data):
        global ferm_data
        ferm_data.wort_density.add(float(data.decode()))

class CoolingStatusProtocol(GenericProtocol):
    def set_the_data(self, data):
        global ferm_data
        ferm_data.cooling_status = data.decode() != "0"