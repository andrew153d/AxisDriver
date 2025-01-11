import Comms
import time
import Messages

print("Setting Motor Position to 14000")
Comms.SetDouble(Messages.MessageTypes.SetTargetPositionRelative, 200*64*1)
time.sleep(0.1)
ret = Comms.GetDouble(Messages.MessageTypes.GetTargetPosition)
print(f"Received Motor Position: {ret:.2f}")
time.sleep(1)

print("Setting Motor to Position")
Comms.SetU8(Messages.MessageTypes.SetMotorState, 1)
time.sleep(0.1)
ret = Comms.GetU8(Messages.MessageTypes.GetMotorState)
print(f"Received Motor State: {ret[4]}")
time.sleep(2)
Comms.SetU8(Messages.MessageTypes.SetMotorState, 0)
time.sleep(0.1)


# print("Setting Motor Velocity to 1000")
# Comms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 1000)
# time.sleep(0.1)
# ret = Comms.GetDouble(Messages.MessageTypes.GetMotorVelocity)
# print(f"Received Motor Velocity: {ret}")
# time.sleep(1)
