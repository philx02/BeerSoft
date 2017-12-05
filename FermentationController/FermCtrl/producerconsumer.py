import asyncio
from datetime import datetime

from data_collection import *

PRODUCER_CONDITION = asyncio.Condition(lock=LOCK, loop=LOOP)

class FermDataSnapshot:
    def __init__(self, timestamp, ferm_data):
        self.timestamp = timestamp
        self.wort_temperature = ferm_data.wort_temperature.get_mean()
        self.chamber_temperature = ferm_data.chamber_temperature.get_mean()
        self.chamber_humidity = ferm_data.chamber_humidity.get_mean()
        self.wort_density = ferm_data.wort_density.get_mean()
        self.cooling_status = ferm_data.cooling_status
    
    def serialize(self):
        return str(self.timestamp) + ",%.2f" % self.wort_temperature + ",%.2f" % self.chamber_temperature + ",%.2f" % self.chamber_humidity + ",%.2f" % self.wort_density + "," + ("1" if ferm_data.cooling_status else "0")

HISTORICAL_DATA = []
HISTORICAL_LOCK = asyncio.Lock(loop=LOOP)

def generate_message(ferm_data):
    return "now_data|%.2f" % ferm_data.wort_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_humidity.get_mean() + "," + "%.2f" % ferm_data.wort_density.get_mean() + "," + ("1" if ferm_data.cooling_status else "0")

@asyncio.coroutine
def init(websocket):
    with (yield from LOCK):
        message = generate_message(FERM_DATA)
    yield from websocket.send(message)

@asyncio.coroutine
def produce(websocket):
    with (yield from PRODUCER_CONDITION):
        yield from PRODUCER_CONDITION.wait()
        message = generate_message(FERM_DATA)
    yield from websocket.send(message)

@asyncio.coroutine
def consume(websocket):
    message = yield from websocket.recv()
    if message == "get_historical_data":
        response = "historical_data|"
        with (yield from HISTORICAL_LOCK):
            for data_point in HISTORICAL_DATA:
                response += data_point.serialize() + ";"
        yield from websocket.send(response)

@asyncio.coroutine
def notify_clients():
    with (yield from PRODUCER_CONDITION):
        PRODUCER_CONDITION.notify_all()
    yield from asyncio.sleep(1)
    asyncio.ensure_future(notify_clients())

@asyncio.coroutine
def store_historical_data():
    yield from asyncio.sleep(60)
    with (yield from LOCK):
        with (yield from HISTORICAL_LOCK):
            HISTORICAL_DATA.append(FermDataSnapshot(int(datetime.now().timestamp()), FERM_DATA))
    asyncio.ensure_future(store_historical_data())

LOOP.run_until_complete(LOOP.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(THERMOCOUPLE_PORT)))
LOOP.run_until_complete(LOOP.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(CHAMBER_TEMP_HUM_PORT)))
LOOP.run_until_complete(LOOP.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(DENSITY_METER_PORT)))
LOOP.run_until_complete(LOOP.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(COOLING_STATUS_PORT)))
asyncio.ensure_future(notify_clients())
asyncio.ensure_future(store_historical_data())
