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
    SET_LED_COLOR_ID = 0x0200
    GET_LED_COLOR_ID = 0x0201
    ADD_LED_STEP_ID = 0x0202
    SET_HOME_DIRECTION_ID = 0x0300
    GET_HOME_DIRECTION_ID = 0x0301
    SET_HOME_THRESHOLD_ID = 0x0302
    GET_HOME_THRESHOLD_ID = 0x0303
    SET_HOME_SPEED_ID = 0x0304
    GET_HOME_SPEED_ID = 0x0305
    GET_HOMED_STATE_ID = 0x0306
    HOME_ID = 0x0400
    SET_MOTOR_STATE_ID = 0x0307
    GET_MOTOR_STATE_ID = 0x0308
    SET_MOTOR_BRAKE_ID = 0x0309
    GET_MOTOR_BRAKE_ID = 0x030A
    SET_MAX_SPEED_ID = 0x030B
    GET_MAX_SPEED_ID = 0x030C
    SET_ACCELERATION_ID = 0x030D
    GET_ACCELERATION_ID = 0x030E
    SET_CURRENT_POSITION_ID = 0x030F
    GET_CURRENT_POSITION_ID = 0x0310
    SET_TARGET_POSITION_ID = 0x0311
    GET_TARGET_POSITION_ID = 0x0312
    SET_RELATIVE_TARGET_POSITION_ID = 0x0401
    SET_VELOCITY_ID = 0x0313
    GET_VELOCITY_ID = 0x0314
    SET_VELOCITY_AND_STEPS_ID = 0x0402
    START_PATH_ID = 0x0403

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

# Message length definitions (in bytes)
MESSAGE_LENGTHS = {
    MessageTypes.ACK_ID: 9,
    MessageTypes.GET_VERSION_ID: 10,
    MessageTypes.SET_I2C_ADDRESS_ID: 7,
    MessageTypes.GET_I2C_ADDRESS_ID: 7,
    MessageTypes.SET_ETHERNET_ADDRESS_ID: 10,
    MessageTypes.GET_ETHERNET_ADDRESS_ID: 10,
    MessageTypes.SET_ETHERNET_PORT_ID: 10,
    MessageTypes.GET_ETHERNET_PORT_ID: 10,
    MessageTypes.GET_MAC_ADDRESS_ID: 12,
    MessageTypes.SAVE_CONFIGURATION_ID: 7,
    MessageTypes.SET_LED_COLOR_ID: 9,
    MessageTypes.GET_LED_COLOR_ID: 9,
    MessageTypes.ADD_LED_STEP_ID: 13,
    MessageTypes.SET_HOME_DIRECTION_ID: 7,
    MessageTypes.GET_HOME_DIRECTION_ID: 7,
    MessageTypes.SET_HOME_THRESHOLD_ID: 10,
    MessageTypes.GET_HOME_THRESHOLD_ID: 10,
    MessageTypes.SET_HOME_SPEED_ID: 10,
    MessageTypes.GET_HOME_SPEED_ID: 10,
    MessageTypes.GET_HOMED_STATE_ID: 7,
    MessageTypes.HOME_ID: 10,
    MessageTypes.SET_MOTOR_STATE_ID: 7,
    MessageTypes.GET_MOTOR_STATE_ID: 7,
    MessageTypes.SET_MOTOR_BRAKE_ID: 7,
    MessageTypes.GET_MOTOR_BRAKE_ID: 7,
    MessageTypes.SET_MAX_SPEED_ID: 10,
    MessageTypes.GET_MAX_SPEED_ID: 10,
    MessageTypes.SET_ACCELERATION_ID: 10,
    MessageTypes.GET_ACCELERATION_ID: 10,
    MessageTypes.SET_CURRENT_POSITION_ID: 14,
    MessageTypes.GET_CURRENT_POSITION_ID: 14,
    MessageTypes.SET_TARGET_POSITION_ID: 14,
    MessageTypes.GET_TARGET_POSITION_ID: 14,
    MessageTypes.SET_RELATIVE_TARGET_POSITION_ID: 14,
    MessageTypes.SET_VELOCITY_ID: 14,
    MessageTypes.GET_VELOCITY_ID: 14,
    MessageTypes.SET_VELOCITY_AND_STEPS_ID: 15,
    MessageTypes.START_PATH_ID: 7,
}

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

def AddLedStepMessage(time_ms: int, red: int, green: int, blue: int) -> bytes:
    """
    Create an Add LED Step message.
    
    Args:
        time_ms: Duration in milliseconds (uint32)
        red: Red component (0-255)
        green: Green component (0-255)
        blue: Blue component (0-255)
    """
    if time_ms < 0 or time_ms > 0xFFFFFFFF:
        raise ValueError("time_ms must fit in uint32")
    if not all(0 <= x <= 255 for x in (red, green, blue)):
        raise ValueError("RGB values must be 0..255")
    body = struct.pack('<IBBB', time_ms, red, green, blue)
    return create_message(MessageTypes.ADD_LED_STEP_ID, body)

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
    print("SetVelocityAndStepsMessage: velocity=%d, steps=%d, position_mode=%s" % (velocity, steps, position_mode))
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

# Parsing functions for response messages

def parse_ack_message(data: bytes) -> Tuple[int, StatusCodes]:
    """
    Parse an ACK message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Tuple of (ack_message_type, status)
    """
    expected_length = 9
    if len(data) != expected_length:
        raise ValueError(f"Invalid ACK message length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, ack_message_type, status, checksum = struct.unpack('<HHHBH', data)
    if message_type != MessageTypes.ACK_ID or body_size != 3:
        raise ValueError("Invalid ACK message format")
    return ack_message_type, StatusCodes(status)

def parse_get_version_response(data: bytes) -> int:
    """
    Parse a Get Version response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Firmware version as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Version response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, version, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_VERSION_ID or body_size != 4:
        raise ValueError("Invalid Get Version response format")
    return version

def parse_get_i2c_address_response(data: bytes) -> int:
    """
    Parse a Get I2C Address response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        I2C address as uint8_t
    """
    expected_length = 7
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get I2C Address response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, address, checksum = struct.unpack('<HHBH', data)
    if message_type != MessageTypes.GET_I2C_ADDRESS_ID or body_size != 1:
        raise ValueError("Invalid Get I2C Address response format")
    return address

def parse_get_ethernet_address_response(data: bytes) -> int:
    """
    Parse a Get Ethernet Address response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        IP address as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Ethernet Address response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, ip_address, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_ETHERNET_ADDRESS_ID or body_size != 4:
        raise ValueError("Invalid Get Ethernet Address response format")
    return ip_address

def parse_get_ethernet_port_response(data: bytes) -> int:
    """
    Parse a Get Ethernet Port response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Port number as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Ethernet Port response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, port, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_ETHERNET_PORT_ID or body_size != 4:
        raise ValueError("Invalid Get Ethernet Port response format")
    return port

def parse_get_mac_address_response(data: bytes) -> Tuple[int, ...]:
    """
    Parse a Get MAC Address response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        MAC address as tuple of 6 bytes
    """
    expected_length = 12
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get MAC Address response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    unpacked = struct.unpack('<HHBBBBBBH', data)
    message_type, body_size = unpacked[0], unpacked[1]
    mac = unpacked[2:8]
    checksum = unpacked[8]
    if message_type != MessageTypes.GET_MAC_ADDRESS_ID or body_size != 6:
        raise ValueError("Invalid Get MAC Address response format")
    return tuple(mac)

def parse_get_led_color_response(data: bytes) -> Tuple[int, int, int]:
    """
    Parse a Get LED Color response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        RGB values as tuple (red, green, blue)
    """
    expected_length = 9
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get LED Color response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, red, green, blue, checksum = struct.unpack('<HHBBBH', data)
    if message_type != MessageTypes.GET_LED_COLOR_ID or body_size != 3:
        raise ValueError("Invalid Get LED Color response format")
    return (red, green, blue)

def parse_get_home_direction_response(data: bytes) -> HomeDirection:
    """
    Parse a Get Home Direction response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Home direction
    """
    expected_length = 7
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Home Direction response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, direction, checksum = struct.unpack('<HHBH', data)
    if message_type != MessageTypes.GET_HOME_DIRECTION_ID or body_size != 1:
        raise ValueError("Invalid Get Home Direction response format")
    return HomeDirection(direction)

def parse_get_home_threshold_response(data: bytes) -> int:
    """
    Parse a Get Home Threshold response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Home threshold as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Home Threshold response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, threshold, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_HOME_THRESHOLD_ID or body_size != 4:
        raise ValueError("Invalid Get Home Threshold response format")
    return threshold

def parse_get_home_speed_response(data: bytes) -> int:
    """
    Parse a Get Home Speed response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Home speed as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Home Speed response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, speed, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_HOME_SPEED_ID or body_size != 4:
        raise ValueError("Invalid Get Home Speed response format")
    return speed

def parse_get_homed_state_response(data: bytes) -> bool:
    """
    Parse a Get Homed State response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Homed state (True if homed)
    """
    expected_length = 7
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Homed State response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, homed, checksum = struct.unpack('<HHBH', data)
    if message_type != MessageTypes.GET_HOMED_STATE_ID or body_size != 1:
        raise ValueError("Invalid Get Homed State response format")
    return homed != 0

def parse_get_motor_state_response(data: bytes) -> MotorStates:
    """
    Parse a Get Motor State response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Motor state
    """
    expected_length = 7
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Motor State response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, motor_state, checksum = struct.unpack('<HHBH', data)
    if message_type != MessageTypes.GET_MOTOR_STATE_ID or body_size != 1:
        raise ValueError("Invalid Get Motor State response format")
    return MotorStates(motor_state)

def parse_get_motor_brake_response(data: bytes) -> MotorBrake:
    """
    Parse a Get Motor Brake response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Motor brake mode
    """
    expected_length = 7
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Motor Brake response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, brake_mode, checksum = struct.unpack('<HHBH', data)
    if message_type != MessageTypes.GET_MOTOR_BRAKE_ID or body_size != 1:
        raise ValueError("Invalid Get Motor Brake response format")
    return MotorBrake(brake_mode)

def parse_get_max_speed_response(data: bytes) -> int:
    """
    Parse a Get Max Speed response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Max speed as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Max Speed response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, max_speed, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_MAX_SPEED_ID or body_size != 4:
        raise ValueError("Invalid Get Max Speed response format")
    return max_speed

def parse_get_acceleration_response(data: bytes) -> int:
    """
    Parse a Get Acceleration response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Acceleration as uint32_t
    """
    expected_length = 10
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Acceleration response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, acceleration, checksum = struct.unpack('<HHIH', data)
    if message_type != MessageTypes.GET_ACCELERATION_ID or body_size != 4:
        raise ValueError("Invalid Get Acceleration response format")
    return acceleration

def parse_get_current_position_response(data: bytes) -> float:
    """
    Parse a Get Current Position response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Current position as double
    """
    expected_length = 14
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Current Position response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, position, checksum = struct.unpack('<HHdH', data)
    if message_type != MessageTypes.GET_CURRENT_POSITION_ID or body_size != 8:
        raise ValueError("Invalid Get Current Position response format")
    return position

def parse_get_target_position_response(data: bytes) -> float:
    """
    Parse a Get Target Position response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Target position as double
    """
    expected_length = 14
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Target Position response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, position, checksum = struct.unpack('<HHdH', data)
    if message_type != MessageTypes.GET_TARGET_POSITION_ID or body_size != 8:
        raise ValueError("Invalid Get Target Position response format")
    return position

def parse_get_velocity_response(data: bytes) -> float:
    """
    Parse a Get Velocity response message.
    
    Args:
        data: Raw message bytes
        
    Returns:
        Velocity as double
    """
    expected_length = 14
    if len(data) != expected_length:
        raise ValueError(f"Invalid Get Velocity response length: expected {expected_length}, got {len(data)}")
    if not verify_checksum(data):
        raise ValueError("Invalid checksum")
    message_type, body_size, velocity, checksum = struct.unpack('<HHdH', data)
    if message_type != MessageTypes.GET_VELOCITY_ID or body_size != 8:
        raise ValueError("Invalid Get Velocity response format")
    return velocity

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

# Example usage code demonstrating how to send and receive messages

if __name__ == "__main__":
    import socket  # For demonstration, assuming Ethernet communication

    # Example 1: Send Get Version and receive response
    print("Example 1: Get Version")
    msg = GetVersionMessage()
    print("Sent message:", format_message_bytes(msg))
    
    # Simulate sending and receiving (replace with actual socket code)
    # sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # sock.connect(('192.168.1.100', 1234))
    # sock.send(msg)
    # response = sock.recv(1024)
    
    # Simulated response (version 1.0.0.1)
    response = b'\x01\x01\x00\x04\x01\x00\x00\x01\x00\x06'
    try:
        version = parse_get_version_response(response)
        print(f"Received version: {version}")
    except ValueError as e:
        print(f"Error parsing response: {e}")
    
    print("\nExample 2: Set LED Color")
    msg = SetLedColorMessage(255, 0, 0)  # Red
    print("Sent message:", format_message_bytes(msg))
    
    print("\nExample 3: Set Motor State to POSITION")
    msg = SetMotorStateMessage(MotorStates.POSITION)
    print("Sent message:", format_message_bytes(msg))
    
    print("\nExample 4: Set Target Position")
    msg = SetTargetPositionMessage(90.0)  # 90 degrees
    print("Sent message:", format_message_bytes(msg))
    
    print("\nExample 5: Parse ACK response")
    # Simulated ACK for successful command
    ack_response = b'\x01\x00\x00\x03\x01\x01\x00\x00\x05'
    try:
        ack_type, status = parse_ack_message(ack_response)
        print(f"ACK for message type 0x{ack_type:04X}, status: {status.name}")
    except ValueError as e:
        print(f"Error parsing ACK: {e}")
    
    print("\nExample 6: Set Velocity and Steps")
    msg = SetVelocityAndStepsMessage(1000, 100, PositionMode.ABSOLUTE)
    print("Sent message:", format_message_bytes(msg))
    
    # Note: In a real application, you would:
    # 1. Establish connection (serial, Ethernet, etc.)
    # 2. Send the packed message bytes
    # 3. Receive response bytes
    # 4. Parse the response using the appropriate parse function
    # 5. Handle any errors or timeouts