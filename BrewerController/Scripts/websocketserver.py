import asyncio
import websockets
import producerconsumer

connected = set()

@asyncio.coroutine
def produce():
    return producerconsumer.producer()

@asyncio.coroutine
def consume(message):
    producerconsumer.consumer(message)

@asyncio.coroutine
def consumer_handler(websocket):
    while True:
        message = yield from websocket.recv()
        yield from consume(message)

@asyncio.coroutine
def producer_handler(websocket):
    while True:
        message = yield from produce()
        yield from websocket.send(message)
        yield from asyncio.sleep(1)

@asyncio.coroutine
def handler(websocket, path):
    print("client connected")
    global connected
    # Register.
    connected.add(websocket)
    try:
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
