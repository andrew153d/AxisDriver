#pragma once
#include <cstdint>
#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t* recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t* send_bytes, uint32_t send_bytes_size);


enum class MessageTypes : uint8_t{
// General Device Info


// Device Settings


//LED control
LedControlMessageTypeLowerBounds = 0x40,
SetLed = 0x41,
GetLed = 0x42,
LedControlMessageTypeUpperBounds = 0x50,

// Drive Configuration


// Motor Driving

};

PACKEDSTRUCT Header{
 MessageTypes message_type;
 uint16_t message_size; //size of header+body+footer
 uint16_t body_size;
};

PACKEDSTRUCT Footer{
 uint16_t checksum;
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