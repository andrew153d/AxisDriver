#include <Arduino.h>
#include <Wire.h>
#include "FlashStorage.h"

namespace FlashStorage
{
    bool has_read_mac_address = false;
    bool has_read_serial_number = false;

    uint32_t last_write_time = 0;
    static const uint32_t write_cycle_time = 5;

    uint8_t mac[MAC_ARRAY_LEN] = {0};
    uint8_t serial_number[SERIAL_ARRAY_LEN] = {0};

    static char macStr[18];
    static char snStr[32];

    void WaitWriteCycle()
    {
        while (millis() - last_write_time < write_cycle_time)
        {
            yield();
        }
    }

    void writeRegister(uint8_t address, uint8_t data)
    {
        WaitWriteCycle();
        Wire1.beginTransmission(0b1010000);
        Wire1.write(address);
        Wire1.write(data);
        last_write_time = millis();
        Wire1.endTransmission();
    }

    uint8_t readRegister(uint8_t address)
    {
        WaitWriteCycle();
        Wire1.beginTransmission(0b01010000);
        Wire1.write(address);
        last_write_time = millis();
        Wire1.endTransmission(false);
        Wire1.requestFrom(0b01010000, 1);
        uint8_t data = Wire1.read();
        return data;
    }

    void writePage(uint8_t address, uint8_t *data, uint8_t data_len)
    {
        if(data_len>16)
        {
            Serial.println("Attemptimg page write greater than 16 bytes");
            return;
        }
        if(address%16!=0)
        {
            Serial.println("Invalide Page Write Address");
            return;
        }
        WaitWriteCycle();
        Wire1.beginTransmission(0b1010000);
        Wire1.write(address);
        for(int i = 0; i<data_len; i++)
        {
            Wire1.write(data[i]);
        }
        last_write_time = millis();
        Wire1.endTransmission();
    }

    void readBytes(uint8_t address, uint8_t *data, uint8_t data_len)
    {
        if((uint16_t)address+(uint16_t)data_len > 256)
        {
            Serial.println("Address too high");
            return;
        }

        WaitWriteCycle();
        Wire1.beginTransmission(0b1010000);
        Wire1.write(address);
        last_write_time = millis();
        Wire1.endTransmission(false);

        Wire1.requestFrom(0b1010000, data_len);

        for (int i = 0; i < data_len; i++)
        {
            data[i] = Wire1.read();
        }

    }

    uint8_t *GetMacAddress()
    {
        if (has_read_mac_address)
            return &mac[0];

        WaitWriteCycle();
        Wire1.beginTransmission(MAC_DEVICE_ADDRESS);
        Wire1.write(MAC_REGISTER_ADDRESS);
        last_write_time = millis();
        Wire1.endTransmission(false);

        Wire1.requestFrom(MAC_DEVICE_ADDRESS, 6); // Read a byte

        for (int i = 0; i < 6; i++)
        {
            mac[i] = Wire1.read();
        }

        // Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        snprintf(macStr, sizeof(macStr),
                 "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        has_read_mac_address = true;
        return &mac[0];
    }

    uint8_t *GetSerialNumber()
    {
        if (has_read_serial_number)
            return &serial_number[0];

        WaitWriteCycle();
        Wire1.beginTransmission(SN_DEVICE_ADDRESS);
        Wire1.write(SN_REGISTER_ADDRESS);
        last_write_time = millis();
        Wire1.endTransmission(false);

        Wire1.requestFrom(SN_DEVICE_ADDRESS, SERIAL_ARRAY_LEN); // Read a byte

        for (int i = 0; i < SERIAL_ARRAY_LEN; i++)
        {
            serial_number[i] = Wire1.read();
        }

        // Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", serial_number[0], serial_number[1], serial_number[2], serial_number[3], serial_number[4], serial_number[5]);

        snprintf(snStr, sizeof(snStr),
                 "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
                 serial_number[0], serial_number[1], serial_number[2], serial_number[3], serial_number[4], serial_number[5], serial_number[6], serial_number[7], serial_number[8], serial_number[9], serial_number[10], serial_number[11], serial_number[12], serial_number[13], serial_number[14], serial_number[15]);

        has_read_serial_number = true;
        return &serial_number[0];
    }

    const char *GetMacAddressString()
    {
        GetMacAddress();
        return macStr;
    }

    const char *GetSerialNumberString()
    {
        GetSerialNumber();
        return snStr;
    }
}