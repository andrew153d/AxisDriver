#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include "Ethernet.h"
#define LED_BUILTIN STAT_LED
#define FLASH_ADDRESS 0x50

void setUp(void)
{
  Wire1.begin();
}

void tearDown(void)
{
  // clean stuff up here
}

void test_i2c_addresses()
{
  byte error, address;
  int nDevices;

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      Serial.println(address, HEX);
    }
    delay(2);
  }

  TEST_ASSERT_GREATER_OR_EQUAL(2, nDevices);
}

byte readRegister(byte r)
{
  unsigned char v;
  Wire1.beginTransmission(FLASH_ADDRESS);
  Wire1.write(r); // Register to read
  Wire1.endTransmission();

  Wire1.requestFrom(FLASH_ADDRESS, 1); // Read a byte
  while (!Wire1.available())
  {
    // Wait
  }
  v = Wire1.read();
  return v;
}

void test_w5500_presence()
{
  byte mac[] = {
      0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip(192, 168, 12, 177);
  Ethernet.init(ETH_CS);
  Ethernet.begin(mac, ip);
  bool presenet = (Ethernet.hardwareStatus() == EthernetHardwareStatus::EthernetW5500);
  TEST_ASSERT_TRUE(presenet);
}

void test_mac_address()
{

  uint8_t mac[6] = {0};
  Wire1.beginTransmission(FLASH_ADDRESS);
  Wire1.write(0xFA); // Register to read
  Wire1.endTransmission();

  Wire1.requestFrom(FLASH_ADDRESS, 6); // Read a byte
  Wire1.readBytes(&mac[0], 6);
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  uint32_t sum = 0;
  for (int i = 0; i < 6; i++)
  {
    sum += mac[i];
  }

  TEST_ASSERT_TRUE(mac[0] != 0xFF);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  // Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  UNITY_BEGIN(); // IMPORTANT LINE!
  //RUN_TEST(test_i2c_addresses);
  //RUN_TEST(test_mac_address);
  RUN_TEST(test_w5500_presence);
  Serial.println("Done");
  UNITY_END();
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop()
{

  // stop unit testing
}