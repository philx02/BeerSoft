import asyncio

from accumulator import Accumulator

MCAST_GRP = '224.0.0.3'
THERMOMETER_PORT = 10000
LOAD_CELL_PORT = 10001
KEG0_LEVEL_PORT = 10002
KEG1_LEVEL_PORT = 10003
ML_PER_TICKS = 1.2
TICKS_PER_KEG = 19000 / ML_PER_TICKS
PCT_TO_TICKS = round(TICKS_PER_KEG / 100)

class KegLevel:
    def __init__(self, id):
        self.level_file = "keg_level/keg" + str(id) + ".txt"
        self.level = int(open(self.level_file).read())
        self.last_count = 0
        self.init = False
    
    def set_level_pct(self, value):
        self.level = value * PCT_TO_TICKS
        open(self.level_file, "w").write(str(self.level))

class KegeratorData:
    def __init__(self):
        self.temperature = Accumulator(60)
        self.co2_level = Accumulator(60)
        self.kegs = [KegLevel(0), KegLevel(1)]
    
    def serialize(self):
        co2_level = self.co2_level.get_mean() * 0.869565217 + 47.82608696
        keg0_level = (self.kegs[0].level / TICKS_PER_KEG)
        keg1_level = (self.kegs[1].level / TICKS_PER_KEG)
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
            if decrement != 0:
                self.data.kegs[0].level -= decrement
                open(self.data.kegs[0].level_file, "w").write(str(self.data.kegs[0].level))
        else:
            self.data.kegs[0].init = True
        self.data.kegs[0].last_count = count
        
class Keg2LevelProtocol(GenericProtocol):
    def set_the_data(self, data):
        count = int(data.decode())
        if self.data.kegs[1].init:
            decrement = count - self.data.kegs[1].last_count
            if decrement != 0:
                self.data.kegs[1].level -= decrement
                open(self.data.kegs[1].level_file, "w").write(str(self.data.kegs[1].level))
        else:
            self.data.kegs[1].init = True
        self.data.kegs[1].last_count = count