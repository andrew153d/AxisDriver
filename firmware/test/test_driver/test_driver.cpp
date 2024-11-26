#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include "Ethernet.h"
#include "FlashStorage.h"

#define LED_BUILTIN STAT_LED

void setUp(void)
{
  Wire1.begin();
}

void tearDown(void)
{
  // clean stuff up here
}

void test_mac_address()
{
  // using namespace FlashStorage;
  uint8_t* mac_address = FlashStorage::GetMacAddress();
  Serial.print("MAC Address: ");
  Serial.println(FlashStorage::GetMacAddressString());
  
  TEST_ASSERT_TRUE(mac_address[0] != 0xFF && mac_address[0] != 0x00);
}

void test_serial_number()
{
  uint8_t* serial_number = FlashStorage::GetSerialNumber();
  Serial.print("Serial Number: ");
  Serial.println(FlashStorage::GetSerialNumberString());
  
  TEST_ASSERT_TRUE(serial_number[0] != 0xFF && serial_number[0] != 0x00);
}

void test_flash_byte_read_write()
{
  FlashStorage::writeRegister(0x22, 0x54);
  uint8_t test = FlashStorage::readRegister(0x22);
  TEST_ASSERT_EQUAL(0x54, test);
}

void test_flash_page_read_write()
{
  uint8_t test_page[] = {45, 23, 76, 72, 112, 36, 118, 15, 45, 156, 148, 1, 246, 4, 45, 111};
  uint8_t test_result[16] = {0};
  FlashStorage::writePage(0x90, &test_page[0], sizeof(test_page));
  
  //uint8_t test_read = FlashStorage::readRegister(0x90 + 5);
  FlashStorage::readBytes(0x90, &test_result[0], sizeof(test_page));
  
  TEST_ASSERT_EQUAL(test_page[15], test_result[15]);
}

void setup()
{

  Serial.begin(115200);
  while(!Serial);

  UNITY_BEGIN(); // IMPORTANT LINE!
  RUN_TEST(test_mac_address);
  RUN_TEST(test_serial_number);
  RUN_TEST(test_flash_byte_read_write);
  RUN_TEST(test_flash_page_read_write);
  UNITY_END();
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop()
{

  // stop unit testing
}