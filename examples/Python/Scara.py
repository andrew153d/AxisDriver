import time
from AxisMessages import *
from Axis import *

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

def degrees_to_steps(degrees):
    steps_per_revolution = 200*8
    return int((degrees / 360) * steps_per_revolution)

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
    #print(f"Moving to {deg} degrees")
    SetTargetPosition(mot, 200*8*(deg/360))

def AddStep(mot, steps, speed, mode):
    #print(f"Adding {steps} steps at {speed} speed")
    SetVelocityAndSteps(mot, speed, steps, mode)

def IdentifyMotors():
        left_motor = None
        right_motor = None
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if(port.vid != 0x239A or port.pid != 0x8031):
                continue
            print(port)
            try:
                motor = AxisSerial(port.device)
                ret = GetMacAddress(motor).mac
                print(ret)
                if ret:
                    mac_address = int.from_bytes(ret, byteorder='big')
                    print(f"Port: {port.device}, MAC Address: {mac_address:012X}")
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
    move(Left, angles1)
    move(Right, angles2)

# calculate the speed of the motors
# the motor with  the least distance to travel will have the max speed
# the other motor will have a speed proportional to the distance
def calculate_motor_speeds(max_speed, left_steps, right_steps):
    time_to_goal = max(abs(left_steps), abs(right_steps)) / max_speed
    
    left_speed = left_steps / time_to_goal
    right_speed = right_steps / time_to_goal
    return left_speed, right_speed

def interpolate(Start:Point, End:Point, distance):
    # Calculate the distance between the two points
    total_distance = math.sqrt((End.x - Start.x) ** 2 + (End.y - Start.y) ** 2)
    
    # Calculate the number of steps needed
    num_steps = int(total_distance / distance)
    
    # Calculate the step size in x and y directions
    x_step = (End.x - Start.x) / num_steps
    y_step = (End.y - Start.y) / num_steps
    
    # Generate the interpolated points
    points = []
    for i in range(num_steps + 1):
        new_x = Start.x + i * x_step
        new_y = Start.y + i * y_step
        points.append(Point(new_x, new_y))
    
    return points

def interpolate_to_goal(Start:Point, End:Point, speed):
    left_time = 0
    right_time = 0
    current_position = Start
    interpolated_points = interpolate(Start, End, 3)
    angle_between_points = math.degrees(math.atan2(End.y - Start.y, End.x - Start.x))
    # for point in interpolated_points:
    #     print(f"Interpolated Point: x={point.x}, y={point.y}")

    for point in interpolated_points:
        curr_left_angle, curr_right_angle = inverse_kinematics_dual(current_position.x, current_position.y)
        new_left_angles, new_right_angles = inverse_kinematics_dual(point.x, point.y)
        
        left_distance = new_left_angles - curr_left_angle
        right_distance = new_right_angles - curr_right_angle
        if(left_distance == 0 and right_distance == 0):
            continue
        left_speed, right_speed = calculate_motor_speeds(speed, left_distance, right_distance)
        #print(f"Left Dist/Speed : {new_left_angles:.2f}/{left_speed:.2f}, Right Dist/Speed: {new_right_angles:.2f}/{right_speed:.2f}")
        right_time = (right_speed/abs(right_distance))
        left_time = (left_speed/abs(left_distance))
        #print(f"Left Time: {left_time:.2f}, Right Time: {right_time:.2f}")
        AddStep(Left, degrees_to_steps(new_left_angles), int(left_speed), ABSOLUTE)
        AddStep(Right, degrees_to_steps(new_right_angles), int(right_speed), ABSOLUTE)
        
        current_position = point


def move_to_goal(Start, End):
    left_steps = 0
    right_steps = 0
    print(f"Start: {Start}, End: {End}")
    while(True):
        distance_to_goal = math.sqrt((End.x-Start.x)**2 + (End.y-Start.y)**2)
        distance_to_go_now = min(50, distance_to_goal)
        angle_to_go_now = math.atan2(End.y-Start.y, End.x-Start.x)
        x_step = distance_to_go_now * math.cos(angle_to_go_now)
        y_step = distance_to_go_now * math.sin(angle_to_go_now)
        Start = Point(Start.x + x_step, Start.y + y_step)
        #print(f"Goal Distance: {distance_to_goal:.2f}, step distance: {distance_to_go_now:.2f}, angle step: {math.degrees(angle_to_go_now):.2f}, x_step: {x_step:.2f}, y_step: {y_step:.2f}")
        if(distance_to_go_now < 0.001):
            break
        curr_left_angle, curr_right_angle = inverse_kinematics_dual(Start.x, Start.y)
        #print(f"Current Left Angle: {curr_left_angle}, Current Right Angle: {curr_right_angle}")
        new_left_angles, new_right_angles = inverse_kinematics_dual(Start.x + x_step, Start.y + y_step)
        #print(f"New Left Angle: {new_left_angles}, New Right Angle: {new_right_angles}")
        left_diff = new_left_angles - curr_left_angle
        right_diff = new_right_angles - curr_right_angle
        #print(f"Left diff: {left_diff}, Right diff: {right_diff}")
        
        #calculate the speed
        right_speed = 100
        left_speed = 100
        if(abs(left_diff)<abs(right_diff)):
            left_speed = left_speed * abs(left_diff/right_diff)
        else:
            right_speed = right_speed * abs(right_diff/left_diff)

        right_speed = right_speed*(right_diff/abs(right_diff))
        left_speed = left_speed*(left_diff/abs(left_diff))

        #print(f"Left Speed: {left_speed}, Right Speed: {right_speed}")
        right_steps += right_diff
        left_steps += left_diff
        AddStep(Left, int(left_diff*(1/360)*(200*8)), int(left_speed))
        AddStep(Right, int(right_diff*(1/360)*(200*8)), int(right_speed))
    #print(f"Left steps: {left_steps}, Right steps: {right_steps}")




#Left, Right = IdentifyMotors()
Left  = AxisUDP("192.168.12.157", 4568)#AxisSerial('/dev/ttyACM0')
Right = AxisUDP("192.168.12.156", 4568)

SetHomeDirection(Left, CLOCKWISE)
SetHomeDirection(Right, CLOCKWISE)

Home(Left, 0)
Home(Right, 0)

WaitState(Left, HOME)
WaitState(Right, HOME)

SetCurrentPosition(Left, degrees_to_steps(-18))
SetCurrentPosition(Right, degrees_to_steps(-18))

P1 = Point(((OFFSET/2)-45), 140)
P2 = Point(((OFFSET/2)+45), 140)
P3 = Point(((OFFSET/2)), 180)


MoveScara(P1.x, P1.y)
time.sleep(3)
while(True):
    interpolate_to_goal(P1, P2, 500)
    StartPath(Left, 0)
    StartPath(Right, 0)
    time.sleep(1)
    interpolate_to_goal(P2, P3, 500)
    StartPath(Left, 0)
    StartPath(Right, 0)
    time.sleep(1)
    interpolate_to_goal(P3, P1, 500)
    StartPath(Left, 0)
    StartPath(Right, 0)
    time.sleep(1)