
from Axis import *
import struct

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
HomeId = 0xF
SetMotorStateId = 0x10
GetMotorStateId = 0x11
SetMotorBrakeId = 0x12
GetMotorBrakeId = 0x13
SetMaxSpeedId = 0x14
GetMaxSpeedId = 0x15
SetAccelerationId = 0x16
GetAccelerationId = 0x17
SetCurrentPositionId = 0x18
GetCurrentPositionId = 0x19
SetTargetPositionId = 0x1A
GetTargetPositionId = 0x1B
SetRelativeTargetPositionId = 0x1C
SetVelocityId = 0x1D
GetVelocityId = 0x1E
SetVelocityAndStepsId = 0x1F
StartPathId = 0x20


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
	def __init__(self, message_type, body_size):
		self.message_type = message_type
		self.body_size = body_size

	def serialize(self):
		ret = bytearray(0)
		ret += self.message_type.to_bytes(2, 'little')
		ret += self.body_size.to_bytes(2, 'little')
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		message_type = int.from_bytes(byte_array[0:2], 'little')
		body_size = int.from_bytes(byte_array[2:4], 'little')
		return cls(message_type, body_size)

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

class VersionMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class I2CAddressMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class EthernetAddressMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class EthernetPortMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
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
		mac = bytearray(byte_array[0:6])
		msg = cls(mac)
		return msg

class MacAddressMessage: 
	def __init__(self, msg_id, mac):
		self.header = Header(msg_id, 6)
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
		header = Header.deserialize(byte_array[0:4])
		body = MacAddressBody.deserialize(byte_array[4:10])
		footer = Footer.deserialize(byte_array[10:12])
		msg = cls(header.message_type, body.mac)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class SaveConfigurationMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class LedStateMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
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
		ledColor = bytearray(byte_array[0:3])
		msg = cls(ledColor)
		return msg

class LedColorMessage: 
	def __init__(self, msg_id, ledColor):
		self.header = Header(msg_id, 3)
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
		header = Header.deserialize(byte_array[0:4])
		body = LedColorBody.deserialize(byte_array[4:7])
		footer = Footer.deserialize(byte_array[7:9])
		msg = cls(header.message_type, body.ledColor)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeDirectionMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class HomeMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MotorStateMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MotorBrakeMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class MaxSpeedMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class AccelerationMessage: 
	def __init__(self, msg_id, value):
		self.body = U32Message(value)
		self.header = Header(msg_id, 4)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U32Message.deserialize(byte_array[4:8])
		footer = Footer.deserialize(byte_array[8:10])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class CurrentPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = DoubleMessage.deserialize(byte_array[4:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class TargetPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = DoubleMessage.deserialize(byte_array[4:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class RelativeTargetPositionMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = DoubleMessage.deserialize(byte_array[4:12])
		footer = Footer.deserialize(byte_array[12:14])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class VelocityMessage: 
	def __init__(self, msg_id, value):
		self.body = DoubleMessage(value)
		self.header = Header(msg_id, 8)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = DoubleMessage.deserialize(byte_array[4:12])
		footer = Footer.deserialize(byte_array[12:14])
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
		self.header = Header(msg_id, 9)
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
		header = Header.deserialize(byte_array[0:4])
		body = VelocityAndStepsBody.deserialize(byte_array[4:13])
		footer = Footer.deserialize(byte_array[13:15])
		msg = cls(header.message_type, body.velocity, body.steps, body.positionMode)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

class StartPathMessage: 
	def __init__(self, msg_id, value):
		self.body = U8Message(value)
		self.header = Header(msg_id, 1)
		self.footer = Footer(sum(self.body.serialize())%0xFFFF)

	def serialize(self):
		ret = bytearray(0)
		ret += self.header.serialize()
		ret += self.body.serialize()
		ret += self.footer.serialize()
		return ret

	@classmethod
	def deserialize(cls, byte_array):
		header = Header.deserialize(byte_array[0:4])
		body = U8Message.deserialize(byte_array[4:5])
		footer = Footer.deserialize(byte_array[5:7])
		msg = cls(header.message_type, body.value)
		msg.body = body
		msg.header = header
		msg.footer = footer
		return msg

def GetVersion(axis:Axis, timeout = 0.1):
	send_msg = VersionMessage(GetVersionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetVersionId):
		msg = VersionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetI2CAddress(axis:Axis, value):
	send_msg = I2CAddressMessage(SetI2CAddressId, value)
	axis.send_message(send_msg.serialize())

def GetI2CAddress(axis:Axis, timeout = 0.1):
	send_msg = I2CAddressMessage(GetI2CAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetI2CAddressId):
		msg = I2CAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetEthernetAddress(axis:Axis, value):
	send_msg = EthernetAddressMessage(SetEthernetAddressId, value)
	axis.send_message(send_msg.serialize())

def GetEthernetAddress(axis:Axis, timeout = 0.1):
	send_msg = EthernetAddressMessage(GetEthernetAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetEthernetAddressId):
		msg = EthernetAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetEthernetPort(axis:Axis, value):
	send_msg = EthernetPortMessage(SetEthernetPortId, value)
	axis.send_message(send_msg.serialize())

def GetEthernetPort(axis:Axis, timeout = 0.1):
	send_msg = EthernetPortMessage(GetEthernetPortId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetEthernetPortId):
		msg = EthernetPortMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def GetMacAddress(axis:Axis, timeout = 0.1):
	send_msg = MacAddressMessage(GetMacAddressId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetMacAddressId):
		msg = MacAddressMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SaveConfiguration(axis:Axis, value):
	send_msg = SaveConfigurationMessage(SaveConfigurationId, value)
	axis.send_message(send_msg.serialize())

def SetLedState(axis:Axis, value):
	send_msg = LedStateMessage(SetLedStateId, value)
	axis.send_message(send_msg.serialize())

def GetLedState(axis:Axis, timeout = 0.1):
	send_msg = LedStateMessage(GetLedStateId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetLedStateId):
		msg = LedStateMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetLedColor(axis:Axis, ledColor):
	send_msg = LedColorMessage(SetLedColorId, ledColor)
	axis.send_message(send_msg.serialize())

def GetLedColor(axis:Axis, timeout = 0.1):
	send_msg = LedColorMessage(GetLedColorId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetLedColorId):
		msg = LedColorMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetHomeDirection(axis:Axis, value):
	send_msg = HomeDirectionMessage(SetHomeDirectionId, value)
	axis.send_message(send_msg.serialize())

def GetHomeDirection(axis:Axis, timeout = 0.1):
	send_msg = HomeDirectionMessage(GetHomeDirectionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetHomeDirectionId):
		msg = HomeDirectionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def Home(axis:Axis, value):
	send_msg = HomeMessage(HomeId, value)
	axis.send_message(send_msg.serialize())

def SetMotorState(axis:Axis, value):
	send_msg = MotorStateMessage(SetMotorStateId, value)
	axis.send_message(send_msg.serialize())

def GetMotorState(axis:Axis, timeout = 0.1):
	send_msg = MotorStateMessage(GetMotorStateId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetMotorStateId):
		msg = MotorStateMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetMotorBrake(axis:Axis, value):
	send_msg = MotorBrakeMessage(SetMotorBrakeId, value)
	axis.send_message(send_msg.serialize())

def GetMotorBrake(axis:Axis, timeout = 0.1):
	send_msg = MotorBrakeMessage(GetMotorBrakeId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetMotorBrakeId):
		msg = MotorBrakeMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetMaxSpeed(axis:Axis, value):
	send_msg = MaxSpeedMessage(SetMaxSpeedId, value)
	axis.send_message(send_msg.serialize())

def GetMaxSpeed(axis:Axis, timeout = 0.1):
	send_msg = MaxSpeedMessage(GetMaxSpeedId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetMaxSpeedId):
		msg = MaxSpeedMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetAcceleration(axis:Axis, value):
	send_msg = AccelerationMessage(SetAccelerationId, value)
	axis.send_message(send_msg.serialize())

def GetAcceleration(axis:Axis, timeout = 0.1):
	send_msg = AccelerationMessage(GetAccelerationId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetAccelerationId):
		msg = AccelerationMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetCurrentPosition(axis:Axis, value):
	send_msg = CurrentPositionMessage(SetCurrentPositionId, value)
	axis.send_message(send_msg.serialize())

def GetCurrentPosition(axis:Axis, timeout = 0.1):
	send_msg = CurrentPositionMessage(GetCurrentPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetCurrentPositionId):
		msg = CurrentPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetTargetPosition(axis:Axis, value):
	send_msg = TargetPositionMessage(SetTargetPositionId, value)
	axis.send_message(send_msg.serialize())

def GetTargetPosition(axis:Axis, timeout = 0.1):
	send_msg = TargetPositionMessage(GetTargetPositionId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetTargetPositionId):
		msg = TargetPositionMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetRelativeTargetPosition(axis:Axis, value):
	send_msg = RelativeTargetPositionMessage(SetRelativeTargetPositionId, value)
	axis.send_message(send_msg.serialize())

def SetVelocity(axis:Axis, value):
	send_msg = VelocityMessage(SetVelocityId, value)
	axis.send_message(send_msg.serialize())

def GetVelocity(axis:Axis, timeout = 0.1):
	send_msg = VelocityMessage(GetVelocityId, 0)
	axis.send_message(send_msg.serialize())
	ret = axis.wait_message(timeout)
	if(ret[0]==GetVelocityId):
		msg = VelocityMessage.deserialize(ret)
		return msg.body
	else:
		 return None

def SetVelocityAndSteps(axis:Axis, velocity, steps, positionMode):
	send_msg = VelocityAndStepsMessage(SetVelocityAndStepsId, velocity, steps, positionMode)
	axis.send_message(send_msg.serialize())

def StartPath(axis:Axis, value):
	send_msg = StartPathMessage(StartPathId, value)
	axis.send_message(send_msg.serialize())

