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

    //W5100.writeIR();
    //W5100.writeIMR();
    
    W5100.writeSIMR(1<<Udp.GetSocketIndex());
    
    W5100.writeSnIMR(Udp.GetSocketIndex(), 0xF);
    //W5100.writeSnIR()

    DEBUG_PRINT("Ip Address: ");
    DEBUG_PRINTLN(Ethernet.localIP());
    DEBUG_PRINTF("Port: 0x%x\n", Udp.localPort());
    // DEBUG_PRINTF("Ip Address: %x, Port: %X\n", Ethernet.localIP(), Udp.localPort());
    // DEBUG_PRINTLN(Ethernet.localIP());
    
    interrupts();
    NVIC_EnableIRQ(EIC_3_IRQn);
    NVIC_SetPriority(EIC_3_IRQn, 2);
    attachInterrupt(AUX4, HandleInturrupts, CHANGE);
}

void AxisEthernet::HandleInturrupt()
{
    uint8_t intr = W5100.readSnIR(Udp.GetSocketIndex());
    //DEBUG_PRINTF("interupt: 0x%x\n", intr);
    W5100.writeSnIR(Udp.GetSocketIndex(), intr);
    uint16_t packetSize = Udp.parsePacket();
    if (packetSize == 0)
        return;

    Udp.read(buffer, packetSize);
    
    HandleIncomingMsg(buffer, packetSize);

    return;
}

void HandleInturrupts()
{
    AEthernet.HandleInturrupt();
}

void AxisEthernet::OnRun()
{
    
}

void AxisEthernet::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    if(processor_interface_!=nullptr){
        processor_interface_->HandleIncomingMsg(recv_bytes, recv_bytes_size);
    }else{
        Serial.println("processor_interface is null");
    }
}

void AxisEthernet::SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size){};

void AxisEthernet::OnStop()
{
}

AxisEthernet AEthernet(10);
