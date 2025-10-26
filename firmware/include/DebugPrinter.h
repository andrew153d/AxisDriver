#pragma once

//#define DEBUG_SERIAL
//#define DEBUG_UDP

#ifdef DEBUG_SERIAL
#define DEBUG_PRINT(...)           \
    {                              \
        Serial.print(__VA_ARGS__); \
    }
#define DEBUG_PRINTLN(...)           \
    {                                \
        Serial.println(__VA_ARGS__); \
    }
#define DEBUG_PRINTF(...)           \
    {                               \
        Serial.printf(__VA_ARGS__); \
    }
#define DEBUG_BEGIN(...)           \
    {                              \
        Serial.begin(__VA_ARGS__); \
    }
#elif defined(DEBUG_UDP)
#include "EthernetHAT/AxisEthernet.h"
#define DEBUG_PRINT(...)              \
    {                                 \
        AEthernet.print(__VA_ARGS__); \
    }
#define DEBUG_PRINTLN(...)              \
    {                                   \
        AEthernet.println(__VA_ARGS__); \
    }
#define DEBUG_PRINTF(...)              \
    {                                  \
        AEthernet.printf(__VA_ARGS__); \
    }
#define DEBUG_BEGIN(...) \
    {                    \
    }
#else
#define DEBUG_PRINT(...) \
    {                    \
    }
#define DEBUG_PRINTLN(...) \
    {                      \
    }
#define DEBUG_PRINTF(...) \
    {                     \
    }
#define DEBUG_BEGIN(...) \
    {                    \
    }
#endif
