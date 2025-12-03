#!/usr/bin/env python3
"""
UDP Position Control Test for Axis Driver

This test demonstrates position control using the AxisUDP class.
It includes comprehensive testing of position-related commands and responses.

Requirements:
- Axis Driver device connected to network
- Device configured with appropriate IP address and port
- Device firmware supporting the Axis protocol

Usage:
    python UDPPositionControlTest.py

Make sure to update the DEVICE_IP and DEVICE_PORT constants below to match your device.
"""

import time
import sys
import struct
import threading
from typing import Optional

# Import the Axis UDP communication class and protocol functions
from Axis import AxisUDP
from AxisProtocol import (
    # Message creation functions
    GetVersionMessage, SetMotorStateMessage, GetMotorStateMessage,
    SetMaxSpeedMessage, GetMaxSpeedMessage, SetAccelerationMessage, GetAccelerationMessage,
    SetCurrentPositionMessage, GetCurrentPositionMessage,
    SetTargetPositionMessage, GetTargetPositionMessage,
    SetRelativeTargetPositionMessage, HomeMessage, GetHomedStateMessage,
    SetLedColorMessage, SaveConfigurationMessage,
    
    # Enums and constants
    MotorStates, MESSAGE_LENGTHS, MessageTypes,
    
    # Response parsing functions
    parse_get_current_position_response, parse_get_target_position_response,
    parse_get_motor_state_response, parse_get_max_speed_response,
    parse_get_acceleration_response, parse_get_homed_state_response,
    parse_ack_message, verify_checksum
)

# Device configuration - UPDATE THESE TO MATCH YOUR DEVICE
DEVICE_IP = "192.168.1.222"  # Replace with your device's IP address
DEVICE_PORT = 8080           # Replace with your device's port

# Test configuration
POSITION_TOLERANCE = 0.5     # Degrees tolerance for position verification
COMMAND_TIMEOUT = 3.0        # Timeout for commands in seconds
MOVEMENT_TIMEOUT = 5.0       # Timeout for movement completion in seconds
MAX_RETRIES = 3              # Maximum number of retries for failed commands


class PositionControlTest:
    """
    Test class for UDP-based position control of Axis Driver.
    """
    
    def __init__(self, device_ip: str, device_port: int):
        """
        Initialize the test with device connection parameters.
        
        Args:
            device_ip: IP address of the Axis Driver device
            device_port: UDP port of the Axis Driver device
        """
        self.device_ip = device_ip
        self.device_port = device_port
        self.axis = None
        self.test_count = 0
        self.passed_count = 0
        
    def connect(self) -> bool:
        """
        Establish UDP connection to the device.
        
        Returns:
            True if connection successful, False otherwise
        """
        try:
            self.axis = AxisUDP(self.device_ip, self.device_port)
            print(f"Connected to Axis Driver at {self.device_ip}:{self.device_port}")
            
            # Verify connection with version request
            if self._verify_connection():
                return True
            else:
                print("Failed to verify connection")
                return False
                
        except Exception as e:
            print(f"Error connecting to device: {e}")
            return False
    
    def _verify_connection(self) -> bool:
        """
        Verify connection by requesting device version.
        
        Returns:
            True if device responds correctly, False otherwise
        """
        try:
            version_msg = GetVersionMessage()
            self.axis.send_message(version_msg)
            
            response = self.axis.wait_message(timeout=COMMAND_TIMEOUT)
            if response and len(response) >= 10:
                major, minor, patch, build = struct.unpack("<BBBB", response[4:8])
                print(f"Device firmware version: {major}.{minor}.{patch}.{build}")
                return True
            else:
                print("No valid version response received")
                return False
                
        except Exception as e:
            print(f"Error verifying connection: {e}")
            return False
    
    def _send_command_with_ack(self, message: bytes, test_name: str, retries: int = MAX_RETRIES) -> bool:
        """
        Send a command and wait for ACK response with retries.
        
        Args:
            message: The message bytes to send
            test_name: Name of the test for error reporting
            retries: Number of retry attempts
            
        Returns:
            True if ACK received, False otherwise
        """
        for attempt in range(retries):
            try:
                self.axis.send_message(message)
                response = self.axis.wait_message(timeout=COMMAND_TIMEOUT)
                
                if response and len(response) >= 7:
                    # Check for ACK (message type 0x0100)
                    msg_type = struct.unpack("<H", response[:2])[0]
                    if msg_type == 0x0100:  # ACK message type
                        ack_code = struct.unpack("<B", response[6:7])[0]
                        if ack_code == 0:  # Success
                            return True
                        else:
                            print(f"  NACK received for {test_name} (code: {ack_code})")
                    
                if attempt < retries - 1:
                    print(f"  Attempt {attempt + 1} failed for {test_name}, retrying...")
                    time.sleep(0.1)
                    
            except Exception as e:
                if attempt < retries - 1:
                    print(f"  Attempt {attempt + 1} failed for {test_name}: {e}, retrying...")
                    time.sleep(0.1)
                else:
                    print(f"  Error in {test_name}: {e}")
        
        return False
    
    def _send_query_command(self, message: bytes, expected_length: int, test_name: str, retries: int = MAX_RETRIES) -> Optional[bytes]:
        """
        Send a query command and wait for response with retries.
        
        Args:
            message: The message bytes to send
            expected_length: Expected length of response
            test_name: Name of the test for error reporting
            retries: Number of retry attempts
            
        Returns:
            Response bytes if successful, None otherwise
        """
        for attempt in range(retries):
            try:
                self.axis.send_message(message)
                response = self.axis.wait_message(timeout=COMMAND_TIMEOUT)
                
                if response and len(response) >= expected_length:
                    if verify_checksum(response):
                        return response
                    else:
                        print(f"  Checksum verification failed for {test_name}")
                        
                if attempt < retries - 1:
                    print(f"  Attempt {attempt + 1} failed for {test_name}, retrying...")
                    time.sleep(0.1)
                    
            except Exception as e:
                if attempt < retries - 1:
                    print(f"  Attempt {attempt + 1} failed for {test_name}: {e}, retrying...")
                    time.sleep(0.1)
                else:
                    print(f"  Error in {test_name}: {e}")
        
        return None
    
    def _run_test(self, test_func, test_name: str) -> bool:
        """
        Run a single test function and track results.
        
        Args:
            test_func: Test function to execute
            test_name: Name of the test
            
        Returns:
            True if test passed, False otherwise
        """
        self.test_count += 1
        print(f"\n[Test {self.test_count}] {test_name}")
        print("-" * 50)
        
        try:
            result = test_func()
            if result:
                self.passed_count += 1
                print(f"✓ PASSED: {test_name}")
            else:
                print(f"✗ FAILED: {test_name}")
            return result
        except Exception as e:
            print(f"✗ ERROR: {test_name} - {e}")
            return False
    
    def test_motor_initialization(self) -> bool:
        """Test motor initialization and state setup."""
        print("Setting up motor for position control...")
        
        # Set motor to position mode
        msg = SetMotorStateMessage(MotorStates.POSITION)
        if not self._send_command_with_ack(msg, "Set Motor State to POSITION"):
            return False
        
        # Verify motor state
        query_msg = GetMotorStateMessage()
        response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_MOTOR_STATE_ID], "Get Motor State")
        if response:
            try:
                state = parse_get_motor_state_response(response)
                print(f"  Motor state: {state.name}")
                return state == MotorStates.POSITION
            except Exception as e:
                print(f"  Error parsing motor state: {e}")
                return False
        return False
    
    def test_speed_and_acceleration_setup(self) -> bool:
        """Test setting up speed and acceleration parameters."""
        print("Configuring motor speed and acceleration...")
        
        # Set maximum speed (degrees/second equivalent)
        max_speed = 1000  # Adjust based on your motor
        msg = SetMaxSpeedMessage(max_speed)
        if not self._send_command_with_ack(msg, "Set Max Speed"):
            return False
        
        # Verify max speed
        query_msg = GetMaxSpeedMessage()
        response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_MAX_SPEED_ID], "Get Max Speed")
        if response:
            try:
                speed = parse_get_max_speed_response(response)
                print(f"  Max speed set to: {speed}")
            except Exception as e:
                print(f"  Error parsing max speed: {e}")
        
        # Set acceleration
        acceleration = 2000  # Adjust based on your motor
        msg = SetAccelerationMessage(acceleration)
        if not self._send_command_with_ack(msg, "Set Acceleration"):
            return False
        
        # Verify acceleration
        query_msg = GetAccelerationMessage()
        response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_ACCELERATION_ID], "Get Acceleration")
        if response:
            try:
                accel = parse_get_acceleration_response(response)
                print(f"  Acceleration set to: {accel}")
                return True
            except Exception as e:
                print(f"  Error parsing acceleration: {e}")
                return False
        return False
    
    def test_position_zeroing(self) -> bool:
        """Test setting current position to zero (reference position)."""
        print("Setting current position to zero...")
        
        # Set current position to 0.0
        msg = SetCurrentPositionMessage(0.0)
        if not self._send_command_with_ack(msg, "Set Current Position to 0"):
            return False
        
        # Verify current position
        query_msg = GetCurrentPositionMessage()
        response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], "Get Current Position")
        if response:
            try:
                position = parse_get_current_position_response(response)
                print(f"  Current position: {position:.2f}°")
                return abs(position) < POSITION_TOLERANCE
            except Exception as e:
                print(f"  Error parsing current position: {e}")
                return False
        return False
    
    def test_absolute_positioning(self) -> bool:
        """Test absolute position control."""
        print("Testing absolute position control...")
        
        test_positions = [45.0, 90.0, -45.0, 180.0, 0.0]
        
        for target_pos in test_positions:
            print(f"  Moving to {target_pos}°...")
            
            # Set target position
            msg = SetTargetPositionMessage(target_pos)
            if not self._send_command_with_ack(msg, f"Set Target Position to {target_pos}"):
                return False
            
            # Wait for movement to complete (with timeout)
            start_time = time.time()
            while time.time() - start_time < MOVEMENT_TIMEOUT:
                query_msg = GetCurrentPositionMessage()
                response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], "Get Current Position")
                if response:
                    try:
                        current_pos = parse_get_current_position_response(response)
                        if abs(current_pos - target_pos) < POSITION_TOLERANCE:
                            print(f"    ✓ Reached {current_pos:.2f}° (target {target_pos}°)")
                            break
                    except Exception as e:
                        print(f"    Error parsing position: {e}")
                
                time.sleep(0.1)  # Small delay between position checks
            else:
                print(f"    ✗ Timeout waiting to reach {target_pos}°")
                return False
            
            time.sleep(0.5)  # Brief pause between movements
        
        return True
    
    def test_relative_positioning(self) -> bool:
        """Test relative position control."""
        print("Testing relative position control...")
        
        # Get initial position
        query_msg = GetCurrentPositionMessage()
        response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], "Get Initial Position")
        if not response:
            return False
        
        try:
            initial_pos = parse_get_current_position_response(response)
            print(f"  Initial position: {initial_pos:.2f}°")
        except Exception as e:
            print(f"  Error parsing initial position: {e}")
            return False
        
        # Test relative movements
        relative_moves = [30.0, -60.0, 45.0, -15.0]
        current_expected = initial_pos
        
        for rel_move in relative_moves:
            print(f"  Relative move: {rel_move:+.1f}°...")
            current_expected += rel_move
            
            # Send relative position command
            msg = SetRelativeTargetPositionMessage(rel_move)
            if not self._send_command_with_ack(msg, f"Relative Move {rel_move}"):
                return False
            
            # Wait for movement to complete
            start_time = time.time()
            while time.time() - start_time < MOVEMENT_TIMEOUT:
                query_msg = GetCurrentPositionMessage()
                response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], "Get Current Position")
                if response:
                    try:
                        current_pos = parse_get_current_position_response(response)
                        if abs(current_pos - current_expected) < POSITION_TOLERANCE:
                            print(f"    ✓ Position: {current_pos:.2f}° (expected {current_expected:.2f}°)")
                            break
                    except Exception as e:
                        print(f"    Error parsing position: {e}")
                
                time.sleep(0.1)
            else:
                print(f"    ✗ Timeout waiting for relative move {rel_move}°")
                return False
            
            time.sleep(0.3)  # Brief pause between movements
        
        return True
    
    def test_position_readback(self) -> bool:
        """Test position readback accuracy."""
        print("Testing position readback accuracy...")
        
        test_positions = [0.0, 90.0, 180.0, -90.0, 270.0]
        
        for target_pos in test_positions:
            # Set target position
            msg = SetTargetPositionMessage(target_pos)
            if not self._send_command_with_ack(msg, f"Set Target to {target_pos}"):
                return False
            
            time.sleep(1.0)  # Wait for movement
            
            # Read back current position
            query_msg = GetCurrentPositionMessage()
            response = self._send_query_command(query_msg, MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], "Get Current Position")
            if response:
                try:
                    current_pos = parse_get_current_position_response(response)
                    error = abs(current_pos - target_pos)
                    print(f"  Target: {target_pos:6.1f}° | Actual: {current_pos:6.2f}° | Error: {error:5.2f}°")
                    
                    if error > POSITION_TOLERANCE:
                        print(f"    ✗ Position error exceeds tolerance ({POSITION_TOLERANCE}°)")
                        return False
                except Exception as e:
                    print(f"  Error parsing position: {e}")
                    return False
            else:
                return False
        
        return True
    
    def test_visual_feedback(self) -> bool:
        """Test visual feedback using LED during positioning."""
        print("Testing LED feedback during positioning...")
        
        # Set LED to green for successful operation
        led_msg = SetLedColorMessage(0, 255, 0)  # Green
        if not self._send_command_with_ack(led_msg, "Set LED Green"):
            return False
        
        print("  LED set to green - moving through test positions")
        
        # Move through positions with LED feedback
        positions = [45.0, 135.0, 225.0, 315.0, 0.0]
        
        for i, pos in enumerate(positions):
            # Change LED color based on position
            colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0), (255, 0, 255)]
            r, g, b = colors[i % len(colors)]
            
            led_msg = SetLedColorMessage(r, g, b)
            self._send_command_with_ack(led_msg, f"Set LED Color {i+1}")
            
            # Move to position
            pos_msg = SetTargetPositionMessage(pos)
            if not self._send_command_with_ack(pos_msg, f"Move to {pos}°"):
                return False
            
            time.sleep(1.5)  # Wait for movement and LED change
        
        # Set LED back to blue (idle)
        led_msg = SetLedColorMessage(0, 0, 255)
        self._send_command_with_ack(led_msg, "Set LED Blue")
        
        return True
    
    def test_save_configuration(self) -> bool:
        """Test saving current configuration."""
        print("Saving configuration to device...")
        
        msg = SaveConfigurationMessage(True)
        if self._send_command_with_ack(msg, "Save Configuration"):
            print("  ✓ Configuration saved successfully")
            return True
        else:
            print("  ✗ Failed to save configuration")
            return False
    
    def run_all_tests(self) -> bool:
        """
        Run all position control tests.
        
        Returns:
            True if all tests pass, False otherwise
        """
        print("=" * 60)
        print("UDP POSITION CONTROL TEST SUITE")
        print("=" * 60)
        
        if not self.connect():
            print("Failed to connect to device. Exiting.")
            return False
        
        # Run test suite
        tests = [
            (self.test_motor_initialization, "Motor Initialization"),
            (self.test_speed_and_acceleration_setup, "Speed & Acceleration Setup"),
            (self.test_position_zeroing, "Position Zeroing"),
            (self.test_absolute_positioning, "Absolute Positioning"),
            (self.test_relative_positioning, "Relative Positioning"),
            (self.test_position_readback, "Position Readback Accuracy"),
            (self.test_visual_feedback, "Visual LED Feedback"),
            (self.test_save_configuration, "Save Configuration"),
        ]
        
        all_passed = True
        for test_func, test_name in tests:
            if not self._run_test(test_func, test_name):
                all_passed = False
                if input("\nContinue with remaining tests? (y/n): ").lower() != 'y':
                    break
        
        # Print summary
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        print(f"Total tests: {self.test_count}")
        print(f"Passed: {self.passed_count}")
        print(f"Failed: {self.test_count - self.passed_count}")
        
        if all_passed and self.passed_count == self.test_count:
            print("\n🎉 ALL TESTS PASSED! Position control is working correctly.")
        else:
            print(f"\n⚠️  Some tests failed. Please check device connection and configuration.")
        
        return all_passed and self.passed_count == self.test_count


def main():
    """Main function to run the position control test."""
    print("Axis Driver UDP Position Control Test")
    print(f"Target device: {DEVICE_IP}:{DEVICE_PORT}")
    print("\nMake sure your device is:")
    print("- Connected to the network")
    print("- Configured with the correct IP address and port")
    print("- Powered on and ready for communication")
    
    if input("\nProceed with test? (y/n): ").lower() != 'y':
        print("Test cancelled.")
        return
    
    # Create and run test
    test = PositionControlTest(DEVICE_IP, DEVICE_PORT)
    success = test.run_all_tests()
    
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())