import asyncio

from data_collection import *

def producer():
    return "%.2f" % wort_temperature.get_mean() + "," + "%.2f" % chamber_temperature.get_mean() + "," + "%.2f" % chamber_humidity.get_mean() + "," + "%.2f" % wort_density.get_mean() + "," + ("1" if cooling_status else "0")

def consumer(message):
    proto = message.split(",")

loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(THERMOCOUPLE_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(CHAMBER_TEMP_HUM_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(DENSITY_METER_PORT)))
loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(COOLING_STATUS_PORT)))