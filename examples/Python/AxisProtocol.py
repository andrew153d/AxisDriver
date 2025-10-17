"""
Axis Driver Protocol Module

This module provides functions to generate byte arrays for Axis Driver protocol messages
based on the protocol specification. Each function returns a properly formatted byte array
that can be sent to the device.

Protocol Version: 1.0.0.0
"""

import struct
from enum import IntEnum
from typing import List, Tuple

# Message Type Constants
class MessageTypes(IntEnum):
    ACK_ID = 0x0100
    GET_VERSION_ID = 0x0101
    SET_I2C_ADDRESS_ID = 0x0102
    GET_I2C_ADDRESS_ID = 0x0103
    SET_ETHERNET_ADDRESS_ID = 0x0104
    GET_ETHERNET_ADDRESS_ID = 0x0105
    SET_ETHERNET_PORT_ID = 0x0106
    GET_ETHERNET_PORT_ID = 0x0107
    GET_MAC_ADDRESS_ID = 0x0108
    SAVE_CONFIGURATION_ID = 0x0109
    SET_LED_STATE_ID = 0x010A
    GET_LED_STATE_ID = 0x010B
    SET_LED_COLOR_ID = 0x010C
    GET_LED_COLOR_ID = 0x010D
    SET_HOME_DIRECTION_ID = 0x010E
    GET_HOME_DIRECTION_ID = 0x010F
    SET_HOME_THRESHOLD_ID = 0x0110
    GET_HOME_THRESHOLD_ID = 0x0111
    SET_HOME_SPEED_ID = 0x0112
    GET_HOME_SPEED_ID = 0x0113
    GET_HOMED_STATE_ID = 0x0114
    HOME_ID = 0x0115
    SET_MOTOR_STATE_ID = 0x0116
    GET_MOTOR_STATE_ID = 0x0117
    SET_MOTOR_BRAKE_ID = 0x0118
    GET_MOTOR_BRAKE_ID = 0x0119
    SET_MAX_SPEED_ID = 0x011A
    GET_MAX_SPEED_ID = 0x011B
    SET_ACCELERATION_ID = 0x011C
    GET_ACCELERATION_ID = 0x011D
    SET_CURRENT_POSITION_ID = 0x011E
    GET_CURRENT_POSITION_ID = 0x011F
    SET_TARGET_POSITION_ID = 0x0120
    GET_TARGET_POSITION_ID = 0x0121
    SET_RELATIVE_TARGET_POSITION_ID = 0x0122
    SET_VELOCITY_ID = 0x0123
    GET_VELOCITY_ID = 0x0124
    SET_VELOCITY_AND_STEPS_ID = 0x0125
    START_PATH_ID = 0x0126

# Status Codes for ACK messages
class StatusCodes(IntEnum):
    SUCCESS = 0x0
    ERROR = 0x1
    INVALID_COMMAND = 0x2

# LED States
class LedStates(IntEnum):
    OFF = 0x0
    FLASH_ERROR = 0x1
    ERROR = 0x2
    BOOTUP = 0x3
    RAINBOW = 0x4
    SOLID = 0x5

# Motor States
class MotorStates(IntEnum):
    OFF = 0x0
    POSITION = 0x1
    VELOCITY = 0x2
    VELOCITY_STEP = 0x3
    IDLE_ON = 0x4
    HOME = 0x5

# Motor Brake Modes
class MotorBrake(IntEnum):
    NORMAL = 0x0
    FREEWHEELING = 0x1
    STRONG_BRAKING = 0x2
    BRAKING = 0x3

# Home Direction
class HomeDirection(IntEnum):
    CLOCKWISE = 0x0
    COUNTERCLOCKWISE = 0x1

# Position Mode
class PositionMode(IntEnum):
    ABSOLUTE = 0x0
    RELATIVE = 0x1

def calculate_checksum(data: bytes) -> int:
    """
    Calculate a simple 16-bit checksum for the given data.
    This is a basic implementation - the actual firmware implementation may differ.
    """
    checksum = sum(data) & 0xFFFF
    return checksum

def create_message(message_type: int, body: bytes) -> bytes:
    """
    Create a complete message with header, body, and footer.
    
    Args:
        message_type: The message type ID
        body: The message body bytes
        
    Returns:
        Complete message as bytes
    """
    # Create header: message_type (2 bytes) + body_size (2 bytes)
    header = struct.pack('<HH', message_type, len(body))
    
    # Calculate checksum of header + body
    checksum = calculate_checksum(header + body)
    
    # Create footer: checksum (2 bytes)
    footer = struct.pack('<H', checksum)
    
    return header + body + footer

# Request messages (typically have empty or minimal bodies for "get" operations)

def GetVersionMessage() -> bytes:
    """
    Create a Get Version request message.
    Note: This is a request message, so it has no body for version data.
    The response will contain the version.
    """
    return create_message(MessageTypes.GET_VERSION_ID, b'')

def AckMessage(ack_message_type: int, status: StatusCodes) -> bytes:
    """
    Create an ACK message.
    
    Args:
        ack_message_type: The message type being acknowledged
        status: Status code (SUCCESS, ERROR, INVALID_COMMAND)
    """
    body = struct.pack('<HB', ack_message_type, status)
    return create_message(MessageTypes.ACK_ID, body)

def SetI2CAddressMessage(address: int) -> bytes:
    """
    Create a Set I2C Address message.
    
    Args:
        address: I2C address (0-127)
    """
    if not (0 <= address <= 127):
        raise ValueError("I2C address must be between 0 and 127")
    
    body = struct.pack('<B', address)
    return create_message(MessageTypes.SET_I2C_ADDRESS_ID, body)

def GetI2CAddressMessage() -> bytes:
    """Create a Get I2C Address request message."""
    return create_message(MessageTypes.GET_I2C_ADDRESS_ID, b'')

def SetEthernetAddressMessage(ip_address: int) -> bytes:
    """
    Create a Set Ethernet Address message.
    
    Args:
        ip_address: IP address as uint32_t
    """
    body = struct.pack('<I', ip_address)
    return create_message(MessageTypes.SET_ETHERNET_ADDRESS_ID, body)

def SetEthernetAddressFromString(ip_string: str) -> bytes:
    """
    Create a Set Ethernet Address message from IP string.
    
    Args:
        ip_string: IP address as string (e.g., "192.168.1.100")
    """
    parts = [int(x) for x in ip_string.split('.')]
    if len(parts) != 4 or any(not (0 <= x <= 255) for x in parts):
        raise ValueError("Invalid IP address format")
    
    ip_int = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3]
    return SetEthernetAddressMessage(ip_int)

def GetEthernetAddressMessage() -> bytes:
    """Create a Get Ethernet Address request message."""
    return create_message(MessageTypes.GET_ETHERNET_ADDRESS_ID, b'')

def SetEthernetPortMessage(port: int) -> bytes:
    """
    Create a Set Ethernet Port message.
    
    Args:
        port: Port number as uint32_t
    """
    if not (0 <= port <= 65535):
        raise ValueError("Port must be between 0 and 65535")
    
    body = struct.pack('<I', port)
    return create_message(MessageTypes.SET_ETHERNET_PORT_ID, body)

def GetEthernetPortMessage() -> bytes:
    """Create a Get Ethernet Port request message."""
    return create_message(MessageTypes.GET_ETHERNET_PORT_ID, b'')

def GetMacAddressMessage() -> bytes:
    """Create a Get MAC Address request message."""
    return create_message(MessageTypes.GET_MAC_ADDRESS_ID, b'')

def SaveConfigurationMessage(save_flag: bool = True) -> bytes:
    """
    Create a Save Configuration message.
    
    Args:
        save_flag: True to save, False for no action
    """
    body = struct.pack('<B', 1 if save_flag else 0)
    return create_message(MessageTypes.SAVE_CONFIGURATION_ID, body)

def SetLedStateMessage(led_state: LedStates) -> bytes:
    """
    Create a Set LED State message.
    
    Args:
        led_state: LED state from LedStates enum
    """
    body = struct.pack('<B', led_state)
    return create_message(MessageTypes.SET_LED_STATE_ID, body)

def GetLedStateMessage() -> bytes:
    """Create a Get LED State request message."""
    return create_message(MessageTypes.GET_LED_STATE_ID, b'')

def SetLedColorMessage(red: int, green: int, blue: int) -> bytes:
    """
    Create a Set LED Color message.
    
    Args:
        red: Red component (0-255)
        green: Green component (0-255)
        blue: Blue component (0-255)
    """
    if not all(0 <= x <= 255 for x in [red, green, blue]):
        raise ValueError("RGB values must be between 0 and 255")
    
    body = struct.pack('<BBB', red, green, blue)
    return create_message(MessageTypes.SET_LED_COLOR_ID, body)

def GetLedColorMessage() -> bytes:
    """Create a Get LED Color request message."""
    return create_message(MessageTypes.GET_LED_COLOR_ID, b'')

def SetHomeDirectionMessage(direction: HomeDirection) -> bytes:
    """
    Create a Set Home Direction message.
    
    Args:
        direction: Home direction from HomeDirection enum
    """
    body = struct.pack('<B', direction)
    return create_message(MessageTypes.SET_HOME_DIRECTION_ID, body)

def GetHomeDirectionMessage() -> bytes:
    """Create a Get Home Direction request message."""
    return create_message(MessageTypes.GET_HOME_DIRECTION_ID, b'')

def SetHomeThresholdMessage(threshold: int) -> bytes:
    """
    Create a Set Home Threshold message.
    
    Args:
        threshold: Home detection threshold as uint32_t
    """
    body = struct.pack('<I', threshold)
    return create_message(MessageTypes.SET_HOME_THRESHOLD_ID, body)

def GetHomeThresholdMessage() -> bytes:
    """Create a Get Home Threshold request message."""
    return create_message(MessageTypes.GET_HOME_THRESHOLD_ID, b'')

def SetHomeSpeedMessage(speed: int) -> bytes:
    """
    Create a Set Home Speed message.
    
    Args:
        speed: Home speed as uint32_t
    """
    body = struct.pack('<I', speed)
    return create_message(MessageTypes.SET_HOME_SPEED_ID, body)

def GetHomeSpeedMessage() -> bytes:
    """Create a Get Home Speed request message."""
    return create_message(MessageTypes.GET_HOME_SPEED_ID, b'')

def GetHomedStateMessage() -> bytes:
    """Create a Get Homed State request message."""
    return create_message(MessageTypes.GET_HOMED_STATE_ID, b'')

def HomeMessage(home_command: int = 1) -> bytes:
    """
    Create a Home message to initiate homing sequence.
    
    Args:
        home_command: Home command value (default: 1)
    """
    body = struct.pack('<I', home_command)
    return create_message(MessageTypes.HOME_ID, body)

def SetMotorStateMessage(motor_state: MotorStates) -> bytes:
    """
    Create a Set Motor State message.
    
    Args:
        motor_state: Motor state from MotorStates enum
    """
    body = struct.pack('<B', motor_state)
    return create_message(MessageTypes.SET_MOTOR_STATE_ID, body)

def GetMotorStateMessage() -> bytes:
    """Create a Get Motor State request message."""
    return create_message(MessageTypes.GET_MOTOR_STATE_ID, b'')

def SetMotorBrakeMessage(brake_mode: MotorBrake) -> bytes:
    """
    Create a Set Motor Brake message.
    
    Args:
        brake_mode: Brake mode from MotorBrake enum
    """
    body = struct.pack('<B', brake_mode)
    return create_message(MessageTypes.SET_MOTOR_BRAKE_ID, body)

def GetMotorBrakeMessage() -> bytes:
    """Create a Get Motor Brake request message."""
    return create_message(MessageTypes.GET_MOTOR_BRAKE_ID, b'')

def SetMaxSpeedMessage(max_speed: int) -> bytes:
    """
    Create a Set Max Speed message.
    
    Args:
        max_speed: Maximum speed as uint32_t
    """
    body = struct.pack('<I', max_speed)
    return create_message(MessageTypes.SET_MAX_SPEED_ID, body)

def GetMaxSpeedMessage() -> bytes:
    """Create a Get Max Speed request message."""
    return create_message(MessageTypes.GET_MAX_SPEED_ID, b'')

def SetAccelerationMessage(acceleration: int) -> bytes:
    """
    Create a Set Acceleration message.
    
    Args:
        acceleration: Acceleration as uint32_t
    """
    body = struct.pack('<I', acceleration)
    return create_message(MessageTypes.SET_ACCELERATION_ID, body)

def GetAccelerationMessage() -> bytes:
    """Create a Get Acceleration request message."""
    return create_message(MessageTypes.GET_ACCELERATION_ID, b'')

def SetCurrentPositionMessage(position: float) -> bytes:
    """
    Create a Set Current Position message.
    
    Args:
        position: Position in degrees as double
    """
    body = struct.pack('<d', position)
    return create_message(MessageTypes.SET_CURRENT_POSITION_ID, body)

def GetCurrentPositionMessage() -> bytes:
    """Create a Get Current Position request message."""
    return create_message(MessageTypes.GET_CURRENT_POSITION_ID, b'')

def SetTargetPositionMessage(position: float) -> bytes:
    """
    Create a Set Target Position message.
    
    Args:
        position: Target position in degrees as double
    """
    body = struct.pack('<d', position)
    return create_message(MessageTypes.SET_TARGET_POSITION_ID, body)

def GetTargetPositionMessage() -> bytes:
    """Create a Get Target Position request message."""
    return create_message(MessageTypes.GET_TARGET_POSITION_ID, b'')

def SetRelativeTargetPositionMessage(relative_position: float) -> bytes:
    """
    Create a Set Relative Target Position message.
    
    Args:
        relative_position: Relative position in degrees as double
    """
    body = struct.pack('<d', relative_position)
    return create_message(MessageTypes.SET_RELATIVE_TARGET_POSITION_ID, body)

def SetVelocityMessage(velocity: float) -> bytes:
    """
    Create a Set Velocity message.
    
    Args:
        velocity: Velocity in degrees/second as double
    """
    body = struct.pack('<d', velocity)
    return create_message(MessageTypes.SET_VELOCITY_ID, body)

def GetVelocityMessage() -> bytes:
    """Create a Get Velocity request message."""
    return create_message(MessageTypes.GET_VELOCITY_ID, b'')

def SetVelocityAndStepsMessage(velocity: int, steps: int, position_mode: PositionMode) -> bytes:
    """
    Create a Set Velocity and Steps message.
    
    Args:
        velocity: Velocity as int32_t
        steps: Number of steps as int32_t
        position_mode: Position mode from PositionMode enum
    """
    body = struct.pack('<iiB', velocity, steps, position_mode)
    return create_message(MessageTypes.SET_VELOCITY_AND_STEPS_ID, body)

def StartPathMessage(path_id: int) -> bytes:
    """
    Create a Start Path message.
    
    Args:
        path_id: Path identifier to execute
    """
    if not (0 <= path_id <= 255):
        raise ValueError("Path ID must be between 0 and 255")
    
    body = struct.pack('<B', path_id)
    return create_message(MessageTypes.START_PATH_ID, body)

# Utility functions for parsing responses

def parse_message_header(data: bytes) -> Tuple[int, int]:
    """
    Parse message header to get message type and body size.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Tuple of (message_type, body_size)
    """
    if len(data) < 4:
        raise ValueError("Data too short for header")
    
    message_type, body_size = struct.unpack('<HH', data[:4])
    return message_type, body_size

def verify_checksum(data: bytes) -> bool:
    """
    Verify message checksum.
    
    Args:
        data: Complete message bytes including header, body, and footer
        
    Returns:
        True if checksum is valid
    """
    if len(data) < 6:  # Minimum message size (header + footer)
        return False
    
    # Extract checksum from footer (last 2 bytes)
    received_checksum = struct.unpack('<H', data[-2:])[0]
    
    # Calculate checksum of header + body (all except last 2 bytes)
    calculated_checksum = calculate_checksum(data[:-2])
    
    return received_checksum == calculated_checksum

def format_message_bytes(data: bytes, bytes_per_line: int = 16) -> str:
    """
    Format message bytes for debugging/display.
    
    Args:
        data: Message bytes
        bytes_per_line: Number of bytes to display per line
        
    Returns:
        Formatted string representation
    """
    lines = []
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        hex_part = ' '.join(f'{b:02X}' for b in chunk)
        ascii_part = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
        lines.append(f'{i:04X}: {hex_part:<{bytes_per_line*3-1}} {ascii_part}')
    return '\n'.join(lines)

# Example usage and testing
if __name__ == "__main__":
    # Example: Create some messages and display their byte representation
    print("Axis Driver Protocol Message Examples")
    print("=" * 50)
    
    # Get Version message
    version_msg = GetVersionMessage()
    print(f"Get Version Message ({len(version_msg)} bytes):")
    print(format_message_bytes(version_msg))
    print()
    
    # Set LED Color message
    led_msg = SetLedColorMessage(255, 128, 0)  # Orange color
    print(f"Set LED Color Message - Orange ({len(led_msg)} bytes):")
    print(format_message_bytes(led_msg))
    print()
    
    # Set Target Position message
    pos_msg = SetTargetPositionMessage(90.5)  # 90.5 degrees
    print(f"Set Target Position Message - 90.5° ({len(pos_msg)} bytes):")
    print(format_message_bytes(pos_msg))
    print()
    
    # Set Motor State message
    motor_msg = SetMotorStateMessage(MotorStates.POSITION)
    print(f"Set Motor State Message - POSITION ({len(motor_msg)} bytes):")
    print(format_message_bytes(motor_msg))
    print()