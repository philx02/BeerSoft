#!/usr/bin/python3

import asyncio
from websocketserver import WebSocketServer
from brewcontrol import BrewerControl

class BrewerServer:
    def __init__(self, brewer_control):
        self.brewer_control = brewer_control
        self.lock = asyncio.Lock()
        self.producer_condition = asyncio.Condition(lock=self.lock) 

    @asyncio.coroutine
    def init(self, websocket):
        with (yield from self.lock):
            message = self.brewer_control.serialize()
        yield from websocket.send(message)

    @asyncio.coroutine
    def produce(self, websocket):
        with (yield from self.producer_condition):
            yield from self.producer_condition.wait()
            message = self.brewer_control.serialize()
        yield from websocket.send(message)
    
    @asyncio.coroutine
    def consume(self, websocket):
        message = yield from websocket.recv()
        proto = message.split(",")
        with (yield from self.lock):
            if len(proto) == 2 and proto[0] == "set_temperature_command":
                self.brewer_control.set_temperature_command(int(proto[1]))
            elif len(proto) == 2 and proto[0] == "set_heater_mode":
                self.brewer_control.set_heater_mode(int(proto[1]))
            elif len(proto) == 2 and proto[0] == "set_pump_mode":
                self.brewer_control.set_pump_mode(int(proto[1]))
            elif len(proto) == 2 and proto[0] == "set_pump_until_command":
                self.brewer_control.set_pump_until_command(int(proto[1]))
            elif len(proto) == 2 and proto[0] == "set_duty_cycle_command":
                self.brewer_control.set_duty_cycle_command(int(proto[1]))
            elif len(proto) == 2 and proto[0] == "set_sparge_heater_mode":
                self.brewer_control.set_sparge_heater_mode(int(proto[1]))
        yield from websocket.send(self.brewer_control.serialize())

    @asyncio.coroutine
    def notify_clients(self):
        with (yield from self.producer_condition):
            self.producer_condition.notify_all()
        yield from asyncio.sleep(1)
        asyncio.ensure_future(self.notify_clients())

class TemperatureProtocol:
    def __init__(self):
        self.brewer_control = None
        self.lock = None
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport
    
    def setup(self, brewer_control, lock):
        self.brewer_control = brewer_control
        self.lock = lock

    def datagram_received(self, data, addr):
        asyncio.ensure_future(self.set_data(data))
    
    @asyncio.coroutine
    def set_data(self, data):
        with (yield from self.lock):
            self.brewer_control.tick(float(data.decode("utf-8")))

def main():
    loop = asyncio.get_event_loop()
    
    brewer_server = BrewerServer(BrewerControl(10))
    
    wt_transport, wt_protocol = loop.run_until_complete(loop.create_datagram_endpoint(TemperatureProtocol, local_addr=('0.0.0.0', 10000)))
    wt_protocol.setup(brewer_server.brewer_control, brewer_server.lock)

    websocket_server = WebSocketServer(brewer_server.init, brewer_server.consume, brewer_server.produce, 8010)
    loop.run_until_complete(websocket_server.start_server)

    asyncio.ensure_future(brewer_server.notify_clients())

    loop.run_forever()

main()
