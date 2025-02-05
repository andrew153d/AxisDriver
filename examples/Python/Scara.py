import DriverComms
import Serial
import Messages
import time
import numpy as np
MotorA = Serial.AxisSerial('/dev/ttyACM0')
#MotorB = Serial.AxisSerial('/dev/ttyACM1')

# Define constants
OFFSET = 200
L1 = 200
L2 = 300

def inverse_kinematics_dual(x, y):
    def calculate_angles(x, y, flip_theta2=False):
        r = np.sqrt(x**2 + y**2)
        cos_theta_2 = (r**2 - L1**2 - L2**2) / (2 * L1 * L2)
        theta_2 = np.arccos(np.clip(cos_theta_2, -1.0, 1.0))
        if flip_theta2:
            theta_2 = -theta_2
        k1 = L1 + L2 * np.cos(theta_2)
        k2 = L2 * np.sin(theta_2)
        theta_1 = np.arctan2(y, x) - np.arctan2(k2, k1)
        return np.degrees(theta_1), np.degrees(theta_2)

    theta1_1, theta2_1 = calculate_angles(x, y, y > 0)
    theta1_2, theta2_2 = calculate_angles(x - OFFSET, y, y < 0)

    return (theta1_1, theta2_1), (theta1_2, theta2_2)

def move(mot, deg):
    print(f"Moving to {deg} degrees")
    mot.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPosition, 200*64*(deg/360)))

def WaitState(motor):
    while True:
        motor.send_message(DriverComms.SetU8(Messages.MessageTypes.GetMotorState, 0))
        ret = motor.wait_serial_message()
        if ret is not None:
            ret = DriverComms.GetU8(ret)
            if(ret != 4):
                break
            print(f"Mode: 0x{ret:02X}")
        else:
            print("Failed to get mode")
        time.sleep(0.1)

#MotorA.send_message(DriverComms.SetU8(Messages.MessageTypes.SetMotorBrake, Messages.MotorBrake.FREEWHEELING))
#time.sleep(0.05)
#MotorA.send_message(DriverComms.SetU32(Messages.MessageTypes.SetAcceleration, 64*100))
#time.sleep(0.05)
#MotorA.send_message(DriverComms.SetU32(Messages.MessageTypes.SetMaxSpeed, 64*50))
#time.sleep(0.05)

MotorA.send_message(DriverComms.SetU8(Messages.MessageTypes.SetHomeDirection, Messages.HomeDirection.CLOCKWISE))
time.sleep(0.05)
MotorA.send_message(DriverComms.SetU8(Messages.MessageTypes.Home, 0))

#WaitState(MotorA)
#move(MotorA, 100)
#time.sleep(2)
#move(MotorA, 0)
#time.sleep(2)
#move(MotorA, -25)
#
# move(MotorA, 0)
time.sleep(5)
