import asyncio

from accumulator import Accumulator

MCAST_GRP = '224.0.0.3'
THERMOMETER_PORT = 10000
LOAD_CELL_PORT = 10001
KEG0_LEVEL_PORT = 10002
KEG1_LEVEL_PORT = 10003

class KegLevel:
    def __init__(self):
        self.level = 0
        self.last_count = 0
        self.init = False
    
    def set_level_pct(self, value):
        self.level = value * 126

class KegeratorData:
    def __init__(self):
        self.temperature = Accumulator(60)
        self.co2_level = Accumulator(60)
        self.kegs = [KegLevel(), KegLevel()]
    
    def serialize(self):
        co2_level = (self.co2_level.get_mean() - 102) / 60
        keg0_level = (self.kegs[0].level / 12600)
        keg1_level = (self.kegs[1].level / 12600)
        return "%.2f" % self.temperature.get_mean() + ",0,%.2f" % co2_level + "," + str(keg0_level) + "," + str(keg1_level)
        
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
        self.data.temperature.add(float(data.decode()))

class LoadCellProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.co2_level.add(float(data.decode()))
        
class Keg1LevelProtocol(GenericProtocol):
    def set_the_data(self, data):
        count = int(data.decode())
        if self.data.kegs[0].init:
            decrement = count - self.data.kegs[0].last_count
            self.data.kegs[0].level -= decrement
        else:
            self.data.kegs[0].init = True
        self.data.kegs[0].last_count = count
        
class Keg2LevelProtocol(GenericProtocol):
    def set_the_data(self, data):
        count = int(data.decode())
        if self.data.kegs[1].init:
            decrement = count - self.data.kegs[1].last_count
            self.data.kegs[1].level -= decrement
        else:
            self.data.kegs[1].init = True
        self.data.kegs[1].last_count = count