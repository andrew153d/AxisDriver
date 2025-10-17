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

  // DEBUG_PRINTF("Received message type: 0x%x\n", hdr->message_type);

  switch ((MessageTypes)hdr->message_type)
  {
  case MessageTypes::GetVersionId: // 0x0000
  {
    VersionMessage *msg = (VersionMessage *)&send_buffer[0];

    msg->header.message_type = (uint16_t)MessageTypes::GetVersionId;
    msg->header.body_size = sizeof(VersionMessage::value);
    uint8_t *v_buf = (uint8_t *)&msg->value;
    sscanf(FIRMWARE_VERSION, "%u.%u.%u.%u", &v_buf[0], &v_buf[1], &v_buf[2], &v_buf[3]);
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(VersionMessage));
    // DEBUG_PRINTF("0x%8X\n", msg->value);
    break;
  }

  case MessageTypes::SetI2CAddressId:
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)recv_bytes;
    FlashStorage::GetI2CSettings()->address = msg->value;

    AckMessage *ack_msg = (AckMessage *)&send_buffer[0];
    ack_msg->header.message_type = (uint16_t)MessageTypes::AckId;
    ack_msg->header.body_size = sizeof(AckMessage::ack_message_type) + sizeof(AckMessage::status);
    ack_msg->ack_message_type = (uint16_t)MessageTypes::SetI2CAddressId;
    ack_msg->status = (uint8_t)StatusCodes::SUCCESS;
    ack_msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(AckMessage));

    break;
  }

  case MessageTypes::GetI2CAddressId:
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetI2CAddressId;
    msg->header.body_size = sizeof(I2CAddressMessage::value);
    msg->value = FlashStorage::GetI2CSettings()->address;
    msg->footer.checksum = 0;
    // DEBUG_PRINTF("Sending I2C Address: 0x%x\n", msg->value);
    SendMsg(send_buffer, sizeof(I2CAddressMessage));
    break;
  }

  case MessageTypes::SetEthernetAddressId:
  {
    EthernetAddressMessage *msg = (EthernetAddressMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->ip_address = msg->value;
    
    //Respond with Ack
    AckMessage *ack_msg = (AckMessage *)&send_buffer[0];
    ack_msg->header.message_type = (uint16_t)MessageTypes::AckId;
    ack_msg->header.body_size = sizeof(AckMessage::ack_message_type) + sizeof(AckMessage::status);
    ack_msg->ack_message_type = (uint16_t)MessageTypes::SetEthernetAddressId;
    ack_msg->status = (uint8_t)StatusCodes::SUCCESS;
    ack_msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(AckMessage));
    break;
  }

  case MessageTypes::GetEthernetAddressId:
  {
    EthernetAddressMessage *msg = (EthernetAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetEthernetAddressId;
    msg->header.body_size = sizeof(EthernetAddressMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->ip_address;
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(EthernetAddressMessage));
    // DEBUG_PRINTF("Sending IP Address: 0x%x\n", sizeof(IPAddressMessage));
    break;
  }

  case MessageTypes::SetLedColorId:
  {
    LedColorMessage *msg = (LedColorMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting LED color                                           to: R=%d, G=%d, B=%d\n", msg->ledColor.r, msg->ledColor.g, msg->ledColor.b);
    addrLedController.SetLEDColor(CRGB(msg->ledColor[0], msg->ledColor[1], msg->ledColor[2]));
    break;
  }

  case MessageTypes::GetLedColorId:
  {
    LedColorMessage *msg = (LedColorMessage *)recv_bytes;
    msg->header.message_type = (uint16_t)MessageTypes::GetLedColorId;
    msg->header.body_size = sizeof(LedColorMessage::ledColor);
    msg->ledColor[0] = addrLedController.GetLedColor().r;
    msg->ledColor[1] = addrLedController.GetLedColor().g;
    msg->ledColor[2] = addrLedController.GetLedColor().b;
    msg->footer.checksum = 0;
    SendMsg(recv_bytes, sizeof(LedColorMessage));
    break;
  }

  case MessageTypes::SetEthernetPortId:
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->port = (uint16_t)(msg->value);
    // DEBUG_PRINTF("Setting Port: 0x%x\n", FlashStorage::GetEthernetSettings()->port);
    break;
  }

  case MessageTypes::GetEthernetPortId:
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetEthernetPortId;
    msg->header.body_size = sizeof(EthernetPortMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->port;
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(EthernetPortMessage));
    DEBUG_PRINTF("Sending Port: %d\n", msg->value);
    // DEBUG_PRINTF("Port msg size: %d\n", sizeof(EthernetPortMessage));
    break;
  }

  case MessageTypes::GetMacAddressId:
  {
    MacAddressMessage *msg = (MacAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMacAddressId;
    msg->header.body_size = sizeof(MacAddressMessage::mac);
    memcpy(&(msg->mac[0]), FlashStorage::GetMacAddress(), 6);
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(MacAddressMessage));
    break;
  }

  case MessageTypes::SaveConfigurationId:
  {
    FlashStorage::WriteFlash();
    // DEBUG_PRINTLN("Writing flash");
    break;
  }

  case MessageTypes::SetMotorStateId:
  {
    MotorStateMessage *msg = (MotorStateMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor State to: %d\n", msg->value);
    motorController.SetMotorState((MotorStates)msg->value);
    break;
  }

  case MessageTypes::GetMotorStateId:
  {
    MotorStateMessage *msg = (MotorStateMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMotorStateId;
    msg->header.body_size = sizeof(MotorStateMessage::value);
    msg->value = (uint8_t)motorController.GetMotorState();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(MotorStateMessage));
    // DEBUG_PRINTF("Sending Motor State: %d\n", (uint8_t)motorController.GetMotorState());
    break;
  }

  case MessageTypes::SetMotorBrakeId:
  {
    MotorBrakeMessage *msg = (MotorBrakeMessage *)&recv_bytes[0];
    motorController.SetMotorBraking((MotorBrake)msg->value);
    break;
  }

  case MessageTypes::SetMaxSpeedId:
  {
    MaxSpeedMessage *msg = (MaxSpeedMessage *)&recv_bytes[0];
    motorController.SetMaxSpeed(msg->value);
    break;
  }

  case MessageTypes::GetMaxSpeedId:
  {
    MaxSpeedMessage *msg = (MaxSpeedMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMaxSpeedId;
    msg->header.body_size = sizeof(MaxSpeedMessage::value);
    msg->value = motorController.GetMaxSpeed();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(CurrentPositionMessage));
    break;
  }

  case MessageTypes::SetAccelerationId:
  {
    AccelerationMessage *msg = (AccelerationMessage *)&recv_bytes[0];
    motorController.SetAcceleration(msg->value);
    break;
  }

  case MessageTypes::GetAccelerationId:
  {
    AccelerationMessage *msg = (AccelerationMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetAccelerationId;
    msg->header.body_size = sizeof(AccelerationMessage::value);
    msg->value = motorController.GetAcceleration();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(AccelerationMessage));
    break;
  }

  case MessageTypes::SetCurrentPositionId:
  {
    CurrentPositionMessage *msg = (CurrentPositionMessage *)&recv_bytes[0];
    motorController.SetPosition(msg->value);
    break;
  }

  case MessageTypes::GetCurrentPositionId:
  {
    DEBUG_PRINTLN("Getting Current Position");
    CurrentPositionMessage *msg = (CurrentPositionMessage *)send_buffer;
    msg->header.message_type = (uint16_t)MessageTypes::GetCurrentPositionId;
    msg->header.body_size = sizeof(CurrentPositionMessage::value);
    msg->value = motorController.GetPosition();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(CurrentPositionMessage));
    break;
  }

  case MessageTypes::SetTargetPositionId:
  {
    TargetPositionMessage *msg = (TargetPositionMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor Position to: %f\n", msg->value);
    motorController.SetPositionTarget(msg->value);
    break;
  }

    // case MessageTypes::GetTargetPositionId:
    // {
    //   PositionMessage *msg = (GetPositionMessage *)&send_buffer[0];
    //   msg->header.message_type = (uint16_t)MessageTypes::GetTargetPositionId;
    //   msg->header.body_size = sizeof(GetPositionMessage::value);
    //   msg->value = motorController.GetPositionTarget();
    //   msg->footer.checksum = 0;
    //   SendMsg(send_buffer, sizeof(GetPositionMessage));
    //   // DEBUG_PRINTF("Sending Motor Position: %f\n", motorController.GetPositionTarget());
    //   break;
    // }

  case MessageTypes::SetRelativeTargetPositionId:
  {
    RelativeTargetPositionMessage *msg = (RelativeTargetPositionMessage *)recv_bytes;
    motorController.SetPositionTargetRelative(msg->value);
    // DEBUG_PRINTF("Setting Motor Position Relative: %f\n", msg->value);
    break;
  }

  case MessageTypes::SetVelocityId:
  {
    VelocityMessage *msg = (VelocityMessage *)recv_bytes;
    motorController.SetVelocityTarget(msg->value);
    // DEBUG_PRINTF("Setting Motor Velocity to: %d\n", msg->value);
    break;
  }

  case MessageTypes::GetVelocityId:
  {
    VelocityMessage *msg = (VelocityMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetVelocityId;
    msg->header.body_size = sizeof(VelocityMessage::value);
    msg->value = motorController.GetVelocityTarget();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(VelocityMessage));
    break;
  }

  case MessageTypes::SetHomeDirectionId:
  {
    HomeDirectionMessage *msg = (HomeDirectionMessage *)recv_bytes;
    motorController.SetHomeDirection((HomeDirection)msg->value);
    break;
  }

  case MessageTypes::GetHomeDirectionId:
  {
    HomeDirectionMessage *msg = (HomeDirectionMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeDirectionId;
    msg->header.body_size = sizeof(HomeDirectionMessage::value);
    msg->value = (uint8_t)motorController.GetHomeDirection();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(HomeDirectionMessage));
    break;
  }

  case MessageTypes::SetHomeSpeedId:
  {
    HomeSpeedMessage *msg = (HomeSpeedMessage *)&recv_bytes[0];
    motorController.SetHomingSpeed(msg->value);
    // DEBUG_PRINTF("Setting Home Speed to: %d\n", msg->value);
    break;
  }

  case MessageTypes::GetHomeSpeedId:
  {
    HomeSpeedMessage *msg = (HomeSpeedMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeSpeedId;
    msg->header.body_size = sizeof(HomeSpeedMessage::value);
    msg->value = motorController.GetHomingSpeed();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(HomeSpeedMessage));
    break;
  }

  case MessageTypes::SetHomeThresholdId:
  {
    HomeThresholdMessage *msg = (HomeThresholdMessage *)&recv_bytes[0];
    motorController.SetHomeThreshold(msg->value);
    // DEBUG_PRINTF("Setting Home Threshold to: %d\n", msg->value);
    break;
  }

  case MessageTypes::GetHomeThresholdId:
  {
    HomeThresholdMessage *msg = (HomeThresholdMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeThresholdId;
    msg->header.body_size = sizeof(HomeThresholdMessage::value);
    msg->value = motorController.GetHomeThreshold();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(HomeThresholdMessage));
    break;
  }

  case MessageTypes::GetHomedStateId:
  {
    HomedStateMessage *msg = (HomedStateMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomedStateId;
    msg->header.body_size = sizeof(HomedStateMessage::value);
    msg->value = (uint8_t)motorController.GetHomeState();
    msg->footer.checksum = 0;
    SendMsg(send_buffer, sizeof(HomedStateMessage));
    DEBUG_PRINTF("Sending Homed State: %d\n", msg->value);
    break;
  }

  case MessageTypes::HomeId:
    motorController.Home();
    break;

  case MessageTypes::SetVelocityAndStepsId:
  {
    VelocityAndStepsMessage *msg = (VelocityAndStepsMessage *)&recv_bytes[0];
    motorController.AddVelocityStep(msg->velocity, msg->steps, msg->positionMode);
    // DEBUG_PRINTF("Velocity: %d, Steps: %d, Position Mode: %d\n", msg->velocity, msg->steps, msg->positionMode);
    break;
  }

  case MessageTypes::StartPathId:
    motorController.StartPath();
    break;

  default:
    DEBUG_PRINTF("Unable to handle message type: 0x%x", hdr->message_type);
    break;
  }
}

void MessageProcessor::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size, IExternalInterface *calling_interface)
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
  if (last_interface_ != nullptr)
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