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
    
}

void DeviceManager::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
    processor_interface_->SendMsg(send_bytes, send_bytes_size);
}
