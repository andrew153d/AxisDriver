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
	SetLedColorId = 0x0200,
	GetLedColorId = 0x0201,
	AddLedStepId = 0x0202,
	SetHomeDirectionId = 0x0300,
	GetHomeDirectionId = 0x0301,
	SetHomeThresholdId = 0x0302,
	GetHomeThresholdId = 0x0303,
	SetHomeSpeedId = 0x0304,
	GetHomeSpeedId = 0x0305,
	GetHomedStateId = 0x0306,
	HomeId = 0x0400,
	SetMotorStateId = 0x0307,
	GetMotorStateId = 0x0308,
	SetMotorBrakeId = 0x0309,
	GetMotorBrakeId = 0x030A,
	SetMaxSpeedId = 0x030B,
	GetMaxSpeedId = 0x030C,
	SetAccelerationId = 0x030D,
	GetAccelerationId = 0x030E,
	SetCurrentPositionId = 0x030F,
	GetCurrentPositionId = 0x0310,
	SetTargetPositionId = 0x0311,
	GetTargetPositionId = 0x0312,
	SetRelativeTargetPositionId = 0x0401,
	SetVelocityId = 0x0313,
	GetVelocityId = 0x0314,
	SetVelocityAndStepsId = 0x0402,
	StartPathId = 0x0403,
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
// AddLedStep message per spec (time in ms + RGB)
PACKEDSTRUCT AddLedStepMessage
{
	Header header;
	uint32_t time_ms;
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


// Message length definitions (in bytes)
const size_t ACK_MESSAGE_LENGTH = sizeof(AckMessage);
const size_t VERSION_MESSAGE_LENGTH = sizeof(VersionMessage);
const size_t I2C_ADDRESS_MESSAGE_LENGTH = sizeof(I2CAddressMessage);
const size_t ETHERNET_ADDRESS_MESSAGE_LENGTH = sizeof(EthernetAddressMessage);
const size_t ETHERNET_PORT_MESSAGE_LENGTH = sizeof(EthernetPortMessage);
const size_t MAC_ADDRESS_MESSAGE_LENGTH = sizeof(MacAddressMessage);
const size_t SAVE_CONFIGURATION_MESSAGE_LENGTH = sizeof(SaveConfigurationMessage);
const size_t LED_COLOR_MESSAGE_LENGTH = sizeof(LedColorMessage);
const size_t ADD_LED_STEP_MESSAGE_LENGTH = sizeof(AddLedStepMessage);
const size_t HOME_DIRECTION_MESSAGE_LENGTH = sizeof(HomeDirectionMessage);
const size_t HOME_THRESHOLD_MESSAGE_LENGTH = sizeof(HomeThresholdMessage);
const size_t HOME_SPEED_MESSAGE_LENGTH = sizeof(HomeSpeedMessage);
const size_t HOMED_STATE_MESSAGE_LENGTH = sizeof(HomedStateMessage);
const size_t HOME_MESSAGE_LENGTH = sizeof(HomeMessage);
const size_t MOTOR_STATE_MESSAGE_LENGTH = sizeof(MotorStateMessage);
const size_t MOTOR_BRAKE_MESSAGE_LENGTH = sizeof(MotorBrakeMessage);
const size_t MAX_SPEED_MESSAGE_LENGTH = sizeof(MaxSpeedMessage);
const size_t ACCELERATION_MESSAGE_LENGTH = sizeof(AccelerationMessage);
const size_t CURRENT_POSITION_MESSAGE_LENGTH = sizeof(CurrentPositionMessage);
const size_t TARGET_POSITION_MESSAGE_LENGTH = sizeof(TargetPositionMessage);
const size_t RELATIVE_TARGET_POSITION_MESSAGE_LENGTH = sizeof(RelativeTargetPositionMessage);
const size_t VELOCITY_MESSAGE_LENGTH = sizeof(VelocityMessage);
const size_t VELOCITY_AND_STEPS_MESSAGE_LENGTH = sizeof(VelocityAndStepsMessage);
const size_t START_PATH_MESSAGE_LENGTH = sizeof(StartPathMessage);
