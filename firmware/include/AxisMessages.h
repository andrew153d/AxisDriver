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
	virtual float GetUpdateRate();
};
enum class MessageTypes : uint16_t
{
	AckId = 0x0100,
	GetVersionId = 0x0101,
	SetI2CAddressId = 0x0102,
	GetI2CAddressId = 0x0103,
	SetEthernetAddressId = 0x0104,
	GetEthernetAddressId = 0x0105,
	SetEthernetPortId = 0x0106,
	GetEthernetPortId = 0x0107,
	GetMacAddressId = 0x0108,
	SaveConfigurationId = 0x0109,
	SetLedStateId = 0x010A,
	GetLedStateId = 0x010B,
	SetLedColorId = 0x010C,
	GetLedColorId = 0x010D,
	SetHomeDirectionId = 0x010E,
	GetHomeDirectionId = 0x010F,
	SetHomeThresholdId = 0x0110,
	GetHomeThresholdId = 0x0111,
	SetHomeSpeedId = 0x0112,
	GetHomeSpeedId = 0x0113,
	GetHomedStateId = 0x0114,
	HomeId = 0x0115,
	SetMotorStateId = 0x0116,
	GetMotorStateId = 0x0117,
	SetMotorBrakeId = 0x0118,
	GetMotorBrakeId = 0x0119,
	SetMaxSpeedId = 0x011A,
	GetMaxSpeedId = 0x011B,
	SetAccelerationId = 0x011C,
	GetAccelerationId = 0x011D,
	SetCurrentPositionId = 0x011E,
	GetCurrentPositionId = 0x011F,
	SetTargetPositionId = 0x0120,
	GetTargetPositionId = 0x0121,
	SetRelativeTargetPositionId = 0x0122,
	SetVelocityId = 0x0123,
	GetVelocityId = 0x0124,
	SetVelocityAndStepsId = 0x0125,
	StartPathId = 0x0126,
};

enum class StatusCodes : uint8_t {
	SUCCESS = 0x0,
	ERROR = 0x1,
	INVALID_COMMAND = 0x2,
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

enum class PositionMode{
	ABSOLUTE = 0x0,
	RELATIVE = 0x1,
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
PACKEDSTRUCT AckMessage
{
	Header header;
	uint16_t ack_message_type;
	uint8_t status;
	Footer footer;
};
typedef U8Message I2CAddressMessage;
typedef U32Message EthernetAddressMessage;
typedef U32Message EthernetPortMessage;
PACKEDSTRUCT MacAddressMessage
{
	Header header;
	uint8_t mac[6];
	Footer footer;
};
typedef U8Message SaveConfigurationMessage;
typedef U8Message LedStateMessage;
PACKEDSTRUCT LedColorMessage
{
	Header header;
	uint8_t ledColor[3];
	Footer footer;
};
typedef U8Message HomeDirectionMessage;
typedef U32Message HomeSpeedMessage;
typedef U32Message HomeThresholdMessage;
typedef U8Message HomedStateMessage;
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
	uint8_t positionMode;
	Footer footer;
};
typedef U8Message StartPathMessage;
