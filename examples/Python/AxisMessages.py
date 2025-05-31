
from Axis import *
import struct


# Constants
SYNC_BYTES = 0xDEADBABE  # Sync bytes for message start
HEADER_SIZE = 8  # Size of the header in bytes
FOOTER_SIZE = 2  # Size of the footer in bytes
GetVersionId = 0x0
SetI2CAddressId = 0x1
GetI2CAddressId = 0x2
SetEthernetAddressId = 0x3
GetEthernetAddressId = 0x4
SetEthernetPortId = 0x5
GetEthernetPortId = 0x6
GetMacAddressId = 0x7
SaveConfigurationId = 0x8
SetLedStateId = 0x9
GetLedStateId = 0xA
SetLedColorId = 0xB
GetLedColorId = 0xC
SetHomeDirectionId = 0xD
GetHomeDirectionId = 0xE
SetHomeThresholdId = 0xF
GetHomeThresholdId = 0x10
SetHomeSpeedId = 0x11
GetHomeSpeedId = 0x12
GetHomedStateId = 0x13
HomeId = 0x14
SetMotorStateId = 0x15
GetMotorStateId = 0x16
SetMotorBrakeId = 0x17
GetMotorBrakeId = 0x18
SetMaxSpeedId = 0x19
GetMaxSpeedId = 0x1A
SetAccelerationId = 0x1B
GetAccelerationId = 0x1C
SetCurrentPositionId = 0x1D
GetCurrentPositionId = 0x1E
SetTargetPositionId = 0x1F
GetTargetPositionId = 0x20
SetRelativeTargetPositionId = 0x21
SetVelocityId = 0x22
GetVelocityId = 0x23
SetVelocityAndStepsId = 0x24
StartPathId = 0x25
SetHAEnableId = 0x26
GetHAEnableId = 0x27
SetHAModeId = 0x28
GetHAModeId = 0x29
SetHAIpAddressId = 0x2A
GetHAIpAddressId = 0x2B
SetHAVelocitySwitchOnSpeedId = 0x2C
GetHAVelocitySwitchOnSpeedId = 0x2D
SetHAVelocitySwitchOffSpeedId = 0x2E
GetHAVelocitySwitchOffSpeedId = 0x2F
SetHAPositionSwitchOnPositionId = 0x30
GetHAPositionSwitchOnPositionId = 0x31
SetHAPositionSwitchOffPositionId = 0x32
GetHAPositionSwitchOffPositionId = 0x33
SetHAVelocitySliderMinId = 0x34
GetHAVelocitySliderMinId = 0x35
SetHAVelocitySliderMaxId = 0x36
GetHAVelocitySliderMaxId = 0x37
SetHAPositionSliderMinId = 0x38
GetHAPositionSliderMinId = 0x39
SetHAPositionSliderMaxId = 0x3A
GetHAPositionSliderMaxId = 0x3B
SetHAMqttUserId = 0x3C
GetHAMqttUserId = 0x3D
SetHAMqttPasswordId = 0x3E
GetHAMqttPasswordId = 0x3F
SetHAMqttNameId = 0x40
GetHAMqttNameId = 0x41
SetHAMqttIconId = 0x42
GetHAMqttIconId = 0x43


#LedStates
OFF = 0x0
FLASH_ERROR = 0x1
ERROR = 0x2
BOOTUP = 0x3
RAINBOW = 0x4
SOLID = 0x5
MAX_VALUE = 0x6

#MotorStates
OFF = 0x0
POSITION = 0x1
VELOCITY = 0x2
VELOCITY_STEP = 0x3
IDLE_ON = 0x4
HOME = 0x5

#MotorBrake
NORMAL = 0x0
FREEWHEELING = 0x1
STRONG_BRAKING = 0x2
BRAKING = 0x3

#HomeDirection
CLOCKWISE = 0x0
COUNTERCLOCKWISE = 0x1

#PositionMode
ABSOLUTE = 0x0
RELATIVE = 0x1

class Header: 
	def __init__(self, sync_bytes, message_type, body_size):
		self.sync_bytes = sync_bytes
		self.message_type = message_type
		self.body_size = body_size

	def serialize(self):
		ret = bytearray(0)
		ret += self.sync_bytes.to_bytes(4, 'little')
		ret += self.message_type.to_bytes(2, 'little')
		ret += self.body_size.to_bytes(2, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		sync_bytes = int.from_bytes(byte_array[0:4], 'little')
		message_type = int.from_bytes(byte_array[4:6], 'little')
		body_size = int.from_bytes(byte_array[6:8], 'little')
		return cls(sync_bytes, message_type, body_size)

class Footer: 
	def __init__(self, checksum):
		self.checksum = checksum

	def serialize(self):
		ret = bytearray(0)
		ret += self.checksum.to_bytes(2, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		checksum = int.from_bytes(byte_array[0:2], 'little')
		return cls(checksum)

class U8Message: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += self.value.to_bytes(1, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = int.from_bytes(byte_array[0:1], 'little')
		return cls(value)

class S8Message: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += self.value.to_bytes(1, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = int.from_bytes(byte_array[0:1], 'little')
		return cls(value)

class U32Message: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += self.value.to_bytes(4, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = int.from_bytes(byte_array[0:4], 'little')
		return cls(value)

class S32Message: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += self.value.to_bytes(4, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = int.from_bytes(byte_array[0:4], 'little')
		return cls(value)

class DoubleMessage: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += bytearray(struct.pack('d', self.value))
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = struct.unpack('d', byte_array[0:8])[0]
		return cls(value)

class StringMessage: 
	def __init__(self, value):
		self.value = value

	def serialize(self):
		ret = bytearray(0)
		ret += bytearray(self.value)
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		value = byte_array[0:32]
		return cls(value)

class VersionMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class I2CAddressMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class EthernetAddressMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class EthernetPortMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MacAddressBody: 
	def __init__(self, mac):
		self.mac = mac

	def serialize(self):
		ret = bytearray(0)
		ret += bytearray(self.mac)
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		mac = byte_array[0:6]
		msg = cls(mac)
		return msg

class MacAddressMessage: 
	def __init__(self, msg_id, mac):
		self.header = Header(SYNC_BYTES, msg_id, 6)
		self.body = MacAddressBody(mac)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = MacAddressBody.deserialize(byte_array[8:14])
		footer = Footer.deserialize(byte_array[14:16])
		msg = cls(header.message_type, body.mac)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class SaveConfigurationMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class LedStateMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class LedColorBody: 
	def __init__(self, ledColor):
		self.ledColor = ledColor

	def serialize(self):
		ret = bytearray(0)
		ret += bytearray(self.ledColor)
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		ledColor = byte_array[0:3]
		msg = cls(ledColor)
		return msg

class LedColorMessage: 
	def __init__(self, msg_id, ledColor):
		self.header = Header(SYNC_BYTES, msg_id, 3)
		self.body = LedColorBody(ledColor)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = LedColorBody.deserialize(byte_array[8:11])
		footer = Footer.deserialize(byte_array[11:13])
		msg = cls(header.message_type, body.ledColor)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeDirectionMessage: 
	def __init__(self, msg_id, value):
		self.body = S8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = S8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeSpeedMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeThresholdMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomedStateMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MotorStateMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MotorBrakeMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MaxSpeedMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class AccelerationMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U32Message.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class CurrentPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class TargetPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class RelativeTargetPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class VelocityMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class VelocityAndStepsBody: 
	def __init__(self, velocity, steps, positionMode):
		self.velocity = velocity
		self.steps = steps
		self.positionMode = positionMode

	def serialize(self):
		ret = bytearray(0)
		ret += struct.pack('<i', self.velocity)#self.velocity.to_bytes(4, 'little')
		ret += struct.pack('<i', self.steps)#self.steps.to_bytes(4, 'little')
		ret += struct.pack('<B', self.positionMode)#self.positionMode.to_bytes(1, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		velocity = int.from_bytes(byte_array[0:4], 'little')
		steps = int.from_bytes(byte_array[4:8], 'little')
		positionMode = int.from_bytes(byte_array[8:9], 'little')
		msg = cls(velocity, steps, positionMode)
		return msg

class VelocityAndStepsMessage: 
	def __init__(self, msg_id, velocity, steps, positionMode):
		self.header = Header(SYNC_BYTES, msg_id, 9)
		self.body = VelocityAndStepsBody(velocity, steps, positionMode)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = VelocityAndStepsBody.deserialize(byte_array[8:17])
		footer = Footer.deserialize(byte_array[17:19])
		msg = cls(header.message_type, body.velocity, body.steps, body.positionMode)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class StartPathMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAEnableMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAModeMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(SYNC_BYTES, msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = U8Message.deserialize(byte_array[8:9])
		footer = Footer.deserialize(byte_array[9:11])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAIpAddressBody: 
	def __init__(self, ha_ip_address):
		self.ha_ip_address = ha_ip_address

	def serialize(self):
		ret = bytearray(0)
		ret += bytearray(self.ha_ip_address)
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		ha_ip_address = byte_array[0:4]
		msg = cls(ha_ip_address)
		return msg

class HAIpAddressMessage: 
	def __init__(self, msg_id, ha_ip_address):
		self.header = Header(SYNC_BYTES, msg_id, 4)
		self.body = HAIpAddressBody(ha_ip_address)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = HAIpAddressBody.deserialize(byte_array[8:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.ha_ip_address)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAVelocitySwitchOnSpeedMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAVelocitySwitchOffSpeedMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAPositionSwitchOnPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAPositionSwitchOffPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAVelocitySliderMinMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAVelocitySliderMaxMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAPositionSliderMinMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAPositionSliderMaxMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = DoubleMessage.deserialize(byte_array[8:16])
		footer = Footer.deserialize(byte_array[16:18])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAMqttUserMessage: 
	def __init__(self, msg_id, value):
		self.body = StringMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 32)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = StringMessage.deserialize(byte_array[8:40])
		footer = Footer.deserialize(byte_array[40:42])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAMqttPasswordMessage: 
	def __init__(self, msg_id, value):
		self.body = StringMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 32)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = StringMessage.deserialize(byte_array[8:40])
		footer = Footer.deserialize(byte_array[40:42])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAMqttNameMessage: 
	def __init__(self, msg_id, value):
		self.body = StringMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 32)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = StringMessage.deserialize(byte_array[8:40])
		footer = Footer.deserialize(byte_array[40:42])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HAMqttIconMessage: 
	def __init__(self, msg_id, value):
		self.body = StringMessage(value)
		self.header = Header(SYNC_BYTES, msg_id, 32)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:8])
		body = StringMessage.deserialize(byte_array[8:40])
		footer = Footer.deserialize(byte_array[40:42])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

def GetVersion(axis:'Axis', timeout = 0.1):
	send_msg = VersionMessage(GetVersionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetVersionId):
		msg = VersionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetI2CAddress(axis:'Axis', value):
	send_msg = I2CAddressMessage(SetI2CAddressId, value)
	axis.send_message(send_msg.serialize())

def GetI2CAddress(axis:'Axis', timeout = 0.1):
	send_msg = I2CAddressMessage(GetI2CAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetI2CAddressId):
		msg = I2CAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetEthernetAddress(axis:'Axis', value):
	send_msg = EthernetAddressMessage(SetEthernetAddressId, value)
	axis.send_message(send_msg.serialize())

def GetEthernetAddress(axis:'Axis', timeout = 0.1):
	send_msg = EthernetAddressMessage(GetEthernetAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetEthernetAddressId):
		msg = EthernetAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetEthernetPort(axis:'Axis', value):
	send_msg = EthernetPortMessage(SetEthernetPortId, value)
	axis.send_message(send_msg.serialize())

def GetEthernetPort(axis:'Axis', timeout = 0.1):
	send_msg = EthernetPortMessage(GetEthernetPortId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetEthernetPortId):
		msg = EthernetPortMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def GetMacAddress(axis:'Axis', timeout = 0.1):
	send_msg = MacAddressMessage(GetMacAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetMacAddressId):
		msg = MacAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SaveConfiguration(axis:'Axis', value):
	send_msg = SaveConfigurationMessage(SaveConfigurationId, value)
	axis.send_message(send_msg.serialize())

def SetLedState(axis:'Axis', value):
	send_msg = LedStateMessage(SetLedStateId, value)
	axis.send_message(send_msg.serialize())

def GetLedState(axis:'Axis', timeout = 0.1):
	send_msg = LedStateMessage(GetLedStateId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetLedStateId):
		msg = LedStateMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetLedColor(axis:'Axis', ledColor):
	send_msg = LedColorMessage(SetLedColorId, ledColor)
	axis.send_message(send_msg.serialize())

def GetLedColor(axis:'Axis', timeout = 0.1):
	send_msg = LedColorMessage(GetLedColorId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetLedColorId):
		msg = LedColorMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHomeDirection(axis:'Axis', value):
	send_msg = HomeDirectionMessage(SetHomeDirectionId, value)
	axis.send_message(send_msg.serialize())

def GetHomeDirection(axis:'Axis', timeout = 0.1):
	send_msg = HomeDirectionMessage(GetHomeDirectionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHomeDirectionId):
		msg = HomeDirectionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHomeThreshold(axis:'Axis', value):
	send_msg = HomeThresholdMessage(SetHomeThresholdId, value)
	axis.send_message(send_msg.serialize())

def GetHomeThreshold(axis:'Axis', timeout = 0.1):
	send_msg = HomeThresholdMessage(GetHomeThresholdId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHomeThresholdId):
		msg = HomeThresholdMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHomeSpeed(axis:'Axis', value):
	send_msg = HomeSpeedMessage(SetHomeSpeedId, value)
	axis.send_message(send_msg.serialize())

def GetHomeSpeed(axis:'Axis', timeout = 0.1):
	send_msg = HomeSpeedMessage(GetHomeSpeedId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHomeSpeedId):
		msg = HomeSpeedMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def GetHomedState(axis:'Axis', timeout = 0.1):
	send_msg = HomedStateMessage(GetHomedStateId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHomedStateId):
		msg = HomedStateMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def Home(axis:'Axis', value):
	send_msg = HomeMessage(HomeId, value)
	axis.send_message(send_msg.serialize())

def SetMotorState(axis:'Axis', value):
	send_msg = MotorStateMessage(SetMotorStateId, value)
	axis.send_message(send_msg.serialize())

def GetMotorState(axis:'Axis', timeout = 0.1):
	send_msg = MotorStateMessage(GetMotorStateId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetMotorStateId):
		msg = MotorStateMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetMotorBrake(axis:'Axis', value):
	send_msg = MotorBrakeMessage(SetMotorBrakeId, value)
	axis.send_message(send_msg.serialize())

def GetMotorBrake(axis:'Axis', timeout = 0.1):
	send_msg = MotorBrakeMessage(GetMotorBrakeId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetMotorBrakeId):
		msg = MotorBrakeMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetMaxSpeed(axis:'Axis', value):
	send_msg = MaxSpeedMessage(SetMaxSpeedId, value)
	axis.send_message(send_msg.serialize())

def GetMaxSpeed(axis:'Axis', timeout = 0.1):
	send_msg = MaxSpeedMessage(GetMaxSpeedId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetMaxSpeedId):
		msg = MaxSpeedMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetAcceleration(axis:'Axis', value):
	send_msg = AccelerationMessage(SetAccelerationId, value)
	axis.send_message(send_msg.serialize())

def GetAcceleration(axis:'Axis', timeout = 0.1):
	send_msg = AccelerationMessage(GetAccelerationId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetAccelerationId):
		msg = AccelerationMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetCurrentPosition(axis:'Axis', value):
	send_msg = CurrentPositionMessage(SetCurrentPositionId, value)
	axis.send_message(send_msg.serialize())

def GetCurrentPosition(axis:'Axis', timeout = 0.1):
	send_msg = CurrentPositionMessage(GetCurrentPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetCurrentPositionId):
		msg = CurrentPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetTargetPosition(axis:'Axis', value):
	send_msg = TargetPositionMessage(SetTargetPositionId, value)
	axis.send_message(send_msg.serialize())

def GetTargetPosition(axis:'Axis', timeout = 0.1):
	send_msg = TargetPositionMessage(GetTargetPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetTargetPositionId):
		msg = TargetPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetRelativeTargetPosition(axis:'Axis', value):
	send_msg = RelativeTargetPositionMessage(SetRelativeTargetPositionId, value)
	axis.send_message(send_msg.serialize())

def SetVelocity(axis:'Axis', value):
	send_msg = VelocityMessage(SetVelocityId, value)
	axis.send_message(send_msg.serialize())

def GetVelocity(axis:'Axis', timeout = 0.1):
	send_msg = VelocityMessage(GetVelocityId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetVelocityId):
		msg = VelocityMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetVelocityAndSteps(axis:'Axis', velocity, steps, positionMode):
	send_msg = VelocityAndStepsMessage(SetVelocityAndStepsId, velocity, steps, positionMode)
	axis.send_message(send_msg.serialize())

def StartPath(axis:'Axis', value):
	send_msg = StartPathMessage(StartPathId, value)
	axis.send_message(send_msg.serialize())

def SetHAEnable(axis:'Axis', value):
	send_msg = HAEnableMessage(SetHAEnableId, value)
	axis.send_message(send_msg.serialize())

def GetHAEnable(axis:'Axis', timeout = 0.1):
	send_msg = HAEnableMessage(GetHAEnableId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAEnableId):
		msg = HAEnableMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAMode(axis:'Axis', value):
	send_msg = HAModeMessage(SetHAModeId, value)
	axis.send_message(send_msg.serialize())

def GetHAMode(axis:'Axis', timeout = 0.1):
	send_msg = HAModeMessage(GetHAModeId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAModeId):
		msg = HAModeMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAIpAddress(axis:'Axis', ha_ip_address):
	send_msg = HAIpAddressMessage(SetHAIpAddressId, ha_ip_address)
	axis.send_message(send_msg.serialize())

def GetHAIpAddress(axis:'Axis', timeout = 0.1):
	send_msg = HAIpAddressMessage(GetHAIpAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAIpAddressId):
		msg = HAIpAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAVelocitySwitchOnSpeed(axis:'Axis', value):
	send_msg = HAVelocitySwitchOnSpeedMessage(SetHAVelocitySwitchOnSpeedId, value)
	axis.send_message(send_msg.serialize())

def GetHAVelocitySwitchOnSpeed(axis:'Axis', timeout = 0.1):
	send_msg = HAVelocitySwitchOnSpeedMessage(GetHAVelocitySwitchOnSpeedId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAVelocitySwitchOnSpeedId):
		msg = HAVelocitySwitchOnSpeedMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAVelocitySwitchOffSpeed(axis:'Axis', value):
	send_msg = HAVelocitySwitchOffSpeedMessage(SetHAVelocitySwitchOffSpeedId, value)
	axis.send_message(send_msg.serialize())

def GetHAVelocitySwitchOffSpeed(axis:'Axis', timeout = 0.1):
	send_msg = HAVelocitySwitchOffSpeedMessage(GetHAVelocitySwitchOffSpeedId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAVelocitySwitchOffSpeedId):
		msg = HAVelocitySwitchOffSpeedMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAPositionSwitchOnPosition(axis:'Axis', value):
	send_msg = HAPositionSwitchOnPositionMessage(SetHAPositionSwitchOnPositionId, value)
	axis.send_message(send_msg.serialize())

def GetHAPositionSwitchOnPosition(axis:'Axis', timeout = 0.1):
	send_msg = HAPositionSwitchOnPositionMessage(GetHAPositionSwitchOnPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAPositionSwitchOnPositionId):
		msg = HAPositionSwitchOnPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAPositionSwitchOffPosition(axis:'Axis', value):
	send_msg = HAPositionSwitchOffPositionMessage(SetHAPositionSwitchOffPositionId, value)
	axis.send_message(send_msg.serialize())

def GetHAPositionSwitchOffPosition(axis:'Axis', timeout = 0.1):
	send_msg = HAPositionSwitchOffPositionMessage(GetHAPositionSwitchOffPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAPositionSwitchOffPositionId):
		msg = HAPositionSwitchOffPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAVelocitySliderMin(axis:'Axis', value):
	send_msg = HAVelocitySliderMinMessage(SetHAVelocitySliderMinId, value)
	axis.send_message(send_msg.serialize())

def GetHAVelocitySliderMin(axis:'Axis', timeout = 0.1):
	send_msg = HAVelocitySliderMinMessage(GetHAVelocitySliderMinId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAVelocitySliderMinId):
		msg = HAVelocitySliderMinMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAVelocitySliderMax(axis:'Axis', value):
	send_msg = HAVelocitySliderMaxMessage(SetHAVelocitySliderMaxId, value)
	axis.send_message(send_msg.serialize())

def GetHAVelocitySliderMax(axis:'Axis', timeout = 0.1):
	send_msg = HAVelocitySliderMaxMessage(GetHAVelocitySliderMaxId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAVelocitySliderMaxId):
		msg = HAVelocitySliderMaxMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAPositionSliderMin(axis:'Axis', value):
	send_msg = HAPositionSliderMinMessage(SetHAPositionSliderMinId, value)
	axis.send_message(send_msg.serialize())

def GetHAPositionSliderMin(axis:'Axis', timeout = 0.1):
	send_msg = HAPositionSliderMinMessage(GetHAPositionSliderMinId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAPositionSliderMinId):
		msg = HAPositionSliderMinMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAPositionSliderMax(axis:'Axis', value):
	send_msg = HAPositionSliderMaxMessage(SetHAPositionSliderMaxId, value)
	axis.send_message(send_msg.serialize())

def GetHAPositionSliderMax(axis:'Axis', timeout = 0.1):
	send_msg = HAPositionSliderMaxMessage(GetHAPositionSliderMaxId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAPositionSliderMaxId):
		msg = HAPositionSliderMaxMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAMqttUser(axis:'Axis', value):
	send_msg = HAMqttUserMessage(SetHAMqttUserId, value)
	axis.send_message(send_msg.serialize())

def GetHAMqttUser(axis:'Axis', timeout = 0.1):
	send_msg = HAMqttUserMessage(GetHAMqttUserId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAMqttUserId):
		msg = HAMqttUserMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAMqttPassword(axis:'Axis', value):
	send_msg = HAMqttPasswordMessage(SetHAMqttPasswordId, value)
	axis.send_message(send_msg.serialize())

def GetHAMqttPassword(axis:'Axis', timeout = 0.1):
	send_msg = HAMqttPasswordMessage(GetHAMqttPasswordId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAMqttPasswordId):
		msg = HAMqttPasswordMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAMqttName(axis:'Axis', value):
	send_msg = HAMqttNameMessage(SetHAMqttNameId, value)
	axis.send_message(send_msg.serialize())

def GetHAMqttName(axis:'Axis', timeout = 0.1):
	send_msg = HAMqttNameMessage(GetHAMqttNameId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAMqttNameId):
		msg = HAMqttNameMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHAMqttIcon(axis:'Axis', value):
	send_msg = HAMqttIconMessage(SetHAMqttIconId, value)
	axis.send_message(send_msg.serialize())

def GetHAMqttIcon(axis:'Axis', timeout = 0.1):
	send_msg = HAMqttIconMessage(GetHAMqttIconId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[4]==GetHAMqttIconId):
		msg = HAMqttIconMessage.deserialize(ret)
		return msg.body
	else:
		 return None

