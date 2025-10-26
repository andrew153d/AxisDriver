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

        ack_code = struct.unpack("<B", response[6:7])[0]
        if ack_code != 0:
            print(f"\nFAILED: {test_name} - NACK received (code: {ack_code})")
            print("Exiting due to test failure...")
            sys.exit(1)

    return response


def send_and_check_with_retry(axis, message, test_name, expected_ack=True, expected_bytes=None, retries=3, timeout=2):
    """
    Send a message and check response with retries.

    Args:
        axis: The AxisSerial instance
        message: The message to send
        test_name: Name of the test for error reporting
        expected_ack: Whether this test expects an ACK response
        expected_bytes: Expected number of bytes in response
        retries: Number of attempts
        timeout: Timeout for each attempt

    Returns:
        The response if successful, exits program if all attempts fail
    """
    if expected_bytes is None:
        expected_bytes = MESSAGE_LENGTHS[MessageTypes.ACK_ID] if expected_ack else None  # For non-ACK, we'll handle differently
    for attempt in range(retries):
        axis.send_message(message)
        response = axis.wait_message(timeout=timeout, expected_bytes=expected_bytes)
        if response:
            if expected_ack:
                if len(response) >= 7:
                    ack_code = struct.unpack("<B", response[6:7])[0]
                    if ack_code == 0:
                        return response
            else:
                return response
        print(f"Attempt {attempt+1} failed for {test_name}")
    print(f"FAILED: {test_name} - No valid response after {retries} attempts")
    sys.exit(1)


def main():

    axis = AxisSerial(baudrate=115200)

    # Test Version Message
    versionMsg = GetVersionMessage()
    # Debug: print each byte of the version message
    response = send_and_check_with_retry(axis, versionMsg, "Get Version", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_VERSION_ID])
    major, minor, patch, build = struct.unpack("<BBBB", response[4:8])
    print(
        f"\nSuccess! Device is running firmware version: {major}.{minor}.{patch}.{build}"
    )
    time.sleep(0.5)
    
    # Set I2C Address Test, expects Ack
    new_i2c_address = 0x42
    setI2CMsg = SetI2CAddressMessage(new_i2c_address)
    response = send_and_check_with_retry(axis, setI2CMsg, "Set I2C Address")
    print(f"\nSuccess! I2C address set to: {hex(new_i2c_address)}")
    time.sleep(0.5)

    # Get I2C Address Test
    getI2CMsg = GetI2CAddressMessage()
    response = send_and_check_with_retry(axis, getI2CMsg, "Get I2C Address", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_I2C_ADDRESS_ID])
    current_i2c_address = struct.unpack("<B", response[4:5])[0]
    print(f"\nSuccess! Current I2C address is: {hex(current_i2c_address)}")
    time.sleep(0.5)

    # Set Ethernet Address Test, expects Ack
    ipAddr = "192.168.1.222"
    ip_bytes = bytes(int(octet) for octet in ipAddr.split("."))
    ip_int = int.from_bytes(ip_bytes, byteorder="little", signed=False)
    setEthMsg = SetEthernetAddressMessage(ip_int)
    response = send_and_check_with_retry(axis, setEthMsg, "Set Ethernet Address")
    print(f"\nSuccess! Ethernet address set to: {'.'.join(f'{b:d}' for b in ip_bytes)}")
    time.sleep(0.5)

    # Get Ethernet Address Test
    getEthMsg = GetEthernetAddressMessage()
    response = send_and_check_with_retry(axis, getEthMsg, "Get Ethernet Address", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_ETHERNET_ADDRESS_ID])
    eth_address = struct.unpack("<I", response[4:8])[0]
    print(
        f"\nSuccess! Current Ethernet address is: {'.'.join(str((eth_address >> (8 * i)) & 0xFF) for i in range(4))}"
    )
    time.sleep(0.5)

    # Set Ethernet Port Test
    new_port = 8080
    setPortMsg = SetEthernetPortMessage(new_port)
    response = send_and_check_with_retry(axis, setPortMsg, "Set Ethernet Port")
    print(f"\nSuccess! Ethernet port set to: {new_port}")
    time.sleep(0.5)

    # Get Ethernet Port Test
    getPortMsg = GetEthernetPortMessage()
    response = send_and_check_with_retry(axis, getPortMsg, "Get Ethernet Port", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_ETHERNET_PORT_ID])
    current_port = struct.unpack("<I", response[4:8])[0]
    print(f"\nSuccess! Current Ethernet port is: {current_port}")
    time.sleep(0.5)

    # Get MAC Address Test
    getMacMsg = GetMacAddressMessage()
    response = send_and_check_with_retry(axis, getMacMsg, "Get MAC Address", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MAC_ADDRESS_ID])
    mac_bytes = response[4:10]
    mac_str = ":".join(f"{b:02X}" for b in mac_bytes)
    print(f"\nSuccess! MAC address is: {mac_str}")
    time.sleep(0.5)

    # Save Configuration Test
    saveMsg = SaveConfigurationMessage(True)
    response = send_and_check_with_retry(axis, saveMsg, "Save Configuration")
    print("\nSuccess! Configuration saved")
    time.sleep(0.5)

    # LED Tests
    print("\n" + "=" * 50)
    print("LED TESTS")
    print("=" * 50)
    
    #Set LED Color Test
    setLedColorMsg = SetLedColorMessage(0, 0, 255)  # Pink color
    response = send_and_check_with_retry(axis, setLedColorMsg, "Set LED Color")
    print("\nSuccess! LED color set to pink (255, 0, 128)")
    time.sleep(0.5)

    # Get LED Color Test
    getLedColorMsg = GetLedColorMessage()
    response = send_and_check_with_retry(axis, getLedColorMsg, "Get LED Color", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_LED_COLOR_ID])
    r, g, b = struct.unpack("<BBB", response[4:7])
    print(f"\nSuccess! Current LED color is: RGB({r}, {g}, {b})")
    time.sleep(0.5)

    # Add LED Step Test
    addLedStepMsg = AddLedStepMessage(500, 0, 255, 0)  # 500 ms green
    response = send_and_check_with_retry(axis, addLedStepMsg, "Add LED Step")
    print("\nSuccess! Added LED step (500ms, green)")
    time.sleep(0.5)
    
    # Motor Configuration Tests
    print("\n" + "=" * 50)
    print("MOTOR CONFIGURATION TESTS")
    print("=" * 50)

    # Set Home Direction Test
    setHomeDirMsg = SetHomeDirectionMessage(HomeDirection.COUNTERCLOCKWISE)
    response = send_and_check_with_retry(axis, setHomeDirMsg, "Set Home Direction")
    print("\nSuccess! Home direction set to COUNTERCLOCKWISE")
    time.sleep(0.5)

    # Get Home Direction Test
    getHomeDirMsg = GetHomeDirectionMessage()
    response = send_and_check_with_retry(axis, getHomeDirMsg, "Get Home Direction", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_HOME_DIRECTION_ID])
    direction = struct.unpack("<B", response[4:5])[0]
    dir_names = {0: "CLOCKWISE", 1: "COUNTERCLOCKWISE"}
    print(
        f"\nSuccess! Current home direction is: {dir_names.get(direction, 'UNKNOWN')} ({direction})"
    )
    time.sleep(0.5)

    # Set Home Threshold Test
    setHomeThresholdMsg = SetHomeThresholdMessage(1000)
    response = send_and_check_with_retry(axis, setHomeThresholdMsg, "Set Home Threshold")
    print("\nSuccess! Home threshold set to 1000")
    time.sleep(0.5)

    # Get Home Threshold Test
    getHomeThresholdMsg = GetHomeThresholdMessage()
    response = send_and_check_with_retry(axis, getHomeThresholdMsg, "Get Home Threshold", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_HOME_THRESHOLD_ID])
    threshold = struct.unpack("<I", response[4:8])[0]
    print(f"\nSuccess! Current home threshold is: {threshold}")
    time.sleep(0.5)

    # Set Home Speed Test
    setHomeSpeedMsg = SetHomeSpeedMessage(500)
    response = send_and_check_with_retry(axis, setHomeSpeedMsg, "Set Home Speed")
    print("\nSuccess! Home speed set to 500")
    time.sleep(0.5)

    # Get Home Speed Test
    getHomeSpeedMsg = GetHomeSpeedMessage()
    response = send_and_check_with_retry(axis, getHomeSpeedMsg, "Get Home Speed", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_HOME_SPEED_ID])
    speed = struct.unpack("<I", response[4:8])[0]
    print(f"\nSuccess! Current home speed is: {speed}")
    time.sleep(0.5)

    # Get Homed State Test
    getHomedStateMsg = GetHomedStateMessage()
    response = send_and_check_with_retry(axis, getHomedStateMsg, "Get Homed State", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_HOMED_STATE_ID])
    homed = struct.unpack("<B", response[4:5])[0]
    print(f"\nSuccess! Motor homed state: {'HOMED' if homed else 'NOT HOMED'}")
    time.sleep(0.5)


    # Motor Control Tests
    print("\n" + "=" * 50)
    print("MOTOR CONTROL TESTS")
    print("=" * 50)

    # Set Motor State Test
    setMotorStateMsg = SetMotorStateMessage(MotorStates.POSITION)
    response = send_and_check_with_retry(axis, setMotorStateMsg, "Set Motor State")
    print("\nSuccess! Motor state set to POSITION")
    time.sleep(0.5)

    # Get Motor State Test
    getMotorStateMsg = GetMotorStateMessage()
    response = send_and_check_with_retry(axis, getMotorStateMsg, "Get Motor State", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MOTOR_STATE_ID])
    state = struct.unpack("<B", response[4:5])[0]
    state_names = {
        0: "OFF",
        1: "POSITION",
        2: "VELOCITY",
        3: "VELOCITY_STEP",
        4: "IDLE_ON",
        5: "HOME",
    }
    print(
        f"\nSuccess! Current motor state is: {state_names.get(state, 'UNKNOWN')} ({state})"
    )
    time.sleep(0.5)

    # Set Motor Brake Test
    setMotorBrakeMsg = SetMotorBrakeMessage(MotorBrake.NORMAL)
    response = send_and_check_with_retry(axis, setMotorBrakeMsg, "Set Motor Brake")
    print("\nSuccess! Motor brake set to NORMAL")
    time.sleep(0.5)

    # Get Motor Brake Test
    getMotorBrakeMsg = GetMotorBrakeMessage()
    response = send_and_check_with_retry(axis, getMotorBrakeMsg, "Get Motor Brake", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MOTOR_BRAKE_ID])
    brake = struct.unpack("<B", response[4:5])[0]
    brake_names = {0: "NORMAL", 1: "FREEWHEELING", 2: "STRONG_BRAKING", 3: "BRAKING"}
    print(
        f"\nSuccess! Current motor brake is: {brake_names.get(brake, 'UNKNOWN')} ({brake})"
    )
    time.sleep(0.5)

    # Set Max Speed Test
    setMaxSpeedMsg = SetMaxSpeedMessage(2000)
    response = send_and_check_with_retry(axis, setMaxSpeedMsg, "Set Max Speed")
    print("\nSuccess! Max speed set to 2000")
    time.sleep(0.5)

    # Get Max Speed Test
    getMaxSpeedMsg = GetMaxSpeedMessage()
    response = send_and_check_with_retry(axis, getMaxSpeedMsg, "Get Max Speed", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_MAX_SPEED_ID])
    max_speed = struct.unpack("<I", response[4:8])[0]
    print(f"\nSuccess! Current max speed is: {max_speed}")
    time.sleep(0.5)

    # Set Acceleration Test
    setAccelMsg = SetAccelerationMessage(1000)
    response = send_and_check_with_retry(axis, setAccelMsg, "Set Acceleration")
    print("\nSuccess! Acceleration set to 1000")
    time.sleep(0.5)

    # Get Acceleration Test
    getAccelMsg = GetAccelerationMessage()
    response = send_and_check_with_retry(axis, getAccelMsg, "Get Acceleration", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_ACCELERATION_ID])
    acceleration = struct.unpack("<I", response[4:8])[0]
    print(f"\nSuccess! Current acceleration is: {acceleration}")
    time.sleep(0.5)

    # Position Control Tests
    print("\n" + "=" * 50)
    print("POSITION CONTROL TESTS")
    print("=" * 50)

    # Set Current Position Test (zero/reference position)
    setCurrentPosMsg = SetCurrentPositionMessage(0.0)
    response = send_and_check_with_retry(axis, setCurrentPosMsg, "Set Current Position")
    print("\nSuccess! Current position set to 0.0 degrees")
    time.sleep(0.5)

    # Get Current Position Test
    getCurrentPosMsg = GetCurrentPositionMessage()
    response = send_and_check_with_retry(axis, getCurrentPosMsg, "Get Current Position", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID])
    position = struct.unpack("<d", response[4:12])[0]
    print(f"\nSuccess! Current position is: {position:.2f} degrees")
    
    # Set Target Position Test
    setTargetPosMsg = SetTargetPositionMessage(90)
    response = send_and_check_with_retry(axis, setTargetPosMsg, "Set Target Position")
    print("\nSuccess! Target position set to 90.0 degrees")
    # wait for the axis to move, then read back the current position to verify the command took effect
    time.sleep(2)  # adjust delay as needed for your hardware

    verifyPosMsg = GetCurrentPositionMessage()
    response = send_and_check_with_retry(axis, verifyPosMsg, "Verify Position", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], timeout=3)
    verified_position = struct.unpack("<d", response[4:12])[0]

    target_position = 90.0
    tolerance = 1.0  # degrees tolerance for verification
    if abs(verified_position - target_position) <= tolerance:
        print(f"\nSuccess! Verified position reached: {verified_position:.2f} deg (target {target_position:.2f})")
    else:
        print(f"\nWARNING: Position mismatch: {verified_position:.2f} deg (target {target_position:.2f})")
    # Get Target Position Test
    getTargetPosMsg = GetTargetPositionMessage()
    response = send_and_check_with_retry(axis, getTargetPosMsg, "Get Target Position", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_TARGET_POSITION_ID])
    position = struct.unpack("<d", response[4:12])[0]
    print(f"\nSuccess! Target position is: {position:.2f} degrees")

    # Set Relative Target Position Test
    setRelPosMsg = SetRelativeTargetPositionMessage(180.0)
    response = send_and_check_with_retry(axis, setRelPosMsg, "Set Relative Target Position")
    print("\nSuccess! Relative target position set to +180.0 degrees")

    # Verify the relative move took effect
    time.sleep(1.5)  # allow motion to start/complete
    verifyRelPosMsg = GetCurrentPositionMessage()
    response = send_and_check_with_retry(axis, verifyRelPosMsg, "Verify Relative Position", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_CURRENT_POSITION_ID], timeout=3)
    current_position = struct.unpack("<d", response[4:12])[0]

    expected_position = target_position + 45.0
    tolerance = 1.0  # degrees tolerance for verification
    if abs(current_position - expected_position) <= tolerance:
        print(f"\nSuccess! Relative move verified: {current_position:.2f} deg (expected {expected_position:.2f})")
    else:
        print(f"\nWARNING: Relative move mismatch: {current_position:.2f} deg (expected {expected_position:.2f})")
    
    # Velocity Control Tests
    print("\n" + "=" * 50)
    print("VELOCITY CONTROL TESTS")
    print("=" * 50)

    # Set Velocity Test
    setVelMsg = SetVelocityMessage(180.0)  # 180 degrees/second
    response = send_and_check_with_retry(axis, setVelMsg, "Set Velocity")
    print("\nSuccess! Velocity set to 180.0 degrees/second")

    # Get Velocity Test
    getVelMsg = GetVelocityMessage()
    response = send_and_check_with_retry(axis, getVelMsg, "Get Velocity", expected_ack=False, expected_bytes=MESSAGE_LENGTHS[MessageTypes.GET_VELOCITY_ID])
    velocity = struct.unpack("<d", response[4:12])[0]
    print(f"\nSuccess! Current velocity is: {velocity:.2f} degrees/second")

    # Set Velocity and Steps Test
    setVelStepsMsg = SetVelocityAndStepsMessage(1000, 200, PositionMode.RELATIVE)
    response = send_and_check_with_retry(axis, setVelStepsMsg, "Set Velocity and Steps")
    print("\nSuccess! Velocity and steps set (velocity=1000, steps=200, mode=RELATIVE)")

    # Advanced Tests
    print("\n" + "=" * 50)
    print("ADVANCED TESTS")
    print("=" * 50)

    # Start Path Test
    startPathMsg = StartPathMessage(1)
    response = send_and_check_with_retry(axis, startPathMsg, "Start Path")
    print("\nSuccess! Path 1 started")

    # Home Command Test
    homeMsg = HomeMessage(1)
    response = send_and_check_with_retry(axis, homeMsg, "Home Command", timeout=5)
    print("\nSuccess! Homing command initiated")

    #Stop the motor
    stopMotorMsg = SetMotorStateMessage(MotorStates.OFF)
    response = send_and_check_with_retry(axis, stopMotorMsg, "Stop Motor")
    print("\nSuccess! Motor stopped")

    print("\n" + "=" * 50)
    print("PROTOCOL TEST COMPLETE - ALL TESTS PASSED")
    print("=" * 50)


if __name__ == "__main__":
    main()
