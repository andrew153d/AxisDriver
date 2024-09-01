#pragma once
#include <cstdint>
#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t* recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t* send_bytes, uint32_t send_bytes_size);

class IEncoderInterface {
    public:
        virtual float GetVelocityDegreesPerSecond();
        virtual float GetPositionDegrees();
};


enum class MessageTypes : uint16_t{
// General Device Info
DeviceInfoMessageTypeLowerBounds = 0x0001,
GetVersion,
DeviceInfoMessageTypeUpperBounds = 0x00FF,

// Device Settings
SetI2CAddress = 0x0500,
GetI2CAddress = 0x0501,


//LED control
LedControlMessageTypeLowerBounds = 0x3000,
SetLedColor,
GetLedColor,
SetLedState,
LedControlMessageTypeUpperBounds = 0x30FF,

// Drive Configuration


// Motor Driving
GetMotorPosition = 0x5000,
GetMotorVelocity,
GetMotorCurrent,

SetMotorPosition,
SetMotorVelocity,
SetMotorCurrent,

SetControlMode,
GetControlMode,

GetMotorStatus,
};

PACKEDSTRUCT Header{
 MessageTypes message_type;
 uint16_t body_size;
};

PACKEDSTRUCT Footer{
 uint16_t checksum;
};

const uint16_t HEADER_SIZE = sizeof(Header);
const uint16_t FOOTER_SIZE = sizeof(Footer);

PACKEDSTRUCT GetVersionMessage{
    Header header;
    Footer footer;
};

PACKEDSTRUCT VersionMessage{
    Header header;
    char version[32];
    Footer footer;
};

PACKEDSTRUCT LedColors{
 union {
        struct {
            union {
                uint8_t r; 
                uint8_t red; 
            };
            union {
                uint8_t g; 
                uint8_t green; 
            };
            union {
                uint8_t b; 
                uint8_t blue; 
            };
        };
        uint8_t raw[3];
    };
};

PACKEDSTRUCT SetLedColorMessage{
    Header header;
    LedColors ledColor;
    Footer footer;
};