#pragma once
#include <cstdint>
#include <Arduino.h>
#include <ArduinoJson.h>
#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t *recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t *send_bytes, uint32_t send_bytes_size);

class IEncoderInterface
{
public:
    virtual float GetVelocityDegreesPerSecond();
    virtual float GetPositionDegrees();
};

#define MOTORTYPESTRING "Motor"
#define ENCODERTYPESTRING "Encoder"
#define LEDTYPESTRING "Led"

//motor commands

//encoder commands
#define GETPOSITIONCOMMAND "GetPosition"
#define SETPOSITIONCOMMAND "SetPosition"
#define GETUPDATERATECOMMAND "GetUpdateRate"

enum class EncoderCommands : uint8_t
{
    GetPosition, 
    SetPosition,
    GetUpdateRate,
    Unknown
};

inline EncoderCommands ToEncoderCommandsEnum(String command_str)
{
    if(command_str == GETPOSITIONCOMMAND)
    return EncoderCommands::GetPosition;
    if(command_str == SETPOSITIONCOMMAND)
    return EncoderCommands::SetPosition;
    if(command_str == GETUPDATERATECOMMAND)
    return EncoderCommands::GetUpdateRate;
    return EncoderCommands::Unknown;
}

//led commands

enum class JsonMessageTypes
{
    Motor,
    Encoder,
    Led,
    Unknown
};

inline JsonMessageTypes ToJsonMessageTypes(String type)
{
    if (type == MOTORTYPESTRING)
        return JsonMessageTypes::Motor;
    if(type == ENCODERTYPESTRING)
        return JsonMessageTypes::Encoder;
    if(type == LEDTYPESTRING)
        return JsonMessageTypes::Led;

    return JsonMessageTypes::Unknown;
}

template<typename T>
inline bool TryParseJson(const char* key, T* out, JsonDocument& json_doc)
{
    if (!json_doc[key].is<T>()) {
        return false;
    }

    *out = json_doc[key].as<T>();
    return true;
}

enum class MessageTypes : uint16_t
{
    // General Device Info
    DeviceInfoMessageTypeLowerBounds = 0x0001,
    GetVersion,
    DeviceInfoMessageTypeUpperBounds = 0x00FF,

    // Device Settings
    SetI2CAddress = 0x0500,
    GetI2CAddress = 0x0501,

    // LED control
    LedControlMessageTypeLowerBounds = 0x3000,
    SetLedColor,
    GetLedColor,
    SetLedState,
    LedControlMessageTypeUpperBounds = 0x30FF,

    // Drive Configuration

    // Motor Driving
    MotorControlMessageTypeLowerBounds = 0x5000,
    GetMotorPosition,
    GetMotorVelocity,
    GetMotorCurrent,

    SetMotorPosition,
    SetMotorVelocity,
    SetMotorCurrent,

    SetControlMode,
    GetControlMode,

    GetMotorStatus,
    StartMotor,
    MotorControlMessageTypeUpperBounds = 0x5FFF,

    // Encoder Messages
    EncoderControlMessageTypeLowerBounds = 0x06000,
    EncoderControlMessageTypeUpperBounds = 0x6FFF,
};

PACKEDSTRUCT Header
{
    MessageTypes message_type;
    uint16_t body_size;
};

PACKEDSTRUCT Footer
{
    uint16_t checksum;
};

const uint32_t HEADER_SIZE = sizeof(Header);
const uint32_t FOOTER_SIZE = sizeof(Footer);

PACKEDSTRUCT GetVersionMessage
{
    Header header;
    Footer footer;
};

PACKEDSTRUCT VersionMessage
{
    Header header;
    char version[32] = {0};
    Footer footer;
};

PACKEDSTRUCT SetIpAddressMessage
{
    Header header;
    uint32_t ip_address;
    Footer footer;
};

PACKEDSTRUCT LedColors
{
    union
    {
        struct
        {
            union
            {
                uint8_t r;
                uint8_t red;
            };
            union
            {
                uint8_t g;
                uint8_t green;
            };
            union
            {
                uint8_t b;
                uint8_t blue;
            };
        };
        uint8_t raw[3];
    };
};

PACKEDSTRUCT SetLedColorMessage
{
    Header header;
    LedColors ledColor;
    Footer footer;
};

