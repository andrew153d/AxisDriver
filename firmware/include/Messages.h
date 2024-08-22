#pragma once
#include <cstdint>
#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t* recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t* send_bytes, uint32_t send_bytes_size);


enum class MessageTypes : uint16_t{
// General Device Info
DeviceInfoMessageTypeLowerBounds = 0x0000,
GetVersionMessageId = 0x0001,
VersionMessageId = 0x0002,
DeviceInfoMessageTypeUpperBounds = 0x00FF,

// Device Settings
DeviceSettingsMessageTypeLowerBounds = 0x0500,
DeviceSettingsMessageTypeUpperBounds = 0x05FF,

//LED control
LedControlMessageTypeLowerBounds = 0x3000,
SetLed = 0x3001,
GetLed = 0x3002,
LedControlMessageTypeUpperBounds = 0x30FF,

// Drive Configuration


// Motor Driving
MotorControlMessageTypeLowerBounds = 0x5000,

MotorControlMessageTypeUpperBounds = 0x50FF,
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