#!/usr/bin/python3

import paho.mqtt.client as mqtt
import os

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("home/basement/light_switch")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    status = msg.payload.decode('utf-8')
    print(status)
    if status == 'ON':
        os.system('vcgencmd display_power 1')
    elif status == 'OFF':
        os.system('vcgencmd display_power 0')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set('philx02', 'orbitxxx')
client.connect("192.168.3.103", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
