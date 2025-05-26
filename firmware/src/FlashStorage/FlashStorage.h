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
enum class HAMode : uint8_t
{
    NONE = 0,
    VELOCITY_SWITCH = 1,
    POSITION_SWITCH = 2,
    VELOCITY_SLIDER = 3,
    POSITION_SLIDER = 4,
};
struct __attribute__ ((packed)) HASettingsStruct
{
    uint8_t enable;
    HAMode mode;
    uint8_t ha_ip_address[4];
    float velocity_switch_on_speed;
    float velocity_switch_off_speed;
    float position_switch_on_position;
    float position_switch_off_position;
    float velocity_slider_min;
    float velocity_slider_max;
    float position_slider_min;
    float position_slider_max;
    char mqtt_user[32];
    char mqtt_password[32];
    char mqtt_name[32];
    char mqtt_icon[32];
};

struct __attribute__ ((packed)) FlashStorageStruct
{
    EthernetSettingsStruct EthernetSettings;
    I2CSettingsStruct I2CSettings;
    MotorSettingsStruct MotorSettings;
    HASettingsStruct HASettings;
    uint8_t empty[82];
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
    HASettingsStruct* GetHASettings();
    uint8_t* GetBuffer();

    void WriteFlash();
    void Task();

}