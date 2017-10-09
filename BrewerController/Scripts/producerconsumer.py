import asyncio
from brewcontrol import BrewerControl

BC = BrewerControl(10)

def producer():
    return BC.serialize()

def consumer(message):
    proto = message.split(",")
    if len(proto) == 2 and proto[0] == "set_temperature_command":
        BC.set_temperature_command(int(proto[1]))
    elif len(proto) == 2 and proto[0] == "set_heater_mode":
        BC.set_heater_mode(int(proto[1]))
    elif len(proto) == 2 and proto[0] == "set_pump_mode":
        BC.set_pump_mode(int(proto[1]))
    elif len(proto) == 2 and proto[0] == "set_pump_until_command":
        BC.set_pump_until_command(int(proto[1]))
    elif len(proto) == 2 and proto[0] == "set_duty_cycle_command":
        BC.set_duty_cycle_command(int(proto[1]))

class TemperatureProtocol:
    def connection_made(self, transport):
        print("connection_made")
        self.transport = transport

    def datagram_received(self, data, addr):
        global BC
        BC.tick(float(data.decode("utf-8")))

udp_server = asyncio.get_event_loop().create_datagram_endpoint(TemperatureProtocol, local_addr=('0.0.0.0', 10000))

asyncio.get_event_loop().run_until_complete(udp_server)