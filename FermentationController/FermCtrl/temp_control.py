from datetime import datetime
from datetime import timedelta
from accumulator import Accumulator
import sys
import socket

import RPi.GPIO as GPIO

class TemperatureControl:
    def __init__(self, gpio, high_threshold):
        self.gpio = gpio
        self.actual_temperature = Accumulator(60)
        self.high_threshold = float(high_threshold)
        self.run_period = timedelta(minutes=10)
        self.cooldown_period = timedelta(minutes=15)
        self.start_time = None
        self.stop_time = datetime.now() - self.cooldown_period
        self.cooling_command = False
        self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.MCAST_GRP = "224.0.0.2"
        self.MCAST_PORT = 10003
        self.enabled = True
        
        GPIO.setup(self.gpio, GPIO.OUT, initial=GPIO.HIGH)
        #self.__set_output(GPIO.HIGH)

    def __del__(self):
        GPIO.cleanup(self.gpio)

    def set_high_threshold(self, high_threshold):
        self.high_threshold = high_threshold

    def enable(self):
        self.enable = True

    def disable(self):
        self.enabled = False
        self.cooling_command = False
        self.__update_cooling()

    def update(self, temp):
        self.actual_temperature.add(temp)

        #debug
        print(str(self.actual_temperature.get_mean()) + ", " + str(self.run_period) + ", " + str(self.cooldown_period) + ", " + str(self.cooling_command) + ", " + str(self.start_time) + ", " + str(self.stop_time) + ", " + str(self.enabled))

        if not self.enabled:
            return

        #the algorithm could have 2 variables, target temp and a delta, with a proper model of the
        #freezer/wort bucket, compute time_on = [model stuff that predicts time required to get target - delta/2]
        # and compute that value only when temperature >= target + delta/2
        #lolz, this works even better with the thresholds

        if self.cooling_command:
            run_duration = datetime.now() - self.start_time
            if run_duration >= self.run_period:
                self.cooling_command = False
                self.stop_time = datetime.now()
        else:
            if self.actual_temperature.get_mean() >= self.high_threshold:
                cooldown_duration = datetime.now() - self.stop_time
                if cooldown_duration >= self.cooldown_period:
                    self.cooling_command = True
                    self.start_time = datetime.now()
        
        self.__update_cooling()
    
    def __update_cooling(self):
        if (self.cooling_command):
            self.__set_output(GPIO.LOW)
        else:
            self.__set_output(GPIO.HIGH)

    def __set_output(self, value):
        message = "0" if value else "1"
        self.udp_socket.sendto(message.encode('utf-8'), (self.MCAST_GRP, self.MCAST_PORT))
        GPIO.output(self.gpio, value)
