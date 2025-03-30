import time
from AxisMessages import *
from Axis import *

Motor = AxisUDP("192.168.12.156", 4568)

def WaitState(motor, state):
    while True:
        ret = GetMotorState(motor).value
        if ret is not None:
            if(ret != state):
                break
            #print(f"Mode: 0x{ret:02X}")
        else:
            print("Failed to get mode")
        time.sleep(0.1)

def mm_to_steps(mm):
    return 40*mm 

limit = 200

SetMaxSpeed(Motor, 3000)
SetAcceleration(Motor, 18000)
SetTargetPosition(Motor, mm_to_steps(200))
time.sleep(3)
SetTargetPosition(Motor, mm_to_steps(0))
