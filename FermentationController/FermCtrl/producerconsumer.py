import asyncio
from datetime import datetime

from data_collection import *

producer_condition = asyncio.Condition(lock=lock, loop=loop)

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

historical_data = []
historical_lock = asyncio.Lock(loop=loop)

@asyncio.coroutine
def produce(websocket):
    with (yield from producer_condition):
        yield from producer_condition.wait()
        message = "now_data|%.2f" % ferm_data.wort_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_humidity.get_mean() + "," + "%.2f" % ferm_data.wort_density.get_mean() + "," + ("1" if ferm_data.cooling_status else "0")
    yield from websocket.send(message)

@asyncio.coroutine
def consume(websocket):
    message = yield from websocket.recv()
    if message == "get_historical_data":
        response = "historical_data|"
        with (yield from historical_lock):
            for data_point in historical_data:
                response += data_point.serialize() + ";"
        yield from websocket.send(response)

@asyncio.coroutine
def notify_clients():
    with (yield from producer_condition):
        producer_condition.notify_all()
    yield from asyncio.sleep(1)
    asyncio.ensure_future(notify_clients())

@asyncio.coroutine
def store_historical_data():
    yield from asyncio.sleep(60)
    with (yield from lock):
        with (yield from historical_lock):
            historical_data.append(FermDataSnapshot(int(datetime.now().timestamp()), ferm_data))
    asyncio.ensure_future(store_historical_data())

loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(THERMOCOUPLE_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(CHAMBER_TEMP_HUM_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(DENSITY_METER_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(COOLING_STATUS_PORT)))
asyncio.ensure_future(notify_clients())
asyncio.ensure_future(store_historical_data())
