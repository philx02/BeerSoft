import asyncio

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

class WortTemperatureProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.wort_temperature.add(float(data.decode()))

class ChamberTemperatureHumidityProtocol(GenericProtocol):
    def set_the_data(self, data):
        split = data.decode().split(",")
        if len(split) == 2:
            self.data.chamber_temperature.add(float(split[0]))
            self.data.chamber_humidity.add(float(split[1]))

class WortDensityProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.wort_density.add(float(data.decode()))

class CoolingStatusProtocol(GenericProtocol):
    def set_the_data(self, data):
        self.data.cooling_status = data.decode() != "0"