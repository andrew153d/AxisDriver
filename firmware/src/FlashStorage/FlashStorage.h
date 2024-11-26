#pragma once

#include <Arduino.h>
#include <Wire.h>

#define FLASH_ADDRESS 0b01011000
#define MAC_ARRAY_LEN 6
#define SERIAL_ARRAY_LEN 16
namespace FlashStorage
{

    static const uint8_t MAC_DEVICE_ADDRESS = 0b01011000;
    static const uint8_t MAC_REGISTER_ADDRESS = 0x9A;
    static const uint8_t SN_DEVICE_ADDRESS = 0b01011000;
    static const uint8_t SN_REGISTER_ADDRESS = 0x80;
    
    void writeRegister(uint8_t address, uint8_t data);
    uint8_t *GetMacAddress();
    uint8_t *GetSerialNumber();
    const char *GetMacAddressString();
    const char *GetSerialNumberString();
}