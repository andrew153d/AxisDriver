import DriverComms
import Serial
import Messages
import time
import numpy as np
from enum import Enum
import serial.tools.list_ports
import math
from collections import namedtuple

class MotorID(Enum):
    LeftMotor = 0xFCC23D6DB42A
    RightMotor = 0xFCC23D6DA11A

Point = namedtuple('Point', ['x', 'y'])
# Define constants
OFFSET = 86
L1 = 50
L2 = 150

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

    return (theta1_1), (theta1_2)

def move(mot, deg):
    print(f"Moving to {deg} degrees")
    mot.send_message(DriverComms.SetDouble(Messages.MessageTypes.SetTargetPosition, 200*64*(deg/360)))

def AddStep(mot, steps, speed):
    print(f"Adding {steps} steps at {speed} speed")
    mot.send_message(DriverComms.SetVelocityAndSteps(Messages.MessageTypes.SetVelocityAndSteps, speed, steps))

def WaitState(motor, state):
    while True:
        motor.send_message(DriverComms.SetU8(Messages.MessageTypes.GetMotorState, 0))
        ret = motor.wait_serial_message()
        if ret is not None:
            ret = DriverComms.GetU8(ret)
            if(ret != state):
                break
            #print(f"Mode: 0x{ret:02X}")
        else:
            print("Failed to get mode")
        time.sleep(0.1)

def IdentifyMotors():
        left_motor = None
        right_motor = None
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if(port.vid != 0x239A or port.pid != 0x8031):
                continue
            try:
                motor = Serial.AxisSerial(port.device)
                motor.send_message(DriverComms.GetMacAddressMessage(Messages.MessageTypes.GetMacAddress))
                ret = motor.wait_serial_message()
                if ret:
                    mac_address = int.from_bytes(ret[4:10], byteorder='big')
                    #print(f"Port: {port.device}, MAC Address: {mac_address:012X}")
                    if mac_address == MotorID.LeftMotor.value:
                        #print("Identified Left Motor")
                        left_motor = motor
                    elif mac_address == MotorID.RightMotor.value:
                        #print("Identified Right Motor")
                        right_motor = motor
                    else:
                        pass
                        #print("Unknown Motor")
            except Exception as e:
                pass
                #print(f"Failed to communicate with port {port.device}: {e}")
        if(left_motor is None or right_motor is None):
            print("Failed to identify motors")
        else:
            return left_motor, right_motor

def MoveScara(x, y):
    (angles1, angles2) = inverse_kinematics_dual(x, y)
    move(Left, angles1+18)
    move(Right, angles2+18)
Left, Right = IdentifyMotors()
 
Left.send_message(DriverComms.SetU8(Messages.MessageTypes.SetHomeDirection, Messages.HomeDirection.CLOCKWISE))
Right.send_message(DriverComms.SetU8(Messages.MessageTypes.SetHomeDirection, Messages.HomeDirection.CLOCKWISE))
Left.send_message(DriverComms.SetU8(Messages.MessageTypes.Home, 0))
Right.send_message(DriverComms.SetU8(Messages.MessageTypes.Home, 0))

WaitState(Left, Messages.MotorStates.HOME)
WaitState(Right, Messages.MotorStates.HOME)


Start = Point(((OFFSET/2) - 40), 130)
End = Point(((OFFSET/2) + 90), 160)

MoveScara(Start.x, Start.y)
time.sleep(2)

while(True):
    distance_to_goal = math.sqrt((End.x-Start.x)**2 + (End.y-Start.y)**2)
    distance_to_go_now = min(5, distance_to_goal)
    if(distance_to_goal < 1):
        break
    angle_to_go_now = math.atan2(End.y-Start.y, End.x-Start.x)
    x_step = distance_to_go_now * math.cos(angle_to_go_now)
    y_step = distance_to_go_now * math.sin(angle_to_go_now)
    Start = Point(Start.x + x_step, Start.y + y_step)
    print(f"Goal Distance: {distance_to_goal}, step distance: {distance_to_go_now}, angle step: {math.degrees(angle_to_go_now)}, x_step: {x_step}, y_step: {y_step}")
    
    curr_left_angle, curr_right_angle = inverse_kinematics_dual(Start.x, Start.y)
    new_left_angles, new_right_angles = inverse_kinematics_dual(Start.x + x_step, Start.y + y_step)

    left_diff = new_left_angles - curr_left_angle
    right_diff = new_right_angles - curr_right_angle
    print(f"Left diff: {left_diff}, Right diff: {right_diff}")
    
    #calculate the speed
    speed = 1000
    right_speed = 1000
    left_speed = 1000
    if(abs(left_diff)<abs(right_diff)):
        left_speed = 1000 * abs(left_diff/right_diff)
    else:
        right_speed = 1000 * abs(right_diff/left_diff)

    right_speed = right_speed*(right_diff/abs(right_diff))
    left_speed = left_speed*(left_diff/abs(left_diff))

    #print(f"Left Speed: {left_speed}, Right Speed: {right_speed}")
    AddStep(Left, int(left_diff*(1/360)*(200*64)), int(left_speed))
    AddStep(Right, int(right_diff*(1/360)*(200*64)), int(right_speed))
    

Left.send_message(DriverComms.SetU8(Messages.MessageTypes.StartPath, 0))
Right.send_message(DriverComms.SetU8(Messages.MessageTypes.StartPath, 0))

#time.sleep(5)
#MoveScara(POS2[0], POS2[1])
#time.sleep(1)