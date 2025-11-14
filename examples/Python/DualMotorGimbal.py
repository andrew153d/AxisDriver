#!/usr/bin/env python3
"""
Dual Motor Gimbal Velocity Control Script with GUI

This script discovers and controls two motors using MAC addresses with velocity control:
- One motor has the known MAC address FC:C2:3D:6D:B4:2A (pitch motor)
- The other motor is automatically discovered as the second available motor
- GUI with joystick control interface for velocity control
- Click and drag joystick to set velocity, release to stop

Based on ProtocolTest.py and Scara.py reference implementations.
"""

import sys
import time
import threading
import math
import serial.tools.list_ports
import tkinter as tk
from tkinter import ttk, messagebox
from AxisProtocol import *
from AxisProtocol import GetMacAddressMessage, parse_get_mac_address_response, SetTargetPositionMessage, SetCurrentPositionMessage, SetVelocityMessage
from Axis import AxisSerial


class JoystickWidget(tk.Canvas):
    def __init__(self, parent, size=200, callback=None):
        super().__init__(parent, width=size, height=size, bg='black')
        self.size = size
        self.center = size // 2
        self.radius = size // 2 - 10
        self.knob_radius = 15
        self.callback = callback
        
        # Current joystick position
        self.knob_x = self.center
        self.knob_y = self.center
        
        # Draw the joystick base
        self.create_oval(
            self.center - self.radius, self.center - self.radius,
            self.center + self.radius, self.center + self.radius,
            outline='gray', width=3
        )
        
        # Draw center crosshair
        self.create_line(
            self.center - 10, self.center, 
            self.center + 10, self.center,
            fill='gray', width=2
        )
        self.create_line(
            self.center, self.center - 10,
            self.center, self.center + 10,
            fill='gray', width=2
        )
        
        # Draw the knob
        self.knob_id = self.create_oval(
            self.knob_x - self.knob_radius, self.knob_y - self.knob_radius,
            self.knob_x + self.knob_radius, self.knob_y + self.knob_radius,
            fill='red', outline='white', width=2
        )
        
        # Bind mouse events
        self.bind('<Button-1>', self.on_click)
        self.bind('<B1-Motion>', self.on_drag)
        self.bind('<ButtonRelease-1>', self.on_release)
        
    def on_click(self, event):
        self.move_knob(event.x, event.y)
        
    def on_drag(self, event):
        self.move_knob(event.x, event.y)
        
    def on_release(self, event):
        # Snap back to center on release and stop motors
        self.move_knob(self.center, self.center)
        # This will trigger on_joystick_move with (0,0) which stops motors
    
    def move_knob(self, x, y):
        # Calculate distance from center
        dx = x - self.center
        dy = y - self.center
        distance = math.sqrt(dx*dx + dy*dy)
        
        # Constrain to circle
        if distance > self.radius - self.knob_radius:
            angle = math.atan2(dy, dx)
            max_distance = self.radius - self.knob_radius
            dx = max_distance * math.cos(angle)
            dy = max_distance * math.sin(angle)
        
        # Update knob position
        self.knob_x = self.center + dx
        self.knob_y = self.center + dy
        
        # Redraw knob
        self.coords(
            self.knob_id,
            self.knob_x - self.knob_radius, self.knob_y - self.knob_radius,
            self.knob_x + self.knob_radius, self.knob_y + self.knob_radius
        )
        
        # Calculate normalized values (-1 to 1)
        norm_x = dx / (self.radius - self.knob_radius)
        norm_y = -dy / (self.radius - self.knob_radius)  # Invert Y axis
        
        # Call callback if provided
        if self.callback:
            self.callback(norm_x, norm_y)
    
    def center_knob(self):
        """Reset joystick to center position"""
        self.move_knob(self.center, self.center)


class GimbalGUI:
    def __init__(self, controller):
        self.controller = controller
        self.root = tk.Tk()
        self.root.title("Gimbal Joystick")
        self.root.geometry("250x250")
        self.root.resizable(False, False)
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        
        # Remove window decorations for minimal look
        self.root.configure(bg='black')
        
        # Variables for continuous movement
        self.current_norm_x = 0.0
        self.current_norm_y = 0.0
        
        self.setup_ui()
    
    def setup_ui(self):
        # Create joystick widget that fills the entire window
        self.joystick = JoystickWidget(self.root, size=240, callback=self.on_joystick_move)
        self.joystick.pack(expand=True, fill='both', padx=5, pady=5)
        
        # Bind escape key to close
        self.root.bind('<Escape>', lambda e: self.on_closing())
        self.root.focus_set()
        
    def on_joystick_move(self, norm_x, norm_y):
        """Handle joystick movement - set motor velocities"""
        self.current_norm_x = norm_x
        self.current_norm_y = norm_y
        
        # Define deadspace threshold (0.0 to 1.0)
        deadspace = 0.15  # 15% deadspace from center
        
        # Apply deadspace to both axes
        if abs(norm_x) < deadspace:
            norm_x = 0.0
        else:
            # Scale the remaining range to full range
            if norm_x > 0:
                norm_x = (norm_x - deadspace) / (1.0 - deadspace)
            else:
                norm_x = (norm_x + deadspace) / (1.0 - deadspace)
        
        if abs(norm_y) < deadspace:
            norm_y = 0.0
        else:
            # Scale the remaining range to full range
            if norm_y > 0:
                norm_y = (norm_y - deadspace) / (1.0 - deadspace)
            else:
                norm_y = (norm_y + deadspace) / (1.0 - deadspace)
        
        # Calculate velocities based on joystick position (after deadspace)
        max_vel = self.controller.max_velocity
        
        # Handle motor coupling: yaw movement requires both motors to move
        if abs(norm_x) > 0:  # X-axis controls yaw, but requires both motors
            # For yaw movement, both motors move together
            yaw_velocity = norm_x * max_vel
            # Pitch motor also moves with yaw to enable yaw movement
            if abs(norm_y) > 0:
                # If both axes are active, combine the movements
                pitch_velocity = norm_y * max_vel
            else:
                # If only yaw is active, pitch motor moves with yaw
                pitch_velocity = norm_x * max_vel
        else:
            # Pure pitch movement (Y-axis only)
            yaw_velocity = 0.0
            pitch_velocity = norm_y * max_vel
        
        # Apply velocities
        threading.Thread(target=self.apply_velocity, args=(pitch_velocity, yaw_velocity), daemon=True).start()
    
    def apply_velocity(self, pitch_velocity, yaw_velocity):
        """Apply velocity to motors (runs in separate thread)"""
        self.controller.set_pitch_velocity(pitch_velocity)
        self.controller.set_yaw_velocity(yaw_velocity)
    
    def on_closing(self):
        """Handle window close event"""
        self.controller.running = False
        self.controller.cleanup()
        self.root.destroy()
    
    def run(self):
        """Start the GUI main loop"""
        self.root.mainloop()


class DualMotorGimbalController:
    def __init__(self, baudrate=115200):
        """Initialize the dual motor gimbal controller with serial communication."""
        self.baudrate = baudrate
        
        # Known MAC addresses
        self.PITCH_MOTOR_MAC = 0xFCC23D6DB42A  # FC:C2:3D:6D:B4:2A
        
        # Motor instances
        self.pitch_motor = None
        self.yaw_motor = None
        
        # Control parameters
        self.current_pitch_velocity = 0.0
        self.current_yaw_velocity = 0.0
        self.max_velocity = 800.0    # Maximum velocity in degrees/second
        self.position_step = 15.0     # Keep for compatibility
        self.max_speed = 2000.0       # Maximum speed for motor setup
        self.running = True
        
    def get_motor_mac_address(self, motor):
        """Get MAC address from a motor using the same logic as Gimbal.py."""
        try:
            # Send GET_MAC_ADDRESS message
            get_mac_msg = GetMacAddressMessage()
            motor.send_message(get_mac_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MAC_ADDRESS_ID], timeout=2)
            
            if response:
                mac_tuple = parse_get_mac_address_response(response)
                return mac_tuple
            else:
                return None
                
        except Exception as e:
            print(f"    Error getting MAC: {e}")
            return None

    def discover_motors(self):
        """Discover and identify motors on serial ports using MAC addresses."""
        print("Scanning for motors...")
        
        pitch_motor = None
        yaw_motor = None
        discovered_motors = []
        
        ports = serial.tools.list_ports.comports()
        
        for port in ports:
            # Filter for known VID/PID of Axis Driver devices
            if port.vid != 0x239A or port.pid != 0x8031:
                continue
                
            print(f"Checking port: {port.device}")
            
            try:
                # Create motor connection (same as Scara.py)
                motor = AxisSerial(port.device, self.baudrate)
                time.sleep(0.5)  # Allow connection to stabilize
                
                # Get MAC address using the same method as Gimbal.py
                ret = self.get_motor_mac_address(motor)
                
                if ret:
                    mac_address = int.from_bytes(ret, byteorder='big')
                    mac_string = ':'.join(f'{b:02X}' for b in ret)
                    print(f"  Port: {port.device}, MAC Address: {mac_string}")
                    
                    if mac_address == self.PITCH_MOTOR_MAC:
                        print("  → Identified as PITCH motor")
                        pitch_motor = motor
                    else:
                        print("  → Available for YAW motor")
                        discovered_motors.append((motor, mac_string, port.device))
                else:
                    print(f"  → Failed to get MAC address from {port.device}")
                    # Properly close the connection
                    try:
                        motor.motor_port.close()
                    except:
                        pass
                    
            except Exception as e:
                print(f"  → Failed to communicate with port {port.device}: {e}")
                # Ensure proper cleanup
                try:
                    if 'motor' in locals():
                        motor.motor_port.close()
                except:
                    pass
                continue
        
        # Assign the first available motor as yaw motor if pitch motor was found
        if pitch_motor and discovered_motors:
            yaw_motor, yaw_mac, yaw_port = discovered_motors[0]
            print(f"\nMotor Assignment:")
            print(f"  PITCH Motor: FC:C2:3D:6D:B4:2A on {pitch_motor.SERIAL_PORT}")
            print(f"  YAW Motor:   {yaw_mac} on {yaw_port}")
            
            self.pitch_motor = pitch_motor
            self.yaw_motor = yaw_motor
            return True
            
        elif pitch_motor and not discovered_motors:
            print(f"\nFound PITCH motor but no additional motor for YAW control.")
            print(f"PITCH Motor: FC:C2:3D:6D:B4:2A on {pitch_motor.SERIAL_PORT}")
            print("Will control pitch motor only.")
            self.pitch_motor = pitch_motor
            return True
            
        else:
            print("\nFailed to find motors!")
            if not pitch_motor:
                print("  - Pitch motor (FC:C2:3D:6D:B4:2A) not found")
            return False

    def setup_motor(self, motor, motor_name):
        """Initialize motor configuration for velocity control."""
        print(f"Setting up {motor_name} motor configuration...")
        
        try:
            # Set motor to velocity mode
            set_motor_state_msg = SetMotorStateMessage(MotorStates.VELOCITY)
            motor.send_message(set_motor_state_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
            if not response:
                raise Exception(f"Failed to set {motor_name} motor state to velocity mode")
            
            # Set maximum speed for velocity moves
            set_max_speed_msg = SetMaxSpeedMessage(int(self.max_speed))
            motor.send_message(set_max_speed_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
            if not response:
                raise Exception(f"Failed to set {motor_name} max speed")
            
            # Set acceleration
            set_acceleration_msg = SetAccelerationMessage(1000)
            motor.send_message(set_acceleration_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
            if not response:
                raise Exception(f"Failed to set {motor_name} acceleration")
            
            # Set initial velocity to 0
            self.set_velocity(motor, 0.0)
            
            print(f"{motor_name} motor configuration complete!")
            
        except Exception as e:
            print(f"Error during {motor_name} motor setup: {e}")
            raise e
    
    def set_velocity(self, motor, velocity):
        """Set the motor velocity."""
        try:
            # Clamp velocity to max limits
            velocity = max(-self.max_velocity, min(self.max_velocity, velocity))
            
            set_velocity_msg = SetVelocityMessage(velocity)
            motor.send_message(set_velocity_msg)
            response = motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=1)
            
            return response is not None
                
        except Exception as e:
            print(f"Error setting velocity: {e}")
            return False
    
    def set_pitch_velocity(self, velocity):
        """Set pitch motor velocity."""
        if not self.pitch_motor:
            return
            
        if self.set_velocity(self.pitch_motor, velocity):
            self.current_pitch_velocity = velocity
    
    def set_yaw_velocity(self, velocity):
        """Set yaw motor velocity."""
        if not self.yaw_motor:
            return
            
        if self.set_velocity(self.yaw_motor, velocity):
            self.current_yaw_velocity = velocity
    
    def stop_motor(self, motor, motor_name):
        """Stop the specified motor by setting velocity to 0."""
        if not motor:
            return
            
        if self.set_velocity(motor, 0.0):
            if motor == self.pitch_motor:
                self.current_pitch_velocity = 0.0
            elif motor == self.yaw_motor:
                self.current_yaw_velocity = 0.0
    
    def stop_all_motors(self):
        """Stop both motors."""
        if self.pitch_motor:
            self.stop_motor(self.pitch_motor, "Pitch")
        if self.yaw_motor:
            self.stop_motor(self.yaw_motor, "Yaw")
    
    def run_gui(self):
        """Initialize and run the GUI interface."""
        print("Starting GUI...")
        
        # Discover and setup motors
        if not self.discover_motors():
            messagebox.showerror("Motor Discovery Failed", "Failed to discover motors. Please check connections.")
            return
        
        print("Setting up motors...")
        try:
            if self.pitch_motor:
                self.setup_motor(self.pitch_motor, "Pitch")
            if self.yaw_motor:
                self.setup_motor(self.yaw_motor, "Yaw")
        except Exception as e:
            messagebox.showerror("Motor Setup Failed", f"Motor setup failed: {e}")
            return
        
        # Create and run GUI
        gui = GimbalGUI(self)
        gui.run()
    
    def cleanup(self):
        """Clean up resources and stop motors."""
        print("Cleaning up...")
        
        # Stop the motors
        try:
            if self.pitch_motor:
                stop_motor_msg = SetMotorStateMessage(MotorStates.OFF)
                self.pitch_motor.send_message(stop_motor_msg)
                self.pitch_motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
                print("Pitch motor stopped")
                
            if self.yaw_motor:
                stop_motor_msg = SetMotorStateMessage(MotorStates.OFF)
                self.yaw_motor.send_message(stop_motor_msg)
                self.yaw_motor.wait_message(expected_bytes=MESSAGE_LENGTHS[MessageTypes.ACK_ID], timeout=2)
                print("Yaw motor stopped")
                
        except Exception as e:
            print(f"Error stopping motors: {e}")
        
        print("Cleanup complete")
    

def main():
    """Main entry point."""
    try:
        print("Initializing Dual Motor Gimbal Velocity Controller with GUI...")
        controller = DualMotorGimbalController(baudrate=115200)
        controller.run_gui()
        
    except Exception as e:
        print(f"Failed to initialize dual motor gimbal velocity controller: {e}")
        print("Please check that:")
        print("  1. The devices are connected")
        print("  2. You have permission to access the serial ports")
        print("  3. The pitch motor (FC:C2:3D:6D:B4:2A) is connected")
        messagebox.showerror("Initialization Error", str(e))
        sys.exit(1)


if __name__ == "__main__":
    main()