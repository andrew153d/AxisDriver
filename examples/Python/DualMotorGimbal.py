#!/usr/bin/env python3
"""
Simplified Dual Motor Gimbal Controller

This script provides a simple interface to control two motors:
- Move to any position with move(tilt_angle, pan_angle)
- Use time.sleep() for delays between moves
- Automatic motor discovery and setup
"""

import sys
import time
import serial.tools.list_ports
from AxisProtocol import *
from Axis import AxisSerial


class SimplifiedGimbalController:
    def __init__(self, baudrate=115200):
        """Initialize the gimbal controller."""
        self.baudrate = baudrate
        self.PITCH_MOTOR_MAC = 0xFCC23D6DB42A  # FC:C2:3D:6D:B4:2A
        self.pitch_motor = None
        self.yaw_motor = None
        self._setup_complete = False
        
        # Track current positions for tilt compensation
        self.current_tilt = 0.0
        self.current_pan = 0.0
        
    def setup(self):
        """Discover and configure motors. Call this once before using move()."""
        if self._setup_complete:
            return True
            
        print("Setting up gimbal...")
        
        if not self._discover_motors():
            print("Failed to discover motors")
            return False
            
        if not self._configure_motors():
            print("Failed to configure motors")
            return False
            
        self._setup_complete = True
        print("Gimbal ready!")
        return True
        
    def move(self, tilt_angle, pan_angle):
        """Move gimbal to specified angles.
        
        Args:
            tilt_angle: Pitch angle in degrees (tilt up/down)
            pan_angle: Yaw angle in degrees (pan left/right)
            
        Note: When panning, the tilt motor is compensated to maintain
        the same absolute tilt angle in space.
        """
        if not self._setup_complete:
            print("Error: Call setup() first!")
            return False
            
        print(f"Moving to: tilt={tilt_angle}°, pan={pan_angle}°")
        
        # Calculate pan change for tilt compensation
        pan_change = pan_angle - self.current_pan
        
        # Calculate compensated tilt position:
        # To maintain absolute tilt angle, tilt motor must move by same amount as pan change
        compensated_tilt = tilt_angle + pan_change
        
        if pan_change != 0:
            print(f"Pan changed by {pan_change}°, compensating tilt: {tilt_angle}° + {pan_change}° = {compensated_tilt}°")
        
        success = True
        
        # Set both motors simultaneously
        if self.pitch_motor:
            success &= self._set_position(self.pitch_motor, compensated_tilt)
        if self.yaw_motor:
            success &= self._set_position(self.yaw_motor, pan_angle)
        
        # Update current positions if successful
        if success:
            self.current_tilt = tilt_angle  # Store the desired tilt (not compensated)
            self.current_pan = pan_angle
            
        return success
        
    def tilt_only(self, tilt_angle):
        """Move only the tilt motor without pan compensation.
        
        Args:
            tilt_angle: Pitch angle in degrees (tilt up/down)
            
        Use this when you want to tilt independently without affecting
        the compensation relationship.
        """
        if not self._setup_complete:
            print("Error: Call setup() first!")
            return False
            
        if not self.pitch_motor:
            print("Error: No pitch motor available")
            return False
            
        print(f"Tilting to: {tilt_angle}° (independent)")
        
        success = self._set_position(self.pitch_motor, tilt_angle)
        
        # Update current tilt position
        if success:
            self.current_tilt = tilt_angle
            
        return success
        
    def _get_motor_mac_address(self, motor):
        """Get MAC address from a motor."""
        try:
            get_mac_msg = GetMacAddressMessage()
            motor.send_message(get_mac_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MAC_ADDRESS_ID], timeout=2)
            
            if response:
                mac_tuple = parse_get_mac_address_response(response)
                return mac_tuple
            return None
                
        except Exception as e:
            print(f"Error getting MAC: {e}")
            return None

    def _discover_motors(self):
        """Discover and identify motors."""
        print("Discovering motors...")
        
        ports = serial.tools.list_ports.comports()
        discovered_motors = []
        
        for port in ports:
            if port.vid != 0x239A or port.pid != 0x8031:
                continue
                
            try:
                motor = AxisSerial(port.device, self.baudrate)
                time.sleep(0.5)
                
                ret = self._get_motor_mac_address(motor)
                if ret:
                    mac_address = int.from_bytes(ret, byteorder='big')
                    mac_string = ':'.join(f'{b:02X}' for b in ret)
                    
                    if mac_address == self.PITCH_MOTOR_MAC:
                        print(f"Found PITCH motor: {mac_string}")
                        self.pitch_motor = motor
                    else:
                        print(f"Found YAW motor: {mac_string}")
                        discovered_motors.append(motor)
                else:
                    motor.motor_port.close()
                    
            except Exception as e:
                print(f"Error with port {port.device}: {e}")
                continue
        
        # Assign first available motor as yaw motor
        if discovered_motors:
            self.yaw_motor = discovered_motors[0]
            
        success = self.pitch_motor is not None
        print(f"Motors found: PITCH={'Yes' if self.pitch_motor else 'No'}, YAW={'Yes' if self.yaw_motor else 'No'}")
        return success
    
    def _configure_motors(self):
        """Configure motors for position control."""
        print("Configuring motors...")
        
        for motor, name in [(self.pitch_motor, "PITCH"), (self.yaw_motor, "YAW")]:
            if motor is None:
                continue
                
            try:
                # Set to position mode
                motor.send_message(SetMotorStateMessage(MotorStates.POSITION))
                if not motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2):
                    raise Exception(f"Failed to set {name} motor state")
                
                # Set max speed
                motor.send_message(SetMaxSpeedMessage(5000))
                if not motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2):
                    raise Exception(f"Failed to set {name} max speed")
                
                # Set acceleration
                motor.send_message(SetAccelerationMessage(3000))
                if not motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2):
                    raise Exception(f"Failed to set {name} acceleration")
                
                # Zero current position
                motor.send_message(SetCurrentPositionMessage(0.0))
                if not motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2):
                    raise Exception(f"Failed to zero {name} position")
                
                print(f"{name} motor configured")
                
            except Exception as e:
                print(f"Error configuring {name} motor: {e}")
                return False
        
        return True
    
    def _set_position(self, motor, position):
        """Set motor target position."""
        try:
            motor.send_message(SetTargetPositionMessage(position))
            return motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2) is not None
        except Exception as e:
            print(f"Error setting position: {e}")
            return False
    
    def stop(self):
        """Stop all motors."""
        for motor in [self.pitch_motor, self.yaw_motor]:
            if motor:
                try:
                    motor.send_message(SetMotorStateMessage(MotorStates.OFF))
                    motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
                except:
                    pass


def demo():
    """Simple demo showing how to use the simplified controller."""
    # Create controller and setup
    gimbal = SimplifiedGimbalController()
    if not gimbal.setup():
        return
    
    print("\nRunning demo sequence...")
    print("This demo shows tilt compensation during pan movements")
     
    # Demo 2: Show tilt compensation during panning
    print("\n=== Demo 2: Tilt Compensation During Pan ===")
    print("Notice how tilt motor compensates to maintain absolute angle:")

    # Demo 3: Multiple combined moves
    print("\n=== Demo 3: Complex Movements ===")
    positions = [
        (0, 0),        # Center
        (30, 45),      # Tilt compensation: 30 + 45 = 75°
        (30, 90),      # Tilt compensation: 30 + (90-45) = 75° (no change as pan delta is 45°)
        (-15, 90),     # Tilt compensation: -15 + (90-90) = -15° (just tilt change)
        (-15, 0),      # Tilt compensation: -15 + (0-90) = -105° (major compensation)
        (0, 0),        # Back to center
    ]
    
    for i, (tilt, pan) in enumerate(positions):
        print(f"\nMove {i+1}: Going to tilt={tilt}°, pan={pan}°")
        gimbal.move(tilt, pan*10)
        time.sleep(3)
    
    gimbal.move(0, 10000)
    time.sleep(10)
    
    print("\nDemo complete!")
    gimbal.stop()


def main():
    """Main entry point - run the demo."""
    try:
        demo()
    except KeyboardInterrupt:
        print("\nDemo interrupted")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()