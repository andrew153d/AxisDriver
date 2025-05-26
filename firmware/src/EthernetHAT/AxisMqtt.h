#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.hpp"
#include "FlashStorage/FlashStorage.h"
#include "DebugPrinter.h"
#include <EthernetUdp.h> 
#include <Ethernet.h>
#include <ArduinoHA.h>
#define BROKER_ADDR IPAddress(192, 168, 12, 143)
class MQTTTask : public ITask, public IExternalInterface
{
private:
    HASettingsStruct *ha_settings;

    EthernetClient client;
    HADevice* device;
    HAMqtt* mqtt;
    
    HASwitch* velocity_switch;
    HASwitch* position_switch;
    HANumber* velocity_slider;
    HANumber* position_slider;

    bool connected = false;

    void OnVelocitySwitchCommand(bool state);
    void OnPositionSwitchCommand(bool state);
    void OnVelocitySliderCommand(const HANumeric& value);
    void OnPositionSliderCommand(const HANumeric& value);
public:
    // EthernetServer server;
    IPAddress ip;

    MQTTTask(int p);
    ~MQTTTask();
    void OnStart();
    void OnStop();
    void OnRun();

    // messageHandlers
    void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);
};

extern MQTTTask mqttTask;
