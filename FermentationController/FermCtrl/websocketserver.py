import asyncio
import websockets
import producerconsumer

connected = set()

@asyncio.coroutine
def consumer_handler(websocket):
    while True:
        yield from producerconsumer.consume(websocket)

@asyncio.coroutine
def producer_handler(websocket):
    while True:
        yield from producerconsumer.produce(websocket)

@asyncio.coroutine
def handler(websocket, path):
    print("client connected")
    global connected
    # Register.
    connected.add(websocket)
    try:
        yield from producerconsumer.init(websocket)
        consumer_task = asyncio.async(consumer_handler(websocket))
        producer_task = asyncio.async(producer_handler(websocket))
        done, pending = yield from asyncio.wait(
            [consumer_task, producer_task],
            return_when=asyncio.FIRST_COMPLETED,
        )
        for task in pending:
            task.cancel()
    finally:
        # Unregister.
        connected.remove(websocket)

start_server = websockets.serve(handler, '0.0.0.0', 8010)

asyncio.get_event_loop().run_until_complete(start_server)
