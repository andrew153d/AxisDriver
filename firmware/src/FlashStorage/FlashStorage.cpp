#include <Arduino.h>
#include <Wire.h>
#include "FlashStorage.h"

namespace FlashStorage
{
    bool has_read_mac_address = false;
    uint8_t mac[MAC_ARRAY_LEN] = {0};
    static char macStr[18];

    uint8_t readMAC(uint8_t r);
    void writeRegister(uint8_t address, uint8_t data);

    uint8_t* GetMacAddress()
    {
        if(has_read_mac_address)
            return &mac[0];
        //Serial.println("reading mac address");   
        readMAC();
        // delay(1);
        // mac[1] = readRegister(0xFB);
        // delay(1);
        // mac[2] = readRegister(0xFC);
        // delay(1);
        // mac[3] = readRegister(0xFD);
        // delay(1);
        // mac[4] = readRegister(0xFE);
        // delay(1);
        // mac[5] = readRegister(0xFF);
        //Serial.println("got mac addss");
        snprintf(macStr, sizeof(macStr),
                 "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        has_read_mac_address = true;
        return &mac[0];
    }

    uint8_t readMAC()
    {
        static const uint8_t addr = 0b01011000;
        Wire1.beginTransmission(addr);
        Wire1.write(0x9A); // Register to read
        Wire1.endTransmission(false);

        Wire1.requestFrom(addr, 6); // Read a byte
        
        for(int i = 0; i<6; i++)
        {
            mac[i] = Wire1.read();
        }
        return 0;
    }
    

    void writeRegister(uint8_t address, uint8_t data)
    {
        Wire1.beginTransmission(FLASH_ADDRESS);
        Wire1.write(address);
        Wire1.write(data);
        Wire1.endTransmission();
    }

    const char* GetMacAddressString()
    {
        GetMacAddress();
        return macStr;
    }
}