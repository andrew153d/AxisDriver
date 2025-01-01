#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include "FastLED.h"
#include "Ethernet.h"
#include "FlashStorage.h"
#include <AccelStepper.h>
#include <easyTMC2209.h>
#define LED_BUILTIN STAT_LED

// Variables for TMC2209
HardwareSerial &serial_stream = Serial1;
easyTMC2209 driver = easyTMC2209();
AccelStepper stepper(stepper.DRIVER, MOTOR_STEP, MOTOR_DIR);

void setUp(void)
{
  Wire1.begin();
  Wire1.setClock(400000);

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);
  pinMode(MOTOR_SPREAD, OUTPUT);
  pinMode(MOTOR_DIAG, INPUT);

  digitalWrite(MOTOR_M0, LOW);
  digitalWrite(MOTOR_M1, LOW);
  digitalWrite(MOTOR_EN, HIGH);
  digitalWrite(MOTOR_SPREAD, LOW);
}

void tearDown(void)
{
  // clean stuff up here
}
namespace FLASH_AT24MAC402
{
  void test_mac_address()
  {
    uint8_t *mac_address = FlashStorage::GetMacAddress();
    Serial.print("MAC Address: ");
    Serial.println(FlashStorage::GetMacAddressString());

    TEST_ASSERT_TRUE(mac_address[0] != 0xFF && mac_address[0] != 0x00);
  }

  void test_serial_number()
  {
    uint8_t *serial_number = FlashStorage::GetSerialNumber();
    Serial.print("Serial Number: ");
    Serial.println(FlashStorage::GetSerialNumberString());

    TEST_ASSERT_TRUE(serial_number[0] != 0xFF && serial_number[0] != 0x00);
  }

  void test_flash_byte_read_write()
  {
    FlashStorage::writeRegister(256, 0x54);
    uint8_t test = FlashStorage::readRegister(256);
    TEST_ASSERT_EQUAL(0x54, test);
  }

  void test_flash_page_read_write()
  {
    uint8_t test_page[] = {45, 23, 76, 72, 112, 36, 118, 15, 45, 156, 148, 1, 246, 4, 45, 111};
    uint8_t test_result[16] = {0};
    FlashStorage::writePage(0x90, &test_page[0], sizeof(test_page));

    // uint8_t test_read = FlashStorage::readRegister(0x90 + 5);
    FlashStorage::readBytes(0x90, &test_result[0], sizeof(test_page));

    TEST_ASSERT_EQUAL(test_page[15], test_result[15]);
  }

  void test_entire_flash_read_write()
  {
    uint8_t test_data[256];
    uint8_t test_result[256] = {0};

    for (uint8_t i = 0; i < 256; i++)
    {
      test_data[i] = i;
    }

    for (uint8_t i = 0; i < 256; i += 16)
    {
      FlashStorage::writePage(i, &test_data[i], 16);
    }

    for (uint8_t i = 0; i < 256; i += 16) {
        FlashStorage::readBytes(i, &test_result[i], 16);
    }
    
    TEST_ASSERT_EQUAL_CHAR_ARRAY(test_data, test_result, 256);
  }
}

void test_addr_led()
{
  CRGB leds[1];
  FastLED.addLeds<NEOPIXEL, NEOPIX>(leds, 1);
  for (int h = 0; h < 256; h++)
  {
    leds[0] = CHSV(h, 255, 100);
    FastLED.show();
    delay(4);
  }
  leds[0] = CRGB::Black;
  FastLED.show();
  TEST_ASSERT(true);
}

void test_stat_led()
{
  pinMode(STAT_LED, OUTPUT);
  digitalWrite(STAT_LED, HIGH);
  delay(1000);
  digitalWrite(STAT_LED, LOW);
  TEST_ASSERT(true);
}

namespace TMC2209
{
  void test_tmc2209_present()
  {
    stepper.setEnablePin(MOTOR_EN);
    stepper.setPinsInverted(true, false, true);
    stepper.disableOutputs();
    stepper.setAcceleration(800 * 64);
    stepper.setMaxSpeed(2500 * 64);

    driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);

    int ver = driver.getVersion();

    TEST_ASSERT_EQUAL(ver, 33);
  }
}

void test_usb_sense()
{
  float voltage = (float)analogRead(PIN_VUSB) * (3.3f / 1024.0f) * 2;
  Serial.print("USB Voltage: ");
  Serial.println(voltage);
  TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(4.5, voltage);
}

void test_vbus_sense()
{
  float voltage = analogRead(PIN_VBUS) * (24.0f / 553.0f);
  Serial.println(voltage);

  TEST_ASSERT_GREATER_THAN_FLOAT(7, voltage);
}

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  UNITY_BEGIN();
  RUN_TEST(test_stat_led);
  RUN_TEST(test_addr_led);
  RUN_TEST(test_usb_sense);
  // RUN_TEST(test_vbus_sense);
  RUN_TEST(TMC2209::test_tmc2209_present);
  RUN_TEST(FLASH_AT24MAC402::test_mac_address);
  RUN_TEST(FLASH_AT24MAC402::test_serial_number);
  RUN_TEST(FLASH_AT24MAC402::test_flash_byte_read_write);
  RUN_TEST(FLASH_AT24MAC402::test_flash_page_read_write);
  //RUN_TEST(FLASH_AT24MAC402::test_entire_flash_read_write);
  UNITY_END();
}

void loop()
{

  // stop unit testing
}