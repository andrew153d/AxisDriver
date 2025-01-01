#include <Arduino.h>
#include <unity.h>
#include <SPI.h>
#include <Ethernet.h>
#include "utility/w5100.h"
#include "FlashStorage.h"

const uint32_t PMODE0 = AUX2;
const uint32_t PMODE1 = AUX1;
const uint32_t PMODE2 = AUX5;

const uint32_t nRST = AUX6;
const uint32_t nINT = AUX4;
const uint32_t nCS = ETH_CS;

IPAddress ip;

void setUp(void)
{
    pinMode(PMODE0, OUTPUT);
    pinMode(PMODE1, OUTPUT);
    pinMode(PMODE2, OUTPUT);
    digitalWrite(PMODE0, HIGH);
    digitalWrite(PMODE1, HIGH);
    digitalWrite(PMODE2, HIGH);
    pinMode(nRST, OUTPUT);
    digitalWrite(nRST, HIGH);
    pinMode(nINT, INPUT);
    pinMode(nCS, OUTPUT);
    digitalWrite(nCS, LOW);

    ip.fromString("192.168.12.177");
}

void tearDown(void)
{
}

void test_ethernet_presence()
{
    Ethernet.init(nCS);
    Ethernet.begin(FlashStorage::GetMacAddress(), ip);
    TEST_ASSERT_EQUAL(EthernetHardwareStatus::EthernetW5500, Ethernet.hardwareStatus());
}

void test_cable_connected()
{
    while(Ethernet.linkStatus() == EthernetLinkStatus::LinkOFF)
    {
        delay(1000);
        Serial.println("Waiting for ethernet status");
    }
    TEST_ASSERT_EQUAL(EthernetLinkStatus::LinkON, Ethernet.linkStatus());
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    UNITY_BEGIN();
    RUN_TEST(test_ethernet_presence);
    RUN_TEST(test_cable_connected);
    UNITY_END();
}

void loop()
{

    // stop unit testing
}