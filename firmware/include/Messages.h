#pragma once

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
    GetMotorPosition = 0x5001,
    GetMotorVelocity = 0x5002,
    GetMotorCurrent = 0x5003,

    SetMotorPosition = 0x5004,
    SetMotorVelocity = 0x5005,
    SetMotorCurrent = 0x5006,

    SetControlMode = 0x5007,
    GetControlMode = 0x5008,

    GetMotorStatus = 0x5009,
    StartMotor = 0x500A,

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
};

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

PACKEDSTRUCT SetLedStateMessage
{
    Header header;
    LedStates ledState;
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


