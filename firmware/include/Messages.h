#pragma once
#include <Arduino.h>
#define FIRMWARE_VERSION "1.0.0.0"

#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t *recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t *send_bytes, uint32_t send_bytes_size);

class IEncoderInterface
{
public:
    virtual float GetVelocityDegreesPerSecond();
    virtual float GetPositionDegrees();
};
enum class MessageTypes : uint16_t
{
	GetVersionId = 0x0,
	SetI2CAddressId = 0x1,
	GetI2CAddressId = 0x2,
	SetEthernetAddressId = 0x3,
	GetEthernetAddressId = 0x4,
	SetEthernetPortId = 0x5,
	GetEthernetPortId = 0x6,
	GetMacAddressId = 0x7,
	SaveSettingsId = 0x8,
	SetLedStateId = 0x9,
	GetLedStateId = 0xA,
	SetLedColorId = 0xB,
	GetLedColorId = 0xC,
	SetHomeDirectionId = 0xD,
	GetHomeDirectionId = 0xE,
	HomeId = 0xF,
	SetMotorStateId = 0x10,
	GetMotorStateId = 0x11,
	SetMotorBrakeId = 0x12,
	GetMotorBrakeId = 0x13,
	SetMaxSpeedId = 0x14,
	GetMaxSpeedId = 0x15,
	SetAccelerationId = 0x16,
	GetAccelerationId = 0x17,
	SetCurrentPositionId = 0x18,
	GetCurrentPositionId = 0x19,
	SetTargetPositionId = 0x1A,
	GetTargetPositionId = 0x1B,
	SetRelativeTargetPositionId = 0x1C,
	SetVelocityId = 0x1D,
	GetVelocityId = 0x1E,
	SetVelocityAndStepsId = 0x1F,
	StartPathId = 0x20,
};

enum class LedStates{
	OFF = 0x0,
	FLASH_ERROR = 0x1,
	ERROR = 0x2,
	BOOTUP = 0x3,
	RAINBOW = 0x4,
	SOLID = 0x5,
	MAX_VALUE = 0x6,
};

enum class MotorStates{
	OFF = 0x0,
	POSITION = 0x1,
	VELOCITY = 0x2,
	VELOCITY_STEP = 0x3,
	IDLE_ON = 0x4,
	HOME = 0x5,
};

enum class MotorBrake{
	NORMAL = 0x0,
	FREEWHEELING = 0x1,
	STRONG_BRAKING = 0x2,
	BRAKING = 0x3,
};

enum class HomeDirection{
	CLOCKWISE = 0x0,
	COUNTERCLOCKWISE = 0x1,
};

PACKEDSTRUCT Header
{
	uint16_t message_type;
	uint16_t body_size;
};
PACKEDSTRUCT Footer
{
	uint16_t checksum;
};
PACKEDSTRUCT U8Message
{
	Header header;
	uint8_t value;
	Footer footer;
};
PACKEDSTRUCT S8Message
{
	Header header;
	int8_t value;
	Footer footer;
};
PACKEDSTRUCT U32Message
{
	Header header;
	uint32_t value;
	Footer footer;
};
PACKEDSTRUCT S32Message
{
	Header header;
	int32_t value;
	Footer footer;
};
PACKEDSTRUCT DoubleMessage
{
	Header header;
	double value;
	Footer footer;
};
typedef U32Message VersionMessage;
typedef U8Message I2CAddressMessage;
typedef U32Message EthernetAddressMessage;
typedef U32Message EthernetPortMessage;
PACKEDSTRUCT MacAddressMessage
{
	Header header;
	uint8_t mac[6];
	uint32_t mac2;
	Footer footer;
};
typedef U8Message SaveSettingsMessage;
typedef U8Message LedStateMessage;
PACKEDSTRUCT LedColorMessage
{
	Header header;
	uint8_t ledColor[3];
	Footer footer;
};
typedef U8Message HomeDirectionMessage;
typedef U32Message HomeMessage;
typedef U8Message MotorStateMessage;
typedef U8Message MotorBrakeMessage;
typedef U32Message MaxSpeedMessage;
typedef U32Message AccelerationMessage;
typedef DoubleMessage CurrentPositionMessage;
typedef DoubleMessage TargetPositionMessage;
typedef DoubleMessage RelativeTargetPositionMessage;
typedef DoubleMessage VelocityMessage;
PACKEDSTRUCT VelocityAndStepsMessage
{
	Header header;
	int32_t velocity;
	int32_t steps;
	Footer footer;
};
typedef U8Message StartPathMessage;
