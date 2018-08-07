#!/usr/bin/python3

import asyncio
from websocketserver import WebSocketServer
from create_socket import create_socket
from data_collection import *

class KegeratorServer:
    def __init__(self):
        self.lock = asyncio.Lock()
        self.producer_condition = asyncio.Condition(lock=self.lock)
        self.kegerator_data = KegeratorData()

    @asyncio.coroutine
    def init(self, websocket):
        with (yield from self.lock):
            message = self.kegerator_data.serialize()
        yield from websocket.send(message)

    @asyncio.coroutine
    def produce(self, websocket):
        with (yield from self.producer_condition):
            yield from self.producer_condition.wait()
            message = self.kegerator_data.serialize()
        yield from websocket.send(message)

    @asyncio.coroutine
    def consume(self, websocket):
        message = yield from websocket.recv()
        if message == "get_status":
			with (yield from self.lock):
				message = self.kegerator_data.serialize()
			yield from websocket.send(message)

    @asyncio.coroutine
    def notify_clients(self):
        with (yield from self.producer_condition):
            self.producer_condition.notify_all()
        yield from asyncio.sleep(1)
        asyncio.ensure_future(self.notify_clients())

def main():
    loop = asyncio.get_event_loop()
    kegerator_server = KegeratorServer()

    t_transport, t_protocol = loop.run_until_complete(loop.create_datagram_endpoint(TemperatureProtocol, sock=create_socket(MCAST_GRP, THERMOMETER_PORT)))
    t_protocol.setup(ferm_server.ferm_data, ferm_server.lock)
    lc_transport, lc_protocol = loop.run_until_complete(loop.create_datagram_endpoint(LoadCellProtocol, sock=create_socket(MCAST_GRP, LOAD_CELL_PORT)))
    lc_protocol.setup(ferm_server.ferm_data, ferm_server.lock)
    k1_transport, k1_protocol = loop.run_until_complete(loop.create_datagram_endpoint(Keg1LevelProtocol, sock=create_socket(MCAST_GRP, KEG1_LEVEL_PORT)))
    k1_protocol.setup(ferm_server.ferm_data, ferm_server.lock)
    k2_transport, k2_protocol = loop.run_until_complete(loop.create_datagram_endpoint(Keg2LevelProtocol, sock=create_socket(MCAST_GRP, KEG2_LEVEL_PORT)))
    k2_protocol.setup(ferm_server.ferm_data, ferm_server.lock)

    websocket_server = WebSocketServer(ferm_server.init, ferm_server.consume, ferm_server.produce, 8011)
    loop.run_until_complete(websocket_server.start_server)

    asyncio.ensure_future(ferm_server.notify_clients())

    asyncio.get_event_loop().run_forever()

main()
