""" BrewerControl """
from enum import Enum
import CHIP_IO.SOFTPWM as SPWM
import CHIP_IO.GPIO as GPIO

class HeaterMode(Enum):
    OFF = 0
    PWM = 1
    ALWAYS_ON = 2

class PumpMode(Enum):
    OFF = 0
    UNTIL = 1
    ALWAYS_ON = 2

class HeaterCommandSource(Enum):
    TEMPERATURE = 0
    FORCE_PWM = 1

class SpargeHeaterMode(Enum):
    OFF = 0
    ON = 1

class BrewerControl:
    """ BrewerControl """
    
    def __init__(self, k_p):
        self.k_p = k_p
        self.target_temp = 0
        self.actual_temp = 0
        self.duty_cycle = 0
        self.heater_mode = HeaterMode.OFF
        self.pump_mode = PumpMode.OFF
        self.pump_remaining_time = 0
        self.heater_command_source = HeaterCommandSource.TEMPERATURE
        self.sparge_heater_mode = SpargeHeaterMode.OFF
        SPWM.start("CSID0", 0, .5, 1)
        GPIO.setup("CSID1", GPIO.OUT)
        GPIO.output("CSID1", GPIO.LOW)
        GPIO.setup("CSID2", GPIO.OUT)
        GPIO.output("CSID2", GPIO.LOW)

    def __del__(self):
        SPWM.cleanup()

    def set_temperature_command(self, temp):
        """ set_temperature_target """
        self.heater_command_source = HeaterCommandSource.TEMPERATURE
        self.target_temp = temp
        print("set_temperature_command " + str(temp))

    def set_heater_mode(self, heater_mode):
        self.heater_mode = HeaterMode(heater_mode)
        if self.heater_mode == HeaterMode.OFF:
            self.duty_cycle = 0
            SPWM.set_duty_cycle("CSID0", self.duty_cycle)
        elif self.heater_mode == HeaterMode.ALWAYS_ON:
            self.duty_cycle = 100
            SPWM.set_duty_cycle("CSID0", self.duty_cycle)
        elif self.heater_mode == HeaterMode.PWM and self.heater_command_source == HeaterCommandSource.FORCE_PWM:
            SPWM.set_duty_cycle("CSID0", self.duty_cycle)
        print("set_heater_mode " + str(heater_mode))

    def set_pump_mode(self, pump_mode):
        self.pump_mode = PumpMode(pump_mode)
        if self.pump_mode == PumpMode.OFF:
            GPIO.output("CSID1", GPIO.LOW)
        elif self.pump_mode == PumpMode.UNTIL:
            pass
        elif self.pump_mode == PumpMode.ALWAYS_ON:
            GPIO.output("CSID1", GPIO.HIGH)
        print("set_pump_mode " + str(pump_mode))

    def set_pump_until_command(self, pump_time):
        self.pump_remaining_time = pump_time
        print("set_pump_until_command " + str(pump_time))

    def set_duty_cycle_command(self, duty_cycle):
        self.heater_command_source = HeaterCommandSource.FORCE_PWM
        self.duty_cycle = duty_cycle
        if self.heater_mode == HeaterMode.PWM:
            SPWM.set_duty_cycle("CSID0", self.duty_cycle)
        print("set_duty_cycle_command " + str(duty_cycle))

    def set_sparge_heater_mode(self, sparge_heater_mode):
        self.sparge_heater_mode = SpargeHeaterMode(sparge_heater_mode)
        if self.sparge_heater_mode == SpargeHeaterMode.ON:
            GPIO.output("CSID2", GPIO.HIGH)
        else:
            GPIO.output("CSID2", GPIO.LOW)
        print("set_sparge_heater_mode " + str(sparge_heater_mode))

    def tick(self, actual_temp):
        """ tick """
        self.actual_temp = actual_temp
        if self.heater_command_source == HeaterCommandSource.TEMPERATURE and self.heater_mode == HeaterMode.PWM:
            diff = self.target_temp - actual_temp
            self.duty_cycle = self.k_p * diff
            if self.duty_cycle < 0:
                self.duty_cycle = 0
            elif self.duty_cycle > 0 and self.duty_cycle < 15:
                self.duty_cycle = 15
            elif self.duty_cycle > 85:
                self.duty_cycle = 100
            SPWM.set_duty_cycle("CSID0", self.duty_cycle)
    
    def serialize(self):
        return str(self.target_temp) + "," + str(self.actual_temp) + "," + str(self.duty_cycle) + "," + str(self.heater_mode.value) + "," + str(self.pump_mode.value) + "," + str(self.pump_remaining_time) + "," + str(self.heater_command_source.value) + "," + str(self.sparge_heater_mode.value)
