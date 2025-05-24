#include "AxisEthernet.h"
#include "utility/w5100.h"
#include "Site.h"
#include "../EncoderController/EncoderController.h"
#include "../LedController/LedController.h"
void HandleInturrupts();

void AxisEthernet::OnStart()
{
    pinMode(nRST, OUTPUT);
    digitalWrite(nRST, LOW);
    delay(500); // increase this if issues with link
    digitalWrite(nRST, HIGH);

    pinMode(nINT, INPUT);
    pinMode(nCS, OUTPUT);
    digitalWrite(nCS, LOW);

    // set mode
    pinMode(PMODE0, OUTPUT);
    pinMode(PMODE1, OUTPUT);
    pinMode(PMODE2, OUTPUT);
    digitalWrite(PMODE0, HIGH);
    digitalWrite(PMODE1, HIGH);
    digitalWrite(PMODE2, HIGH);

    Ethernet.init(nCS);
    // DEBUG_PRINTF("Initializing ethernet with ip 0x%X\n", FlashStorage::GetEthernetSettings()->ip_address);
    Ethernet.begin(FlashStorage::GetMacAddress(), IPAddress(FlashStorage::GetEthernetSettings()->ip_address));

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        DEBUG_PRINTLN("Ethernet shield not found");
    }
    else
    {
        DEBUG_PRINTLN("Found Ethernet Hat");
        device_found = true;
    }

    // if (Ethernet.linkStatus() == LinkOFF)
    // {
    //     // Serial.println("Ethernet cable is not connected");
    // }
    // else
    // {
    //     // Serial.println("Detected Ethernet cable");
    // }
    Udp.begin(FlashStorage::GetEthernetSettings()->port);

    // W5100.writeIR();
    // W5100.writeIMR();

    W5100.writeSIMR(1 << Udp.GetSocketIndex());

    W5100.writeSnIMR(Udp.GetSocketIndex(), 0xF);
    // W5100.writeSnIR()
    uint16_t int_level = 0x0FFF;
    W5100.writeINTLEVEL((uint8_t*)&int_level);
    DEBUG_PRINT("Ip Address: ");
    DEBUG_PRINTLN(Ethernet.localIP());
    DEBUG_PRINTF("Port: 0x%x\n", Udp.localPort());

    interrupts();
    NVIC_EnableIRQ(EIC_3_IRQn);
    NVIC_SetPriority(EIC_3_IRQn, 1);
    attachInterrupt(AUX4, HandleInturrupts, CHANGE);
}

void AxisEthernet::HandleInturrupt()
{
    uint8_t intr = W5100.readSnIR(Udp.GetSocketIndex());
    W5100.writeSnIR(Udp.GetSocketIndex(), intr);
    //DEBUG_PRINTF("interupt: 0x%x\n", intr);
    
    while (true)
    {
        uint16_t packetSize = Udp.parsePacket();
        if (packetSize == 0)
            return;
        remoteIp = Udp.remoteIP();
        remotePort = Udp.remotePort();
        //DEBUG_PRINTF("Received message from %d.%d.%d.%d:%d\n", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3], remotePort);
    
        //DEBUG_PRINTF("Inturrupt received size %d\n", packetSize);
        memset(buffer, 0, 1024);
        Udp.read(buffer, packetSize);
        HandleIncomingMsg(buffer, packetSize);
    }
}

void HandleInturrupts()
{
    AEthernet.HandleInturrupt();
}

void AxisEthernet::OnRun()
{
    uint16_t packetSize = Udp.parsePacket();
    if (packetSize == 0)
        return;
    //DEBUG_PRINTF("Run Received %d\n", packetSize);
    
    remoteIp = Udp.remoteIP();
    remotePort = Udp.remotePort();
    //DEBUG_PRINTF("Received message from %d.%d.%d.%d:%d\n", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3], remotePort);
    memset(buffer, 0, 1024);
    Udp.read(buffer, packetSize);
    HandleIncomingMsg(buffer, packetSize);
}

void AxisEthernet::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // for (uint32_t i = 0; i < recv_bytes_size; i++)
    // {
    //     DEBUG_PRINT("0x");
    //     if (recv_bytes[i] < 0x10)
    //     {
    //         Serial.print("0");
    //     }
    //     DEBUG_PRINT(recv_bytes[i], HEX);
    //     DEBUG_PRINT(" ");
    // }
    // DEBUG_PRINTLN();
    // DEBUG_PRINTF("Received %d bytes\n", recv_bytes_size);
    if (processor_interface_ != nullptr)
    {
        processor_interface_->HandleIncomingMsg(recv_bytes, recv_bytes_size, this);
    }
    else
    {
        Serial.println("processor_interface is null");
    }
}

void AxisEthernet::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) 
{
    Udp.beginPacket(remoteIp, remotePort);
    Udp.write(send_bytes, send_bytes_size);
    Udp.endPacket();
};

void AxisEthernet::OnStop()
{
}

AxisEthernet AEthernet(10);
