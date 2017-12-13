#!/usr/bin/python3

import asyncio
import sys
import argparse
from datetime import datetime

from data_collection import *
from create_socket import create_socket

@asyncio.coroutine
def print_data(ferm_data, lock, period):
    with (yield from lock):
        print(str(int(datetime.now().timestamp())) + "," + "%.2f" % ferm_data.wort_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_temperature.get_mean() + "," + "%.2f" % ferm_data.chamber_humidity.get_mean() + "," + "%.2f" % ferm_data.wort_density.get_mean() + "," + ("1" if ferm_data.cooling_status else "0"))
    sys.stdout.flush()
    yield from asyncio.sleep(period)
    asyncio.ensure_future(print_data(ferm_data, lock, period))

def main():
    parser = argparse.ArgumentParser(description='Data Collector for Fermentation Controller.')
    parser.add_argument('-p', '--period', help='Period in seconds', required=True)

    args = parser.parse_args()

    period = float(args.period)

    ferm_data = FermData()

    loop = asyncio.get_event_loop()

    lock = asyncio.Lock(loop=loop)

    wt_transport, wt_protocol = loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(MCAST_GRP, THERMOCOUPLE_PORT)))
    wt_protocol.setup(ferm_data, lock)
    cth_transport, cth_protocol = loop.run_until_complete(loop.create_datagram_endpoint(ChamberTemperatureHumidityProtocol, sock=create_socket(MCAST_GRP, CHAMBER_TEMP_HUM_PORT)))
    cth_protocol.setup(ferm_data, lock)
    wd_transport, wd_protocol = loop.run_until_complete(loop.create_datagram_endpoint(WortDensityProtocol, sock=create_socket(MCAST_GRP, DENSITY_METER_PORT)))
    wd_protocol.setup(ferm_data, lock)
    cs_transport, cs_protocol = loop.run_until_complete(loop.create_datagram_endpoint(CoolingStatusProtocol, sock=create_socket(MCAST_GRP, COOLING_STATUS_PORT)))
    cs_protocol.setup(ferm_data, lock)
    loop.run_until_complete(print_data(ferm_data, lock, period))

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass

    wt_transport.close()
    cth_transport.close()
    wd_transport.close()
    cs_transport.close()
    loop.close()

main()
