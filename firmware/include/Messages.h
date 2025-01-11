#pragma once

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
    // General Device Info
    GetVersion = 0x0002,

    // Device Settings
    SetI2CAddress = 0x0500,
    GetI2CAddress = 0x0501,
    SetEthernetAddress = 0x0502,
    GetEthernetAddress = 0x0503,
    SetEthernetPort = 0x0504,
    GetEthernetPort = 0x0505,
    GetMacAddress = 0x0506,

    // LED control

    SetLedState = 0x3001,
    SetLedColor = 0x3002,
    GetLedColor = 0x3003,

    // Ethernet Configuration
    

    // Drive Configuration

    // Motor Driving
    SetMotorState = 0x5000,
    GetMotorState = 0x5001,

    GetTargetPosition = 0x5010,
    GetMotorVelocity = 0x5012,
    GetMotorCurrent = 0x5013,

    SetTargetPosition = 0x5014,
    SetTargetPositionRelative = 0x5015,
    SetMotorVelocity = 0x5016,
    SetMotorCurrent = 0x5017,
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
// *********************** Settings and Configuration *********************** //

typedef U32Message VersionMessage;
typedef U8Message I2CAddressMessage;
typedef U32Message IPAddressMessage;
typedef U32Message EthernetPortMessage;
PACKEDSTRUCT MACAddressMessage
{
    Header header;
    uint8_t mac[6];
    Footer footer;
};

// *********************** LED interface *********************** //

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

enum LedStates : uint8_t{
  OFF,
  FLASH_ERROR,
  ERROR,
  BOOTUP,
  RAINBOW,
  SOLID,
  MAX_VALUE
};

typedef U8Message SetLedStateMessage;
typedef U8Message GetLedStateMessage;

PACKEDSTRUCT SetLedColorMessage
{
    Header header;
    LedColors ledColor;
    Footer footer;
};

PACKEDSTRUCT GetLedColorMessage
{
    Header header;
    LedColors ledColor;
    Footer footer;
};

// *********************** Ethernet interface *********************** //


PACKEDSTRUCT SetEthernetIpAddressMessage
{
    Header header;
    uint32_t ip_address;
    Footer footer;
};

PACKEDSTRUCT SetEthernetPortMessage
{
    Header header;
    uint32_t ip_address;
    Footer footer;
};

// *********************** Motor interface *********************** //

enum class MotorStates : uint8_t{
  OFF,
  POSITION,
  VELOCITY,
  IDLE
};

typedef U8Message SetMotorStateMessage;
typedef U8Message GetMotorStateMessage;

typedef DoubleMessage SetMotorPositionMessage;
typedef DoubleMessage GetMotorPositionMessage;
typedef DoubleMessage SetMotorVelocityMessage;
typedef DoubleMessage GetMotorVelocityMessage;
typedef DoubleMessage SetMotorCurrentMessage;
typedef DoubleMessage GetMotorCurrentMessage;