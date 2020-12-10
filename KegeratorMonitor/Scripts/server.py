#!/usr/bin/python3

import asyncio
from websocketserver import WebSocketServer
from create_socket import create_socket
from data_collection import *

class KegeratorServer:
    def __init__(self):
        self.lock = asyncio.Lock()
        self.producer_condition = asyncio.Condition(lock=self.lock)
        self.data = KegeratorData()

    async def init(self, websocket):
        async with self.lock:
            message = self.data.serialize()
        await websocket.send(message)

    async def produce(self, websocket):
        async with self.producer_condition:
            await self.producer_condition.wait()
            message = self.data.serialize()
        await websocket.send(message)

    async def consume(self, websocket):
        message = await websocket.recv()
        split = message.split("|")
        if len(split) == 3 and split[0] == "set_keg_level":
            async with self.lock:
                self.data.kegs[int(split[1])].set_level_pct(int(split[2]))
        async with self.lock:
            message = self.data.serialize()
        await websocket.send(message)


    async def notify_clients(self):
        async with self.producer_condition:
            self.producer_condition.notify_all()
        await asyncio.sleep(1)
        asyncio.ensure_future(self.notify_clients())

def main():
    loop = asyncio.get_event_loop()
    kegerator_server = KegeratorServer()

    t_transport, t_protocol = loop.run_until_complete(loop.create_datagram_endpoint(TemperatureProtocol, sock=create_socket(MCAST_GRP, THERMOMETER_PORT)))
    t_protocol.setup(kegerator_server.data, kegerator_server.lock)
    lc_transport, lc_protocol = loop.run_until_complete(loop.create_datagram_endpoint(LoadCellProtocol, sock=create_socket(MCAST_GRP, LOAD_CELL_PORT)))
    lc_protocol.setup(kegerator_server.data, kegerator_server.lock)
    k1_transport, k1_protocol = loop.run_until_complete(loop.create_datagram_endpoint(Keg1LevelProtocol, sock=create_socket(MCAST_GRP, KEG0_LEVEL_PORT)))
    k1_protocol.setup(kegerator_server.data, kegerator_server.lock)
    k2_transport, k2_protocol = loop.run_until_complete(loop.create_datagram_endpoint(Keg2LevelProtocol, sock=create_socket(MCAST_GRP, KEG1_LEVEL_PORT)))
    k2_protocol.setup(kegerator_server.data, kegerator_server.lock)

    websocket_server = WebSocketServer(kegerator_server.init, kegerator_server.consume, kegerator_server.produce, 8012)
    loop.run_until_complete(websocket_server.start_server)

    asyncio.ensure_future(kegerator_server.notify_clients())

    asyncio.get_event_loop().run_forever()

main()
