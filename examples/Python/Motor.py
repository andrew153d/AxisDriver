import time
from AxisMessages import *
from Axis import *

AxisComms = AxisUDP("192.168.12.156", 4568)

Home(AxisComms, 0)
#Start in velocity and step mode
SetVelocityAndSteps(AxisComms, 100, 8*100)
SetVelocityAndSteps(AxisComms, -100, -8*100)
StartPath(AxisComms, 0)
exit()
time.sleep(4)
#Setup parameters
SetAcceleration(AxisComms, 8*5000)
SetMaxSpeed(AxisComms, 8*6000)

# #Drive in relative position mode
SetCurrentPosition(AxisComms, 0)
SetTargetPosition(AxisComms, 200*8*0.5)
time.sleep(1)
SetTargetPosition(AxisComms, 122)
time.sleep(2)

# #Get the current position
print(f"Received Motor Position: {GetCurrentPosition(AxisComms).value:.2f}")

# #Drive in velocity mode
SetVelocity(AxisComms, 8*100)

print(f"Received Motor Velocity: {GetVelocity(AxisComms).value:.2f}")

time.sleep(2)

# #Stop the motor
SetMotorState(AxisComms, IDLE_ON)
