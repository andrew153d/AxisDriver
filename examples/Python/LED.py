import time
from AxisMessages import *
from Axis import *

SerialComms = AxisUDP("192.168.12.156", 4568)


print("Flashing Error")
SetLedState(SerialComms, FLASH_ERROR)
time.sleep(2)
print("Setting LED state to solid")
SetLedState(SerialComms, SOLID)
print("Setting LED color to blue")
SetLedColor(SerialComms, {0, 0, 255})
time.sleep(2)
print("Setting LED color to green")
SetLedColor(SerialComms, {0, 255, 0})
time.sleep(2)
print("Setting LED color to red")
SetLedColor(SerialComms, {255, 0, 0})
time.sleep(2)
print("Setting LED color to white")
SetLedColor(SerialComms, {255, 255, 255})
time.sleep(1)
print(GetLedColor(SerialComms).ledColor)
#print("Received LED color: R: {}, G: {}, B: {}".format(ret[4], ret[5], ret[6]))
time.sleep(1)
print("Setting LED state to off")
SetLedState(SerialComms, OFF)
time.sleep(1)