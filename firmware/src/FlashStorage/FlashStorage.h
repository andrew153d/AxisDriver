#pragma once

#include <Arduino.h>
#include <Wire.h>

#define FLASH_ADDRESS 0b01011000
#define MAC_ARRAY_LEN 6
namespace FlashStorage
{
    uint8_t readMAC();
    void writeRegister(uint8_t address, uint8_t data);
    uint8_t* GetMacAddress();
    const char* GetMacAddressString();
}