#include "DeviceManager.h"

void DeviceManager::OnStart()
{
}

void DeviceManager::OnStop()
{
}

void DeviceManager::OnRun()
{
}

void DeviceManager::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
    {
        Serial.println("Size too small");
    }

    Header *header = (Header *)recv_bytes;

    if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
    {
        Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
        return;
    }

    switch (header->message_type)
    {
    case MessageTypes::GetVersionMessageId:
        VersionMessage msg;
        msg.header.message_type = MessageTypes::VersionMessageId;
        msg.header.body_size = sizeof(msg.version);
        strcpy(msg.version, "AxisDriver V0.0");
        msg.footer.checksum = 0;
        SendMsg((uint8_t*)&msg, sizeof(VersionMessage));
        break;
    default:
        break;
    }
}

void DeviceManager::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
    processor_interface_->SendMsg(send_bytes, send_bytes_size);
}
