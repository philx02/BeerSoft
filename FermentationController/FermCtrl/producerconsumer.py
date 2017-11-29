import asyncio

from data_collection import *

producer_condition = asyncio.Condition(lock=lock, loop=loop)

@asyncio.coroutine
def produce():
    with (yield from producer_condition.wait()):
        message = "%.2f" % ferm_data.wort_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_humidity.get_mean() + "," + "%.2f" % ferm_data.wort_density.get_mean() + "," + ("1" if ferm_data.cooling_status else "0")
    return message

@asyncio.coroutine
def consume(message):
    print(message.decode())

@asyncio.coroutine
def gather_data():
    while True:
        with (yield from lock):
            producer_condition.notify_all()
        yield from asyncio.sleep(1)

loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(THERMOCOUPLE_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(CHAMBER_TEMP_HUM_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(DENSITY_METER_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(COOLING_STATUS_PORT)))
