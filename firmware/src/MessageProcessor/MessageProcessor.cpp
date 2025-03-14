#include "MessageProcessor.hpp"
#include "DebugPrinter.h"
#include "LedController/LedController.h"
#include "FlashStorage/FlashStorage.h"
#include "MotorController/MotorController.h"
#include "Ethernet.h"
MessageProcessor::MessageProcessor(uint32_t period)
{
  executionPeriod = period;
}

void MessageProcessor::AddExternalInterface(IExternalInterface *new_interface)
{
  externalInterfaces.push_back(new_interface);
  new_interface->SetProcessorInterface(this);
}

void MessageProcessor::OnStart()
{
}

void MessageProcessor::OnStop()
{
}

void MessageProcessor::OnRun()
{
}

void MessageProcessor::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
}

void MessageProcessor::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  Header *hdr = (Header *)&recv_bytes[0];

  // TODO: check for checksum

  //DEBUG_PRINTF("Received message type: 0x%x\n", hdr->message_type);

  switch (hdr->message_type)
  {
  case MessageTypes::SetLedColor:
  {
    SetLedColorMessage *msg = (SetLedColorMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting LED color to: R=%d, G=%d, B=%d\n", msg->ledColor.r, msg->ledColor.g, msg->ledColor.b);
    addrLedController.SetLEDColor(CRGB(msg->ledColor.r, msg->ledColor.g, msg->ledColor.b));
    break;
  }

  case MessageTypes::GetLedColor:
  {
    GetLedColorMessage *msg = (GetLedColorMessage *)recv_bytes;
    msg->header.message_type = MessageTypes::GetLedColor;
    msg->header.body_size = sizeof(GetLedColorMessage::ledColor);
    msg->ledColor.r = addrLedController.GetLedColor().r;
    msg->ledColor.g = addrLedController.GetLedColor().g;
    msg->ledColor.b = addrLedController.GetLedColor().b;
    msg->footer.checksum = 0;
    SendMsg(recv_bytes, sizeof(GetLedColorMessage));
    break;
  }

  case MessageTypes::GetVersion:
  {
    VersionMessage *msg = (VersionMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetVersion;
    msg->header.body_size = sizeof(VersionMessage::value);
    uint8_t *v_buf = (uint8_t *)&msg->value;
    sscanf(FIRMWARE_VERSION, "%u.%u.%u.%u", &v_buf[0], &v_buf[1], &v_buf[2], &v_buf[3]);
    // DEBUG_PRINTF("0x%8X\n", msg->value);
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(VersionMessage));
    break;
  }

  case MessageTypes::SetI2CAddress:
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)recv_bytes;
    FlashStorage::GetI2CSettings()->address = msg->value;
    break;
  }

  case MessageTypes::GetI2CAddress:
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetI2CAddress;
    msg->header.body_size = sizeof(I2CAddressMessage::value);
    msg->value = FlashStorage::GetI2CSettings()->address;
    msg->footer.checksum = 0;
    // DEBUG_PRINTF("Sending I2C Address: 0x%x\n", msg->value);
    SendMsg(send_buffer, sizeof(I2CAddressMessage));
    break;
  }

  case MessageTypes::SetEthernetAddress:
  {
    IPAddressMessage *msg = (IPAddressMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->ip_address = msg->value;
    // DEBUG_PRINT("Setting IP Address: ");
    // DEBUG_PRINTLN(IPAddress(msg->value));
    break;
  }

  case MessageTypes::GetEthernetAddress:
  {
    IPAddressMessage *msg = (IPAddressMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetEthernetAddress;
    msg->header.body_size = sizeof(IPAddressMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->ip_address;
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(IPAddressMessage));
    // DEBUG_PRINTF("Sending IP Address: 0x%x\n", sizeof(IPAddressMessage));
    break;
  }

  case MessageTypes::SetEthernetPort:
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->port = (uint16_t)(msg->value);
    // DEBUG_PRINTF("Setting Port: 0x%x\n", FlashStorage::GetEthernetSettings()->port);
    break;
  }

  case MessageTypes::GetEthernetPort:
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetEthernetPort;
    msg->header.body_size = sizeof(EthernetPortMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->port;
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(EthernetPortMessage));
    // DEBUG_PRINTF("Sending Port: %d\n", FlashStorage::GetEthernetSettings()->port);
    // DEBUG_PRINTF("Port msg size: %d\n", sizeof(EthernetPortMessage));
    break;
  }

  case MessageTypes::GetMacAddress:
  {
    MACAddressMessage *msg = (MACAddressMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetMacAddress;
    msg->header.body_size = sizeof(MACAddressMessage::mac);
    memcpy(&(msg->mac[0]), FlashStorage::GetMacAddress(), 6);
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(MACAddressMessage));
    break;
  }

  case MessageTypes::SaveSettings:
  {
    FlashStorage::WriteFlash();
    // DEBUG_PRINTLN("Writing flash");
    break;
  }

  case MessageTypes::SetMotorState:
  {
    SetMotorStateMessage *msg = (SetMotorStateMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor State to: %d\n", msg->value);
    motorController.SetMotorState((MotorStates)msg->value);
    break;
  }

  case MessageTypes::GetMotorState:
  {
    GetMotorStateMessage *msg = (GetMotorStateMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetMotorState;
    msg->header.body_size = sizeof(GetMotorStateMessage::value);
    msg->value = (uint8_t)motorController.GetMotorState();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetMotorStateMessage));
    // DEBUG_PRINTF("Sending Motor State: %d\n", (uint8_t)motorController.GetMotorState());
    break;
  }

  case MessageTypes::SetMotorBrake:
  {
    SetMotorBrakeMessage *msg = (SetMotorBrakeMessage *)&recv_bytes[0];
    motorController.SetMotorBraking((MotorBrake)msg->value);
    break;
  }

  case MessageTypes::SetMaxSpeed:
  {
    SetMaxSpeedMessage *msg = (SetMaxSpeedMessage *)&recv_bytes[0];
    motorController.SetMaxSpeed(msg->value);
    break;
  }

  case MessageTypes::GetMaxSpeed:
  {
    GetMaxSpeedMessage *msg = (GetMaxSpeedMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetMaxSpeed;
    msg->header.body_size = sizeof(GetMaxSpeedMessage::value);
    msg->value = motorController.GetMaxSpeed();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetPositionMessage));
    break;
  }

  case MessageTypes::SetAcceleration:
  {
    SetAccelerationMessage *msg = (SetAccelerationMessage *)&recv_bytes[0];
    motorController.SetAcceleration(msg->value);
    break;
  }

  case MessageTypes::GetAcceleration:
  {
    GetAccelerationMessage *msg = (GetAccelerationMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetAcceleration;
    msg->header.body_size = sizeof(GetAccelerationMessage::value);
    msg->value = motorController.GetAcceleration();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetAccelerationMessage));
    break;
  }

  case MessageTypes::SetPosition:
  {
    SetPositionMessage *msg = (SetPositionMessage *)&recv_bytes[0];
    motorController.SetPosition(msg->value);
    break;
  }

  case MessageTypes::GetPosition:
  {
    GetPositionMessage *msg = (GetPositionMessage*)send_buffer;
    msg->header.message_type = MessageTypes::GetPosition;
    msg->header.body_size = sizeof(GetPositionMessage::value);
    msg->value = motorController.GetPosition();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetPositionMessage));
    break;
  }

  case MessageTypes::SetTargetPosition:
  {
    SetTargetPositionMessage *msg = (SetTargetPositionMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor Position to: %f\n", msg->value);
    motorController.SetPositionTarget(msg->value);
    break;
  }

  case MessageTypes::GetTargetPosition:
  {
    GetPositionMessage *msg = (GetPositionMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetTargetPosition;
    msg->header.body_size = sizeof(GetPositionMessage::value);
    msg->value = motorController.GetPositionTarget();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetPositionMessage));
    // DEBUG_PRINTF("Sending Motor Position: %f\n", motorController.GetPositionTarget());
    break;
  }

  case MessageTypes::SetTargetPositionRelative:
  {
    SetTargetRelativePositionMessage *msg = (SetTargetRelativePositionMessage *)recv_bytes;
    motorController.SetPositionTargetRelative(msg->value);
    //DEBUG_PRINTF("Setting Motor Position Relative: %f\n", msg->value);
    break;
  }

  case MessageTypes::SetVelocity:
  {
    SetVelocityMessage *msg = (SetVelocityMessage *)recv_bytes;
    motorController.SetVelocityTarget(msg->value);
    // DEBUG_PRINTF("Setting Motor Velocity to: %d\n", msg->value);
    break;
  }

  case MessageTypes::GetVelocity:
  {
    GetVelocityMessage *msg = (GetVelocityMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetVelocity;
    msg->header.body_size = sizeof(GetVelocityMessage::value);
    msg->value = motorController.GetVelocityTarget();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetVelocityMessage));
    break;
  }

  case MessageTypes::SetHomeDirection:
  {
    SetHomeDirectionMessage *msg = (SetHomeDirectionMessage *)recv_bytes;
    motorController.SetHomeDirection((HomeDirection)msg->value);
    break;
  }

  case MessageTypes::GetHomeDirection:
  {
    GetHomeDirectionMessage *msg = (GetHomeDirectionMessage *)&send_buffer[0];
    msg->header.message_type = MessageTypes::GetHomeDirection;
    msg->header.body_size = sizeof(GetHomeDirectionMessage::value);
    msg->value = (uint8_t)motorController.GetHomeDirection();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(GetHomeDirectionMessage));
    break;
  }

  case MessageTypes::Home:
    motorController.Home();
    break;

  case MessageTypes::SetVelocityAndSteps:
  {
    SetVelocityAndStepsMessage *msg = (SetVelocityAndStepsMessage *)&recv_bytes[0];
    motorController.AddVelocityStep(msg->velocity, msg->steps);
    // DEBUG_PRINTF("Velocity: %x, Steps: %x", msg->velocity, msg->steps);
    break;
  }

  case MessageTypes::StartPath:
    motorController.StartPath();
    break;

  default:
    DEBUG_PRINTF("Unable to handle message type: 0x%x", hdr->message_type);
    break;
  }
}

void MessageProcessor::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size, IExternalInterface* calling_interface)
{
  last_interface_ = calling_interface;
  if (recv_bytes_size == 0)
    return;

  if (recv_bytes[0] == '{')
  {
    HandleJsonMsg(recv_bytes, recv_bytes_size);
  }
  else
  {
    HandleByteMsg(recv_bytes, recv_bytes_size);
  }
}

void MessageProcessor::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
  if(last_interface_!=nullptr)
  {
    last_interface_->SendMsg(send_bytes, send_bytes_size);
    return;
  }

  for (auto i : externalInterfaces)
  {
    i->SendMsg(send_bytes, send_bytes_size);
  }
  memset(send_bytes, 0, send_bytes_size);
}