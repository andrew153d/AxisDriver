import DriverComms
from Serial import *
import Messages
import time

#SerialComms = AxisSerial('/dev/ttyACM0')
SerialComms = AxisUDP("192.168.12.156", 4568)

#Start in velocity and step mode
# SerialComms.send_message(DriverComms.SetVelocityAndSteps(Messages.MessageTypes.SetVelocityAndSteps, 1800, 64*100))
# SerialComms.send_message(DriverComms.SetVelocityAndSteps(Messages.MessageTypes.SetVelocityAndSteps, -4000, -64*100))
# SerialComms.send_message(DriverComms.SetU8(Messages.MessageTypes.StartPath, 0))

# time.sleep(6)

#Setup parameters
SerialComms.send_message(DriverComms.SetU32(Messages.MessageTypes.SetAcceleration, 64*1000))
SerialComms.send_message(DriverComms.SetU32(Messages.MessageTypes.SetMaxSpeed, 64*5000))

#Drive in relative position mode
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPositionRelative, 200*64*1))
exit()
time.sleep(7)
exit()
#Get the current position
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.GetTargetPosition, 0))
ret = DriverComms.GetDouble(SerialComms.wait_serial_message())
print(f"Received Motor Position: {ret:.2f}")

#Drive in velocity mode
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 6000))
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.GetMotorVelocity, 0))
ret = DriverComms.GetDouble(SerialComms.wait_serial_message())
print(f"Received Motor Velocity: {ret:.2f}")

time.sleep(2)

#Stop the motor
SerialComms.send_message(DriverComms.SetU8(Messages.MessageTypes.SetMotorState, Messages.MotorStates.IDLE_ON))
