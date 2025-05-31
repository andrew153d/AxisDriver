#pragma once
#include <Arduino.h>
#define FIRMWARE_VERSION "1.0.0.0"

#define PACKEDSTRUCT struct __attribute__((packed))
#define SYNC_BYTES 0xDEADBABE
#define HEADER_SIZE 8
#define FOOTER_SIZE 2
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
	GetVersionId = 0x0,
	SetI2CAddressId = 0x1,
	GetI2CAddressId = 0x2,
	SetEthernetAddressId = 0x3,
	GetEthernetAddressId = 0x4,
	SetEthernetPortId = 0x5,
	GetEthernetPortId = 0x6,
	GetMacAddressId = 0x7,
	SaveConfigurationId = 0x8,
	SetLedStateId = 0x9,
	GetLedStateId = 0xA,
	SetLedColorId = 0xB,
	GetLedColorId = 0xC,
	SetHomeDirectionId = 0xD,
	GetHomeDirectionId = 0xE,
	SetHomeThresholdId = 0xF,
	GetHomeThresholdId = 0x10,
	SetHomeSpeedId = 0x11,
	GetHomeSpeedId = 0x12,
	GetHomedStateId = 0x13,
	HomeId = 0x14,
	SetMotorStateId = 0x15,
	GetMotorStateId = 0x16,
	SetMotorBrakeId = 0x17,
	GetMotorBrakeId = 0x18,
	SetMaxSpeedId = 0x19,
	GetMaxSpeedId = 0x1A,
	SetAccelerationId = 0x1B,
	GetAccelerationId = 0x1C,
	SetCurrentPositionId = 0x1D,
	GetCurrentPositionId = 0x1E,
	SetTargetPositionId = 0x1F,
	GetTargetPositionId = 0x20,
	SetRelativeTargetPositionId = 0x21,
	SetVelocityId = 0x22,
	GetVelocityId = 0x23,
	SetVelocityAndStepsId = 0x24,
	StartPathId = 0x25,
	SetHAEnableId = 0x26,
	GetHAEnableId = 0x27,
	SetHAModeId = 0x28,
	GetHAModeId = 0x29,
	SetHAIpAddressId = 0x2A,
	GetHAIpAddressId = 0x2B,
	SetHAVelocitySwitchOnSpeedId = 0x2C,
	GetHAVelocitySwitchOnSpeedId = 0x2D,
	SetHAVelocitySwitchOffSpeedId = 0x2E,
	GetHAVelocitySwitchOffSpeedId = 0x2F,
	SetHAPositionSwitchOnPositionId = 0x30,
	GetHAPositionSwitchOnPositionId = 0x31,
	SetHAPositionSwitchOffPositionId = 0x32,
	GetHAPositionSwitchOffPositionId = 0x33,
	SetHAVelocitySliderMinId = 0x34,
	GetHAVelocitySliderMinId = 0x35,
	SetHAVelocitySliderMaxId = 0x36,
	GetHAVelocitySliderMaxId = 0x37,
	SetHAPositionSliderMinId = 0x38,
	GetHAPositionSliderMinId = 0x39,
	SetHAPositionSliderMaxId = 0x3A,
	GetHAPositionSliderMaxId = 0x3B,
	SetHAMqttUserId = 0x3C,
	GetHAMqttUserId = 0x3D,
	SetHAMqttPasswordId = 0x3E,
	GetHAMqttPasswordId = 0x3F,
	SetHAMqttNameId = 0x40,
	GetHAMqttNameId = 0x41,
	SetHAMqttIconId = 0x42,
	GetHAMqttIconId = 0x43,
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
	uint32_t sync_bytes;
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
PACKEDSTRUCT StringMessage
{
	Header header;
	uint8_t value[32];
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
typedef S8Message HomeDirectionMessage;
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
typedef U8Message HAEnableMessage;
typedef U8Message HAModeMessage;
PACKEDSTRUCT HAIpAddressMessage
{
	Header header;
	uint8_t ha_ip_address[4];
	Footer footer;
};
typedef DoubleMessage HAVelocitySwitchOnSpeedMessage;
typedef DoubleMessage HAVelocitySwitchOffSpeedMessage;
typedef DoubleMessage HAPositionSwitchOnPositionMessage;
typedef DoubleMessage HAPositionSwitchOffPositionMessage;
typedef DoubleMessage HAVelocitySliderMinMessage;
typedef DoubleMessage HAVelocitySliderMaxMessage;
typedef DoubleMessage HAPositionSliderMinMessage;
typedef DoubleMessage HAPositionSliderMaxMessage;
typedef StringMessage HAMqttUserMessage;
typedef StringMessage HAMqttPasswordMessage;
typedef StringMessage HAMqttNameMessage;
typedef StringMessage HAMqttIconMessage;
