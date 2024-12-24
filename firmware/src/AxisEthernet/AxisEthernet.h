#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.h"
#include "FlashStorage/FlashStorage.h"
#include "DebugPrinter.h"
#include <EthernetUdp.h> 

class AxisEthernet : public ITask, public IInternalInterface
{
private:
    const uint32_t PMODE0 = AUX2;
    const uint32_t PMODE1 = AUX1;
    const uint32_t PMODE2 = AUX5;

    const uint32_t nRST = AUX6;
    const uint32_t nINT = AUX4;
    const uint32_t nCS = ETH_CS;

    bool device_found = false;

    EthernetUDP Udp; 
    uint8_t buffer[1024];
    
public:
    // EthernetServer server;
    IPAddress ip;

    AxisEthernet(int p)
    {
        executionPeriod = p;
    }

    void OnStart();
    void OnStop();
    void OnRun();

    bool IsPresent(){ return device_found; }
    // messageHandlers
    void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);

    void HandleInturrupt();
};

extern AxisEthernet* AEthernet;
