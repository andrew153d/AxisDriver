import DriverComms
import Serial
import Messages
import time

MotorA = Serial.AxisSerial('/dev/ttyACM0')
#MotorB = Serial.AxisSerial('/dev/ttyACM1')

def move(mot, deg):
    print(f"Moving to {deg} degrees")
    mot.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPositionRelative, 200*64*(deg/360)))

MotorA.send_message(DriverComms.SetU8(Messages.MessageTypes.SetMotorBrake, Messages.MotorBrake.FREEWHEELING))
MotorA.send_message(DriverComms.SetU32(Messages.MessageTypes.SetAcceleration, 64*100))
MotorA.send_message(DriverComms.SetU32(Messages.MessageTypes.SetMaxSpeed, 64*100))

time.sleep(0.1)
move(MotorA, -365*2)
time.sleep(0.1)
