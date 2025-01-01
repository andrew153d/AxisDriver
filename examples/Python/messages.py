from enum import Enum
import struct


class MessageTypes(Enum):
    # General Device Info
    DeviceInfoMessageTypeLowerBounds = 0x0001
    GetVersion = 0x0002
    DeviceInfoMessageTypeUpperBounds = 0x00FF

    # Device Settings
    SetI2CAddress = 0x0500
    GetI2CAddress = 0x0501

    # LED control
    LedControlMessageTypeLowerBounds = 0x3000
    SetLedColor = 0x3001
    GetLedColor = 0x3002
    SetLedState = 0x3003
    LedControlMessageTypeUpperBounds = 0x30FF
    
    # Ethernet Configuration
    SetEthernetAddress = 0x4000
    SetEthernetPort = 0x4001

    # Drive Configuration

    # Motor Driving
    MotorControlMessageTypeLowerBounds = 0x5000
    GetMotorPosition = 0x5001
    GetMotorVelocity = 0x5002
    GetMotorCurrent = 0x5003
    SetMotorPosition = 0x5004
    SetMotorVelocity = 0x5005
    SetMotorCurrent = 0x5006
    SetControlMode = 0x5007
    GetControlMode = 0x5008
    GetMotorStatus = 0x5009
    StartMotor = 0x500A
    MotorControlMessageTypeUpperBounds = 0x5FFF

    # Encoder Messages


class Header:
    def __init__(self, message_type, body_size):
        self.message_type = message_type
        self.body_size = body_size

    def pack(self):
        return struct.pack('<HH', self.message_type.value, self.body_size)

    @staticmethod
    def unpack(data):
        message_type, body_size = struct.unpack('<HH', data)
        return Header(MessageTypes(message_type), body_size)

class Footer:
    def __init__(self, checksum):
        self.checksum = checksum

    def pack(self):
        return struct.pack('<H', self.checksum)

    @staticmethod
    def unpack(data):
        checksum, = struct.unpack('<H', data)
        return Footer(checksum)

HEADER_SIZE = struct.calcsize('<HH')
FOOTER_SIZE = struct.calcsize('<H')

class GetVersionMessage:
    def __init__(self, header, footer):
        self.header = header
        self.footer = footer

    def pack(self):
        return self.header.pack() + self.footer.pack()

    @staticmethod
    def unpack(data):
        header = Header.unpack(data[:HEADER_SIZE])
        footer = Footer.unpack(data[HEADER_SIZE:])
        return GetVersionMessage(header, footer)

class VersionMessage:
    def __init__(self, header, version, footer):
        self.header = header
        self.version = version
        self.footer = footer

    def pack(self):
        return self.header.pack() + struct.pack('<32s', self.version.encode('utf-8')) + self.footer.pack()

    @staticmethod
    def unpack(data):
        header = Header.unpack(data[:HEADER_SIZE])
        version = struct.unpack('<32s', data[HEADER_SIZE:HEADER_SIZE+32])[0].decode('utf-8').rstrip('\x00')
        footer = Footer.unpack(data[HEADER_SIZE+32:])
        return VersionMessage(header, version, footer)

class SetIpAddressMessage:
    def __init__(self, header, ip_address, footer):
        self.header = header
        self.ip_address = ip_address
        self.footer = footer

    def pack(self):
        return self.header.pack() + struct.pack('<I', self.ip_address) + self.footer.pack()

    @staticmethod
    def unpack(data):
        header = Header.unpack(data[:HEADER_SIZE])
        ip_address = struct.unpack('<I', data[HEADER_SIZE:HEADER_SIZE+4])[0]
        footer = Footer.unpack(data[HEADER_SIZE+4:])
        return SetIpAddressMessage(header, ip_address, footer)

class LedColors:
    def __init__(self, r, g, b):
        self.r = r
        self.g = g
        self.b = b

    def pack(self):
        return struct.pack('<BBB', self.r, self.g, self.b)

    @staticmethod
    def unpack(data):
        r, g, b = struct.unpack('<BBB', data)
        return LedColors(r, g, b)

class SetLedColorMessage:
    def __init__(self, header, ledColor, footer):
        self.header = header
        self.ledColor = ledColor
        self.footer = footer

    def pack(self):
        return self.header.pack() + self.ledColor.pack() + self.footer.pack()

    @staticmethod
    def unpack(data):
        header = Header.unpack(data[:HEADER_SIZE])
        ledColor = LedColors.unpack(data[HEADER_SIZE:HEADER_SIZE+3])
        footer = Footer.unpack(data[HEADER_SIZE+3:])
        return SetLedColorMessage(header, ledColor, footer)

class LedStates(Enum):
    OFF = 0
    FLASH_ERROR = 1
    ERROR = 2
    BOOTUP = 3
    RAINBOW = 4
    SOLID = 5

class SetLedStateMessage:
    def __init__(self, header, ledState, footer):
        self.header = header
        self.ledState = ledState
        self.footer = footer

    def pack(self):
        return self.header.pack() + struct.pack('<B', self.ledState.value) + self.footer.pack()

    @staticmethod
    def unpack(data):
        header = Header.unpack(data[:HEADER_SIZE])
        ledState = LedStates(struct.unpack('<B', data[HEADER_SIZE:HEADER_SIZE+1])[0])
        footer = Footer.unpack(data[HEADER_SIZE+1:])
        return SetLedStateMessage(header, ledState, footer)
    
def create_led_state_message(led_state):
    header = Header(MessageTypes.SetLedState, 1)
    footer = Footer(0)
    return SetLedStateMessage(header, led_state, footer)

def create_get_version_message():
        header = Header(MessageTypes.GetVersion, 0)
        footer = Footer(0)
        return GetVersionMessage(header, footer)

def create_version_message(version):
    header = Header(MessageTypes.GetVersion, len(version))
    footer = Footer(0)
    return VersionMessage(header, version, footer)

def create_set_ip_address_message(ip_address):
    header = Header(MessageTypes.SetI2CAddress, 4)
    footer = Footer(0)
    return SetIpAddressMessage(header, ip_address, footer)

def create_set_led_color_message(r, g, b):
    header = Header(MessageTypes.SetLedColor, 3)
    footer = Footer(0)
    led_color = LedColors(r, g, b)
    return SetLedColorMessage(header, led_color, footer)
