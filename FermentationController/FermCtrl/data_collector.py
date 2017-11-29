#!/usr/bin/python3

import asyncio
import sys
import argparse
from datetime import datetime

from data_collection import *

parser = argparse.ArgumentParser(description='Data Collector for Fermentation Controller.')
parser.add_argument('-p', '--period', help='Period in seconds', required=True)

args = parser.parse_args()

PERIOD = float(args.period)

@asyncio.coroutine
def print_data():
    global ferm_data
    with (yield from lock):
        print(str(int(datetime.now().timestamp())) + "," + "%.2f" % ferm_data.wort_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_humidity.get_mean() + "," + "%.2f" % ferm_data.wort_density.get_mean() + "," + ("1" if ferm_data.cooling_status else "0"))
    sys.stdout.flush()
    yield from asyncio.sleep(PERIOD)
    asyncio.ensure_future(print_data())

wort_temperature_transport = loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(THERMOCOUPLE_PORT)))
chamber_temperature_humidity_transport = loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(CHAMBER_TEMP_HUM_PORT)))
wort_densit_transporty = loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(DENSITY_METER_PORT)))
cooling_status_transport = loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(COOLING_STATUS_PORT)))
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
