import serial
import time
import struct
import threading
import sys
from AxisProtocol import *
from Axis import AxisSerial


def check_response_or_exit(response, test_name, expected_ack=True):
    """
    Check if response is valid and exit if test fails.
    
    Args:
        response: The response bytes from the device
        test_name: Name of the test for error reporting
        expected_ack: Whether this test expects an ACK response
        
    Returns:
        The response if successful, exits program if failed
    """
    if not response:
        print(f"\nFAILED: {test_name} - No response received")
        print("Exiting due to test failure...")
        sys.exit(1)
    
    if expected_ack:
        if len(response) < 7:
            print(f"\nFAILED: {test_name} - Invalid response length")
            print("Exiting due to test failure...")
            sys.exit(1)
        
        ack_code = struct.unpack('<B', response[6:7])[0]
        if ack_code != 0:
            print(f"\nFAILED: {test_name} - NACK received (code: {ack_code})")
            print("Exiting due to test failure...")
            sys.exit(1)
    
    return response


def main():
    
    axis = AxisSerial()
    
    # Test Version Message
    versionMsg = GetVersionMessage()
    axis.send_message(versionMsg) 
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Version", expected_ack=False)
    major, minor, patch, build = struct.unpack('<BBBB', response[4:8])
    print(f"\nSuccess! Device is running firmware version: {major}.{minor}.{patch}.{build}")

    # Set I2C Address Test, expects Ack
    new_i2c_address = 0x42
    setI2CMsg = SetI2CAddressMessage(new_i2c_address)
    axis.send_message(setI2CMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set I2C Address")
    print(f"\nSuccess! I2C address set to: {hex(new_i2c_address)}")
        
    # Get I2C Address Test
    getI2CMsg = GetI2CAddressMessage()
    axis.send_message(getI2CMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get I2C Address", expected_ack=False)
    current_i2c_address = struct.unpack('<B', response[4:5])[0]
    print(f"\nSuccess! Current I2C address is: {hex(current_i2c_address)}")
        
    # Set Ethernet Address Test, expects Ack
    ipAddr = "192.168.12.1"
    ip_bytes = bytes(int(octet) for octet in ipAddr.split('.'))
    ip_int = int.from_bytes(ip_bytes, byteorder='little', signed=False)
    setEthMsg = SetEthernetAddressMessage(ip_int)
    axis.send_message(setEthMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Ethernet Address")
    print(f"\nSuccess! Ethernet address set to: {'.'.join(f'{b:d}' for b in ip_bytes)}")
        
    # Get Ethernet Address Test
    getEthMsg = GetEthernetAddressMessage()
    axis.send_message(getEthMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Ethernet Address", expected_ack=False)
    eth_address = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current Ethernet address is: {'.'.join(str((eth_address >> (8 * i)) & 0xFF) for i in range(4))}")
        
    # Set Ethernet Port Test
    new_port = 8080
    setPortMsg = SetEthernetPortMessage(new_port)
    axis.send_message(setPortMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Ethernet Port")
    print(f"\nSuccess! Ethernet port set to: {new_port}")
        
    # Get Ethernet Port Test
    getPortMsg = GetEthernetPortMessage()
    axis.send_message(getPortMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Ethernet Port", expected_ack=False)
    current_port = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current Ethernet port is: {current_port}")
        
    # Get MAC Address Test
    getMacMsg = GetMacAddressMessage()
    axis.send_message(getMacMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get MAC Address", expected_ack=False)
    mac_bytes = response[4:10]
    mac_str = ':'.join(f'{b:02X}' for b in mac_bytes)
    print(f"\nSuccess! MAC address is: {mac_str}")
        
    # Save Configuration Test
    saveMsg = SaveConfigurationMessage(True)
    axis.send_message(saveMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Save Configuration")
    print("\nSuccess! Configuration saved")
        
    # LED Tests
    print("\n" + "="*50)
    print("LED TESTS")
    print("="*50)
    
    # Set LED State Test
    setLedStateMsg = SetLedStateMessage(LedStates.RAINBOW)
    axis.send_message(setLedStateMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set LED State")
    print("\nSuccess! LED state set to RAINBOW")
        
    # Get LED State Test
    getLedStateMsg = GetLedStateMessage()
    axis.send_message(getLedStateMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get LED State", expected_ack=False)
    led_state = struct.unpack('<B', response[4:5])[0]
    state_names = {0: "OFF", 1: "FLASH_ERROR", 2: "ERROR", 3: "BOOTUP", 4: "RAINBOW", 5: "SOLID"}
    print(f"\nSuccess! Current LED state is: {state_names.get(led_state, 'UNKNOWN')} ({led_state})")
        
    # Set LED Color Test
    setLedColorMsg = SetLedColorMessage(255, 0, 128)  # Pink color
    axis.send_message(setLedColorMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set LED Color")
    print("\nSuccess! LED color set to pink (255, 0, 128)")
        
    # Get LED Color Test
    getLedColorMsg = GetLedColorMessage()
    axis.send_message(getLedColorMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get LED Color", expected_ack=False)
    r, g, b = struct.unpack('<BBB', response[4:7])
    print(f"\nSuccess! Current LED color is: RGB({r}, {g}, {b})")
        
    # Motor Configuration Tests
    print("\n" + "="*50)
    print("MOTOR CONFIGURATION TESTS")
    print("="*50)
    
    # Set Home Direction Test
    setHomeDirMsg = SetHomeDirectionMessage(HomeDirection.COUNTERCLOCKWISE)
    axis.send_message(setHomeDirMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Home Direction")
    print("\nSuccess! Home direction set to COUNTERCLOCKWISE")
        
    # Get Home Direction Test
    getHomeDirMsg = GetHomeDirectionMessage()
    axis.send_message(getHomeDirMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Home Direction", expected_ack=False)
    direction = struct.unpack('<B', response[4:5])[0]
    dir_names = {0: "CLOCKWISE", 1: "COUNTERCLOCKWISE"}
    print(f"\nSuccess! Current home direction is: {dir_names.get(direction, 'UNKNOWN')} ({direction})")
        
    # Set Home Threshold Test
    setHomeThresholdMsg = SetHomeThresholdMessage(1000)
    axis.send_message(setHomeThresholdMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Home Threshold")
    print("\nSuccess! Home threshold set to 1000")
        
    # Get Home Threshold Test
    getHomeThresholdMsg = GetHomeThresholdMessage()
    axis.send_message(getHomeThresholdMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Home Threshold", expected_ack=False)
    threshold = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current home threshold is: {threshold}")
        
    # Set Home Speed Test
    setHomeSpeedMsg = SetHomeSpeedMessage(500)
    axis.send_message(setHomeSpeedMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Home Speed")
    print("\nSuccess! Home speed set to 500")
        
    # Get Home Speed Test
    getHomeSpeedMsg = GetHomeSpeedMessage()
    axis.send_message(getHomeSpeedMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Home Speed", expected_ack=False)
    speed = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current home speed is: {speed}")
        
    # Get Homed State Test
    getHomedStateMsg = GetHomedStateMessage()
    axis.send_message(getHomedStateMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Homed State", expected_ack=False)
    homed = struct.unpack('<B', response[4:5])[0]
    print(f"\nSuccess! Motor homed state: {'HOMED' if homed else 'NOT HOMED'}")
        
    # Motor Control Tests
    print("\n" + "="*50)
    print("MOTOR CONTROL TESTS")
    print("="*50)
    
    # Set Motor State Test
    setMotorStateMsg = SetMotorStateMessage(MotorStates.POSITION)
    axis.send_message(setMotorStateMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Motor State")
    print("\nSuccess! Motor state set to POSITION")
        
    # Get Motor State Test
    getMotorStateMsg = GetMotorStateMessage()
    axis.send_message(getMotorStateMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Motor State", expected_ack=False)
    state = struct.unpack('<B', response[4:5])[0]
    state_names = {0: "OFF", 1: "POSITION", 2: "VELOCITY", 3: "VELOCITY_STEP", 4: "IDLE_ON", 5: "HOME"}
    print(f"\nSuccess! Current motor state is: {state_names.get(state, 'UNKNOWN')} ({state})")
        
    # Set Motor Brake Test
    setMotorBrakeMsg = SetMotorBrakeMessage(MotorBrake.NORMAL)
    axis.send_message(setMotorBrakeMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Motor Brake")
    print("\nSuccess! Motor brake set to NORMAL")
        
    # Get Motor Brake Test
    getMotorBrakeMsg = GetMotorBrakeMessage()
    axis.send_message(getMotorBrakeMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Motor Brake", expected_ack=False)
    brake = struct.unpack('<B', response[4:5])[0]
    brake_names = {0: "NORMAL", 1: "FREEWHEELING", 2: "STRONG_BRAKING", 3: "BRAKING"}
    print(f"\nSuccess! Current motor brake is: {brake_names.get(brake, 'UNKNOWN')} ({brake})")
        
    # Set Max Speed Test
    setMaxSpeedMsg = SetMaxSpeedMessage(2000)
    axis.send_message(setMaxSpeedMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Max Speed")
    print("\nSuccess! Max speed set to 2000")
        
    # Get Max Speed Test
    getMaxSpeedMsg = GetMaxSpeedMessage()
    axis.send_message(getMaxSpeedMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Max Speed", expected_ack=False)
    max_speed = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current max speed is: {max_speed}")
        
    # Set Acceleration Test
    setAccelMsg = SetAccelerationMessage(1000)
    axis.send_message(setAccelMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Acceleration")
    print("\nSuccess! Acceleration set to 1000")
        
    # Get Acceleration Test
    getAccelMsg = GetAccelerationMessage()
    axis.send_message(getAccelMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Acceleration", expected_ack=False)
    acceleration = struct.unpack('<I', response[4:8])[0]
    print(f"\nSuccess! Current acceleration is: {acceleration}")
        
    # Position Control Tests
    print("\n" + "="*50)
    print("POSITION CONTROL TESTS")
    print("="*50)
    
    # Set Current Position Test (zero/reference position)
    setCurrentPosMsg = SetCurrentPositionMessage(0.0)
    axis.send_message(setCurrentPosMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Current Position")
    print("\nSuccess! Current position set to 0.0 degrees")
        
    # Get Current Position Test
    getCurrentPosMsg = GetCurrentPositionMessage()
    axis.send_message(getCurrentPosMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Current Position", expected_ack=False)
    position = struct.unpack('<d', response[4:12])[0]
    print(f"\nSuccess! Current position is: {position:.2f} degrees")
        
    # Set Target Position Test
    setTargetPosMsg = SetTargetPositionMessage(90.0)
    axis.send_message(setTargetPosMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Target Position")
    print("\nSuccess! Target position set to 90.0 degrees")
        
    # Get Target Position Test
    getTargetPosMsg = GetTargetPositionMessage()
    axis.send_message(getTargetPosMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Target Position", expected_ack=False)
    position = struct.unpack('<d', response[4:12])[0]
    print(f"\nSuccess! Target position is: {position:.2f} degrees")
        
    # Set Relative Target Position Test
    setRelPosMsg = SetRelativeTargetPositionMessage(45.0)
    axis.send_message(setRelPosMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Relative Target Position")
    print("\nSuccess! Relative target position set to +45.0 degrees")
        
    # Velocity Control Tests
    print("\n" + "="*50)
    print("VELOCITY CONTROL TESTS")
    print("="*50)
    
    # Set Velocity Test
    setVelMsg = SetVelocityMessage(180.0)  # 180 degrees/second
    axis.send_message(setVelMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Velocity")
    print("\nSuccess! Velocity set to 180.0 degrees/second")
        
    # Get Velocity Test
    getVelMsg = GetVelocityMessage()
    axis.send_message(getVelMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Get Velocity", expected_ack=False)
    velocity = struct.unpack('<d', response[4:12])[0]
    print(f"\nSuccess! Current velocity is: {velocity:.2f} degrees/second")
        
    # Set Velocity and Steps Test
    setVelStepsMsg = SetVelocityAndStepsMessage(1000, 200, PositionMode.RELATIVE)
    axis.send_message(setVelStepsMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Set Velocity and Steps")
    print("\nSuccess! Velocity and steps set (velocity=1000, steps=200, mode=RELATIVE)")
        
    # Advanced Tests
    print("\n" + "="*50)
    print("ADVANCED TESTS")
    print("="*50)
    
    # Start Path Test
    startPathMsg = StartPathMessage(1)
    axis.send_message(startPathMsg)
    response = axis.wait_message(timeout=2)
    response = check_response_or_exit(response, "Start Path")
    print("\nSuccess! Path 1 started")
        
    # Home Command Test (Note: This will actually start homing if device supports it)
    print("\nWARNING: The following test will initiate motor homing if device is connected!")
    homeMsg = HomeMessage(1)
    axis.send_message(homeMsg)
    response = axis.wait_message(timeout=5)  # Longer timeout for homing
    response = check_response_or_exit(response, "Home Command")
    print("\nSuccess! Homing command initiated")
        
    print("\n" + "="*50)
    print("PROTOCOL TEST COMPLETE - ALL TESTS PASSED")
    print("="*50)
    

if __name__ == "__main__":
    main()
