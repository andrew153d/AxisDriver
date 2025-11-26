#!/usr/bin/env python3
"""
Simple Gimbal Gearing Test

Moves each motor one revolution (360 degrees) to determine gear ratio.
"""

import time
import serial.tools.list_ports
from AxisProtocol import *
from Axis import AxisSerial
import math


def find_motors():
    """Find and identify motors."""
    PITCH_MOTOR_MAC = 0xFCC23D6DB42A
    pitch_motor = None
    yaw_motor = None
    
    print("Looking for motors...")
    
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port.vid != 0x239A or port.pid != 0x8031:
            continue
            
        try:
            motor = AxisSerial(port.device, 115200)
            time.sleep(0.5)
            
            # Get MAC address
            motor.send_message(GetMacAddressMessage())
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MAC_ADDRESS_ID], timeout=2)
            
            if response:
                mac_tuple = parse_get_mac_address_response(response)
                mac_address = int.from_bytes(mac_tuple, byteorder='big')
                
                if mac_address == PITCH_MOTOR_MAC:
                    print(f"Found PITCH motor")
                    pitch_motor = motor
                else:
                    print(f"Found YAW motor")
                    yaw_motor = motor
            else:
                motor.motor_port.close()
                
        except Exception as e:
            print(f"Error with port {port.device}: {e}")
    
    return pitch_motor, yaw_motor


def setup_motor(motor, name):
    """Configure motor for position control."""
    try:
        # Set to position mode
        motor.send_message(SetMotorStateMessage(MotorStates.POSITION))
        motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
        
        # Set speed and acceleration
        motor.send_message(SetMaxSpeedMessage(2000))
        motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
        
        motor.send_message(SetAccelerationMessage(1000))
        motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
        
        # Zero position
        motor.send_message(SetCurrentPositionMessage(0.0))
        motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
        
        print(f"{name} motor ready")
        return True
    except:
        print(f"Failed to setup {name} motor")
        return False


def test_motor(motor, name):
    """Move motor one revolution and calculate gear ratio."""
    print(f"\nTesting {name} motor...")
    
    # Get starting position
    motor.send_message(GetCurrentPositionMessage())
    response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], timeout=2)
    start_pos = parse_get_current_position_response(response)
    
    print(f"Start position: {start_pos:.1f}°")
    
    # Move 360 degrees
    target_pos = start_pos + 360.0
    motor.send_message(SetTargetPositionMessage(target_pos))
    motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
    
    print("Moving 360°...")
    time.sleep(4)
    
    # Get end position
    motor.send_message(GetCurrentPositionMessage())
    response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], timeout=2)
    end_pos = parse_get_current_position_response(response)
    
    print(f"End position: {end_pos:.1f}°")
    
    # Calculate gear ratio
    actual_movement = end_pos - start_pos
    gear_ratio = 360.0 / actual_movement if actual_movement != 0 else 0
    
    print(f"Actual movement: {actual_movement:.1f}°")
    print(f"Gear ratio: {gear_ratio:.2f}:1")
    
    return gear_ratio

def GimbalTo(pitch, yaw, pitch_motor, yaw_motor):
    yaw_motor_steps = int(yaw * (41/10))
    pitch_motor_steps = int(yaw_motor_steps - (pitch  * (41/10) * (15/45)))
    
    # maximum allowed speed (steps per second)
    MAX_SPEED = 1000.0

    # absolute step counts
    yaw_abs = abs(yaw_motor_steps)
    pitch_abs = abs(pitch_motor_steps)

    # if both targets are zero, set speeds to zero
    if yaw_abs == 0 and pitch_abs == 0:
        yaw_speed = 0
        pitch_speed = 0
    else:
        # choose a common travel time so the larger move runs at MAX_SPEED
        travel_time = max(yaw_abs, pitch_abs) / MAX_SPEED if max(yaw_abs, pitch_abs) > 0 else 0.0
        # guard against zero travel_time
        if travel_time <= 0:
            yaw_speed = 0
            pitch_speed = 0
        else:
            # compute required speeds to finish in travel_time, cap at MAX_SPEED
            yaw_speed = 0 if yaw_abs == 0 else min(MAX_SPEED, yaw_abs / travel_time)
            pitch_speed = 0 if pitch_abs == 0 else min(MAX_SPEED, pitch_abs / travel_time)

            # convert to integers, ensure non-zero moves get at least 1 step/s
            yaw_speed = 0 if yaw_speed == 0 else int(math.ceil(yaw_speed))
            pitch_speed = 0 if pitch_speed == 0 else int(math.ceil(pitch_speed))
    
    yaw_motor.send_message(SetVelocityAndStepsMessage(yaw_speed, yaw_motor_steps, PositionMode.ABSOLUTE))
    pitch_motor.send_message(SetVelocityAndStepsMessage(pitch_speed, pitch_motor_steps, PositionMode.ABSOLUTE))

    
    
def main():
    """Simple gearing test."""
    print("Gimbal Gearing Test")
    print("Moving each motor one revolution to determine gear ratios")
    
    # Find motors
    pitch_motor, yaw_motor = find_motors()
    
    if not pitch_motor and not yaw_motor:
        print("No motors found!")
        return
    
    #setup_motor(pitch_motor, "PITCH")
    #setup_motor(yaw_motor, "YAW")
    
    GimbalTo(0, -700, pitch_motor, yaw_motor)
    time.sleep(0.1)
    GimbalTo(360, 0, pitch_motor, yaw_motor)
    time.sleep(0.1)
    GimbalTo(-360, 180, pitch_motor, yaw_motor)
    time.sleep(0.1)
    yaw_motor.send_message(SetMotorStateMessage(MotorStates.VELOCITY_STEP))
    pitch_motor.send_message(SetMotorStateMessage(MotorStates.VELOCITY_STEP))
    exit(0)
    
    # Test each motor
    if pitch_motor:
        setup_motor(pitch_motor, "PITCH")
        test_motor(pitch_motor, "PITCH")
    
    if yaw_motor:
        setup_motor(yaw_motor, "YAW")
        test_motor(yaw_motor, "YAW")
    
    print("\nTest complete!")


if __name__ == "__main__":
    main()