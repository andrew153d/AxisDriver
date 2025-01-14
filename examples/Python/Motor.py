import DriverComms
import Serial
import Messages
import time

SerialComms = Serial.AxisSerial('/dev/ttyACM0')

print("Setting Motor Position to 14000")
SerialComms.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPositionRelative, 200*64*1))
time.sleep(0.1)
msg = DriverComms.SetDouble(Messages.MessageTypes.GetTargetPosition, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
ret = DriverComms.GetDouble(ret)
print(f"Received Motor Position: {ret:.2f}")
time.sleep(0.1)

# print("Setting Motor to Position")
# msg = DriverComms.SetU8(Messages.MessageTypes.SetMotorState, 1)
# SerialComms.send_message(msg)
# time.sleep(0.1)
# msg = DriverComms.SetU8(Messages.MessageTypes.GetMotorState, 0)
# SerialComms.send_message(msg)
# ret = SerialComms.wait_serial_message()
# ret = DriverComms.GetU8(ret)
# print(f"Received Motor State: {ret}")
# time.sleep(4)
# msg = DriverComms.SetU8(Messages.MessageTypes.SetMotorState, 0)
# SerialComms.send_message(msg)
# time.sleep(0.1)

exit()

print("Setting Motor Velocity to 1000")
msg = DriverComms.SetDouble(Messages.MessageTypes.SetMotorVelocity, 1000)
SerialComms.send_message(msg)
time.sleep(0.1)
msg = DriverComms.SetDouble(Messages.MessageTypes.GetMotorVelocity, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
ret = DriverComms.GetDouble(ret)
print(f"Received Motor Velocity: {ret:.2f}")
time.sleep(1)
