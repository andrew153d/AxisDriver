import DriverComms
import Serial
import Messages
import time

SerialComms = Serial.AxisSerial('/dev/ttyACM0')

print("Setting Motor Position to 14000")
SerialComms.send_message(DriverComms.SetU32(Messages.MessageTypes.SetAcceleration, 64*500))
SerialComms.send_message(DriverComms.SetU32(Messages.MessageTypes.SetMaxSpeed, 64*100))
time.sleep(0.1)
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPositionRelative, 200*64*5))
time.sleep(0.1)
msg = DriverComms.SetDouble(Messages.MessageTypes.GetTargetPosition, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
ret = DriverComms.GetDouble(ret)
print(f"Received Motor Position: {ret:.2f}")
exit()
time.sleep(4)

print("Setting Motor Velocity to 2000")
msg = DriverComms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 6000)
SerialComms.send_message(msg)
time.sleep(0.1)
msg = DriverComms.SetDouble(Messages.MessageTypes.GetMotorVelocity, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
ret = DriverComms.GetDouble(ret)
print(f"Received Motor Velocity: {ret:.2f}")
time.sleep(10)
msg = DriverComms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 0)
SerialComms.send_message(msg)
