#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <assert.h>
#include "DebugPrinter.h"

#define FLASH_ADDRESS 0b01011000
#define MAC_ARRAY_LEN 6
#define SERIAL_ARRAY_LEN 16

struct __attribute__ ((packed)) EthernetSettingsStruct
{
    uint16_t port;
    uint32_t ip_address;
};
struct __attribute__ ((packed)) I2CSettingsStruct
{
    uint8_t address;
};
struct __attribute__ ((packed)) MotorSettingsStruct
{
    uint8_t placeholder_settings;
};

struct __attribute__ ((packed)) FlashStorageStruct
{
    EthernetSettingsStruct EthernetSettings;
    I2CSettingsStruct I2CSettings;
    MotorSettingsStruct MotorSettings;
    uint8_t empty[248];
};

static_assert(sizeof(FlashStorageStruct) == 256, "Incorrect flash struct size");

namespace FlashStorage
{

    static const uint16_t EEPROM_SIZE_BYTES = 256;

    static const uint8_t MAC_DEVICE_ADDRESS = 0b01011000;
    static const uint8_t MAC_REGISTER_ADDRESS = 0x9A;
    static const uint8_t SN_DEVICE_ADDRESS = 0b01011000;
    static const uint8_t SN_REGISTER_ADDRESS = 0x80;
    static const uint16_t PORT_BASE = 12000;
    
    void Init();

    void PrintBuffer(uint8_t* buf);

    void writeRegister(uint8_t address, uint8_t data);
    uint8_t readRegister(uint8_t address);

    void writePage(uint8_t address, uint8_t *data, uint8_t data_len);
    void readBytes(uint8_t address, uint8_t *data, uint8_t data_len);

    uint8_t *GetMacAddress();
    uint8_t *GetSerialNumber();
    const char *GetMacAddressString();
    const char *GetSerialNumberString();

    EthernetSettingsStruct* GetEthernetSettings();
    I2CSettingsStruct* GetI2CSettings();
    MotorSettingsStruct* GetMotorSettings();

    void WriteFlash();

}