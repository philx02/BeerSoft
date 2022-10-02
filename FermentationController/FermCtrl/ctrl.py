#!/usr/bin/python3

import socket
import struct
import argparse

import RPi.GPIO as GPIO
from temp_control import TemperatureControl
from data_collection import *
from create_socket import create_socket
from websocketserver import WebSocketServer
import requests

GPIO.setmode(GPIO.BCM)

MCAST_GRP = '224.0.0.2'
MCAST_PORT = 10000

def detect_temp_sensor_fault(data):
    if abs(data.get_difference_quotient()) > 5:
        return (True, "Derivative exceed tolerance: " + str(data.get_difference_quotient()))
    if data.last_value() > 50:
        return (True, "Absolute value exceed limit: " + str(data.last_value()))
    return (False, "")

def notify_sensor_fault(message_detail):
    url = 'https://voip.ms/api/v1/rest.php'
    message = 'Fermentation Control Sensor Malfunction Detected: ' + message_detail
    payload = {'api_username': 'pcayouette@spoluck.ca', 'api_password': '0TH7zRXKINXj7Exz8S0c', 'method': 'sendSMS', 'did': '4503141161', 'dst': '5144760655', 'message': message}
    return requests.get(url, params=payload)

class CtrlServer:
    def __init__(self, gpio, high_threshold):
        self.lock = asyncio.Lock()
        self.producer_condition = asyncio.Condition(lock=self.lock)
        self.temp_ctrl = TemperatureControl(gpio=gpio, high_threshold=high_threshold)
        self.ferm_data = FermData()
        self.sensor_fault = False

    def update(self):
        if not self.sensor_fault:
            sensor_fault, error_msg = detect_temp_sensor_fault(self.ferm_data.wort_temperature)
            if sensor_fault:
                notify_sensor_fault(error_msg)
                self.sensor_fault = True
                self.temp_ctrl.disable()
        self.temp_ctrl.update(self.ferm_data.wort_temperature.get_mean())

    @asyncio.coroutine
    def init(self, websocket):
        pass
        #with (yield from self.lock):
        #    message = str(self.temp_ctrl.high_threshold)
        #yield from websocket.send(message)

    @asyncio.coroutine
    def produce(self, websocket):
        with (yield from self.producer_condition):
            yield from self.producer_condition.wait()
            message = str(self.temp_ctrl.high_threshold)
        yield from websocket.send(message)

    @asyncio.coroutine
    def consume(self, websocket):
        message = yield from websocket.recv()
        print(message)
        command = message.split("|")
        if len(command) == 2 and command[0] == "set_high_threshold":
            with (yield from self.lock):
                self.temp_ctrl.set_high_threshold(float(command[1]))
                f = open('data/high_threshold.txt','w')
                f.write(command[1])
                f.close()
        with (yield from self.producer_condition):
            self.producer_condition.notify_all()

def main():
    parser = argparse.ArgumentParser(description='Temperature sensing.')
    parser.add_argument('-g', '--gpio', help='Output GPIO', required=True)
    parser.add_argument('-t', '--high_threshold', help='High threshold', required=True)

    args = parser.parse_args()

    loop = asyncio.get_event_loop()
    ctrl_server = CtrlServer(int(args.gpio), args.high_threshold)

    wt_transport, wt_protocol = loop.run_until_complete(loop.create_datagram_endpoint(WortTemperatureProtocol, sock=create_socket(MCAST_GRP, THERMOCOUPLE_PORT)))
    wt_protocol.setup(ctrl_server.ferm_data, ctrl_server.lock, ctrl_server.update)

    websocket_server = WebSocketServer(ctrl_server.init, ctrl_server.consume, ctrl_server.produce, 8011)
    loop.run_until_complete(websocket_server.start_server)

    asyncio.get_event_loop().run_forever()

main()
