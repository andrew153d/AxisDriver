import Comms
import time
import Messages

print("Setting Motor to Position")
Comms.SetU8(Messages.MessageTypes.SetMotorState, 1)
time.sleep(0.1)
ret = Comms.GetU8(Messages.MessageTypes.GetMotorState)
print(f"Received Motor State: {ret[4]}")

print("Setting Motor Position to 14000")
Comms.SetDouble(Messages.MessageTypes.SetMotorPosition, 14000)
time.sleep(0.1)
ret = Comms.GetDouble(Messages.MessageTypes.GetMotorPosition)
print(f"Received Motor Position: {ret}")
time.sleep(1)

print("Setting Motor Velocity to 1000")
Comms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 1000)
time.sleep(0.1)
ret = Comms.GetDouble(Messages.MessageTypes.GetMotorVelocity)
print(f"Received Motor Velocity: {ret}")
time.sleep(1)
