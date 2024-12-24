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
    delay(500); //increase this if issues with link
    digitalWrite(nRST, HIGH);
    
    pinMode(nINT, INPUT);
    pinMode(nCS, OUTPUT);
    digitalWrite(nCS, LOW);

    //set mode
    pinMode(PMODE0, OUTPUT);
    pinMode(PMODE1, OUTPUT);
    pinMode(PMODE2, OUTPUT);
    digitalWrite(PMODE0, HIGH);
    digitalWrite(PMODE1, HIGH);
    digitalWrite(PMODE2, HIGH);

    DEBUG_PRINTLN("initting");
    Ethernet.init(nCS);
    DEBUG_PRINTLN("begind");


    DEBUG_PRINTF("MAC: %s, IP: %x", FlashStorage::GetMacAddressString(), FlashStorage::GetEthernetSettings()->ip_address);  

    Ethernet.begin(FlashStorage::GetMacAddress(), IPAddress(FlashStorage::GetEthernetSettings()->ip_address));
    
    DEBUG_PRINTF("Mac Address: %s\n", FlashStorage::GetMacAddressString());
    DEBUG_PRINTF("IP address from flash: 0x%x\n", FlashStorage::GetEthernetSettings()->ip_address);
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

    DEBUG_PRINT("Ip Address: ");
    DEBUG_PRINTLN(Ethernet.localIP());
    // interrupts();
    attachInterrupt(AUX4, HandleInturrupts, RISING);
    
}

void AxisEthernet::HandleInturrupt()
{
    Serial.println("int");
    return;
}

void HandleInturrupts()
{
    // if(AEthernet!=nullptr)
    //     AEthernet->HandleInturrupt();
}
void AxisEthernet::OnRun()
{
    uint16_t packetSize = Udp.parsePacket();
    if(packetSize == 0)
        return;

    Udp.read(buffer, packetSize); 
    DEBUG_PRINT((char*)&buffer[0]);
}

void AxisEthernet::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // if (recv_bytes_size == 0)
    //     return;

    // if ((char)recv_bytes[0] == '{')
    //     HandleJsonMsg(recv_bytes, recv_bytes_size);
    // else
    //     HandleByteMsg(recv_bytes, recv_bytes_size);
}

void AxisEthernet::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // DeserializationError error = deserializeJson(recvd_json, recv_bytes, 256);
    // if (error)
    // {
    //     Serial.println("Error parsing json in motor controller");
    //     return;
    // }

    // String command = "";

    // if (!TryParseJson<String>("mode", &command, recvd_json))
    // {
    //     Serial.println("Failed to parse encoder mode");
    //     return;
    // }

    // EncoderCommands encoder_command = ToEncoderCommandsEnum(command);

    // switch (encoder_command)
    // {
    // case EncoderCommands::SetPosition:
    // {
    //     float offset = 0;
    //     if (!TryParseJson<float>("position", &offset, recvd_json))
    //     {
    //         Serial.println("Failed to parse position");
    //         return;
    //     }
    //     SetPosition(offset);
    //     break;
    // }
    // case EncoderCommands::GetPosition:
    // {
    //     recvd_json.clear();
    //     recvd_json["type"] = ENCODERTYPESTRING;
    //     recvd_json["mode"] = GETPOSITIONCOMMAND;
    //     recvd_json["position"] = GetShaftAngle();
    //     String output;
    //     serializeJson(recvd_json, output);
    //     Serial.println(output);
    //     break;
    // }
    // case EncoderCommands::GetUpdateRate:
    // {

    //     recvd_json.clear();
    //     recvd_json["type"] = ENCODERTYPESTRING;
    //     recvd_json["mode"] = GETUPDATERATECOMMAND;
    //     recvd_json["rate"] = GetUpdateRate();
    //     String output;
    //     serializeJson(recvd_json, output);
    //     Serial.println(output);
    //     break;
    // }
    // }
}

void AxisEthernet::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
    // {
    //     Serial.println("Size too small");
    // }

    // Header *header = (Header *)recv_bytes;
    // if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
    // {
    //     Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
    //     return;
    // }

    // //   switch (header->message_type)
    // //   {
    // //   break;
    // //   default:
    // //     break;
    // //   }
}

void AxisEthernet::OnStop()
{
}
