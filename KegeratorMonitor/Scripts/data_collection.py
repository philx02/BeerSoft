import asyncio

from accumulator import Accumulator

MCAST_GRP = '224.0.0.3'
THERMOMETER_PORT = 10000
LOAD_CELL_PORT = 10001
KEG1_LEVEL_PORT = 10002
KEG2_LEVEL_PORT = 10003

class KegeratorData:
    def __init__(self):
        self.temperature = Accumulator(60)
        self.co2_level = Accumulator(60)
        self.keg1_level = 0
        self.keg2_level = 0
    
    def serialize(self):
        return "%.2f" % self.temperature.get_mean() + ",0,%.2f" % self.co2_level.get_mean() + ",%.2f" % self.keg1_level + ",%.2f" % self.keg2_level
		
class GenericProtocol(asyncio.DatagramProtocol):
    def __init__(self):
        self.data = None
        self.lock = None
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport

    def datagram_received(self, data, addr):
        asyncio.ensure_future(self.__set_data(data))

    def set_the_data(self, data):
        raise NotImplementedError()

    def setup(self, data, lock):
        self.data = data
        self.lock = lock

    @asyncio.coroutine
    def __set_data(self, data):
        with (yield from self.lock):
            self.set_the_data(data)

class TemperatureProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.wort_temperature.add(float(data.decode()))

class LoadCellProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.co2_level.add(float(data.decode()))
		
class Keg1LevelProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.keg1_level = int(data.decode())
		
class Keg2LevelProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.keg2_level = int(data.decode())