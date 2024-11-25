#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "Task.h"
#include "MessageProcessor/MessageProcessor.h"



class AxisEthernet : public ITask, public IInternalInterface
{
    private:
    const uint32_t PMODE0 = AUX2;
    const uint32_t PMODE1 = AUX1;
    const uint32_t PMODE2 = AUX5;

    const uint32_t nRST = AUX6;
    const uint32_t nINT = AUX4;
    const uint32_t nCS = ETH_CS;
public:
   
    EthernetServer server;
    IPAddress ip;

    AxisEthernet(int p):server(80),ip(192,168,12,177)
    {
        executionPeriod = p;
        mac[0] = 0xDE;
        mac[1] = 0xAD;
        mac[2] = 0xBE;
        mac[3] = 0xEF;
        mac[4] = 0xFE;
        mac[5] = 0xED;
    }

    uint8_t mac[6];    


    void OnStart();
    void OnStop();
    void OnRun();

    // messageHandlers
    void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
 
    void HandleInturrupt();
    
};

extern AxisEthernet AEthernet;
