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

  addrLedController.AddLedStep(CRGB::Green, 10);
  addrLedController.AddLedStep(CRGB::Black, 1);

  // TODO: check for checksum
  DEBUG_PRINTF("Received %d bytes\n", recv_bytes_size);
  // DEBUG_PRINTF("Received message type: 0x%x\n", hdr->message_type);
  switch ((MessageTypes)hdr->message_type)
  {
  case MessageTypes::AckId: // 0x0100
  {
    // Note: ACK messages are typically not received but sent in response to other messages
    // This case is included for completeness but may not be used
    break;
  }

  case MessageTypes::GetVersionId: // 0x0101
  {
    VersionMessage *msg = (VersionMessage *)&send_buffer[0];

    msg->header.message_type = (uint16_t)MessageTypes::GetVersionId;
    msg->header.body_size = sizeof(VersionMessage::value);
    unsigned int v0, v1, v2, v3;
    sscanf(FIRMWARE_VERSION, "%u.%u.%u.%u", &v0, &v1, &v2, &v3);
    uint8_t *v_buf = (uint8_t *)&msg->value;
    v_buf[0] = (uint8_t)v0;
    v_buf[1] = (uint8_t)v1;
    v_buf[2] = (uint8_t)v2;
    v_buf[3] = (uint8_t)v3;
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(VersionMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(VersionMessage));
    DEBUG_PRINTF("0x%8X\n", msg->value);
    break;
  }

  case MessageTypes::SetI2CAddressId: // 0x0102
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)recv_bytes;
    FlashStorage::GetI2CSettings()->address = msg->value;

    SendAck(MessageTypes::SetI2CAddressId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetI2CAddressId: // 0x0103
  {
    I2CAddressMessage *msg = (I2CAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetI2CAddressId;
    msg->header.body_size = sizeof(I2CAddressMessage::value);
    msg->value = FlashStorage::GetI2CSettings()->address;
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(I2CAddressMessage) - sizeof(Footer));
    // DEBUG_PRINTF("Sending I2C Address: 0x%x\n", msg->value);
    SendMsg(send_buffer, sizeof(I2CAddressMessage));
    break;
  }

  case MessageTypes::SetEthernetAddressId: // 0x0104
  {
    EthernetAddressMessage *msg = (EthernetAddressMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->ip_address = msg->value;

    // Respond with Ack
    SendAck(MessageTypes::SetEthernetAddressId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetEthernetAddressId: // 0x0105
  {
    EthernetAddressMessage *msg = (EthernetAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetEthernetAddressId;
    msg->header.body_size = sizeof(EthernetAddressMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->ip_address;
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(EthernetAddressMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(EthernetAddressMessage));
    // DEBUG_PRINTF("Sending IP Address: 0x%x\n", sizeof(IPAddressMessage));
    break;
  }

  case MessageTypes::SetEthernetPortId: // 0x0106
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)recv_bytes;
    FlashStorage::GetEthernetSettings()->port = (uint16_t)(msg->value);

    // Respond with Ack
    SendAck(MessageTypes::SetEthernetPortId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetEthernetPortId: // 0x0107
  {
    EthernetPortMessage *msg = (EthernetPortMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetEthernetPortId;
    msg->header.body_size = sizeof(EthernetPortMessage::value);
    msg->value = FlashStorage::GetEthernetSettings()->port;
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(EthernetPortMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(EthernetPortMessage));
    DEBUG_PRINTF("Sending Port: %d\n", msg->value);
    // DEBUG_PRINTF("Port msg size: %d\n", sizeof(EthernetPortMessage));
    break;
  }

  case MessageTypes::GetMacAddressId: // 0x0108
  {
    MacAddressMessage *msg = (MacAddressMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMacAddressId;
    msg->header.body_size = sizeof(MacAddressMessage::mac);
    memcpy(&(msg->mac[0]), FlashStorage::GetMacAddress(), 6);
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(MacAddressMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(MacAddressMessage));
    break;
  }

  case MessageTypes::SaveConfigurationId: // 0x0109
  {
    FlashStorage::WriteFlash();
    SendAck(MessageTypes::SaveConfigurationId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::SetLedColorId: // 0x010A
  {
    LedColorMessage *msg = (LedColorMessage *)recv_bytes;
    //DEBUG_PRINTF("Setting LED Color: %d, %d, %d\n", msg->ledColor[0], msg->ledColor[1], msg->ledColor[2]);
    addrLedController.SetLEDColor(CRGB(msg->ledColor[0], msg->ledColor[1], msg->ledColor[2]));
    
    AckMessage *ack_msg = (AckMessage *)&send_buffer[0];
    ack_msg->header.message_type = (uint16_t)MessageTypes::AckId;
    ack_msg->header.body_size = sizeof(AckMessage::ack_message_type) + sizeof(AckMessage::status);
    ack_msg->ack_message_type = (uint16_t)MessageTypes::SetLedColorId;
    ack_msg->status = (uint8_t)StatusCodes::SUCCESS;
    ack_msg->footer.checksum = CalculateChecksum((uint8_t *)ack_msg, sizeof(AckMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(AckMessage));

    break;
  }

  case MessageTypes::GetLedColorId: // 0x010B
  {
    CRGB led_color = addrLedController.GetLedColor();
    LedColorMessage *msg = (LedColorMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetLedColorId;
    msg->header.body_size = sizeof(LedColorMessage::ledColor);
    msg->ledColor[0] = led_color.r;
    msg->ledColor[1] = led_color.g;
    msg->ledColor[2] = led_color.b;
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(LedColorMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(LedColorMessage));
    break;
  }

  case MessageTypes::AddLedStepId: // Add LED Step
  {
    AddLedStepMessage *msg = (AddLedStepMessage *)recv_bytes;
    addrLedController.AddLedStep(CRGB(msg->ledColor[0], msg->ledColor[1], msg->ledColor[2]), msg->time_ms);
    SendAck(MessageTypes::AddLedStepId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::SetHomeDirectionId: // 0x010E
  {
    HomeDirectionMessage *msg = (HomeDirectionMessage *)recv_bytes;
    motorController.SetHomeDirection((HomeDirection)msg->value);
    SendAck(MessageTypes::SetHomeDirectionId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetHomeDirectionId: // 0x010F
  {
    HomeDirectionMessage *msg = (HomeDirectionMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeDirectionId;
    msg->header.body_size = sizeof(HomeDirectionMessage::value);
    msg->value = (uint8_t)motorController.GetHomeDirection();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(HomeDirectionMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(HomeDirectionMessage));
    break;
  }

  case MessageTypes::SetHomeThresholdId: // 0x0110
  {
    HomeThresholdMessage *msg = (HomeThresholdMessage *)&recv_bytes[0];
    motorController.SetHomeThreshold(msg->value);
    // DEBUG_PRINTF("Setting Home Threshold to: %d\n", msg->value);
    SendAck(MessageTypes::SetHomeThresholdId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetHomeThresholdId: // 0x0111
  {
    HomeThresholdMessage *msg = (HomeThresholdMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeThresholdId;
    msg->header.body_size = sizeof(HomeThresholdMessage::value);
    msg->value = motorController.GetHomeThreshold();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(HomeThresholdMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(HomeThresholdMessage));
    break;
  }

  case MessageTypes::SetHomeSpeedId: // 0x0112
  {
    HomeSpeedMessage *msg = (HomeSpeedMessage *)&recv_bytes[0];
    motorController.SetHomingSpeed(msg->value);
    // DEBUG_PRINTF("Setting Home Speed to: %d\n", msg->value);
    SendAck(MessageTypes::SetHomeSpeedId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetHomeSpeedId: // 0x0113
  {
    HomeSpeedMessage *msg = (HomeSpeedMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomeSpeedId;
    msg->header.body_size = sizeof(HomeSpeedMessage::value);
    msg->value = motorController.GetHomingSpeed();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(HomeSpeedMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(HomeSpeedMessage));
    break;
  }

  case MessageTypes::GetHomedStateId: // 0x0114
  {
    HomedStateMessage *msg = (HomedStateMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetHomedStateId;
    msg->header.body_size = sizeof(HomedStateMessage::value);
    msg->value = (uint8_t)motorController.GetHomeState();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(HomedStateMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(HomedStateMessage));
    //DEBUG_PRINTF("Sending Homed State: %d\n", msg->value);

    break;
  }

  case MessageTypes::HomeId: // 0x0115
    motorController.Home();
    SendAck(MessageTypes::HomeId, StatusCodes::SUCCESS);
    break;

  case MessageTypes::SetMotorStateId: // 0x0116
  {
    MotorStateMessage *msg = (MotorStateMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor State to: %d\n", msg->value);
    motorController.SetMotorState((MotorStates)msg->value);
    SendAck(MessageTypes::SetMotorStateId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetMotorStateId: // 0x0117
  {
    MotorStateMessage *msg = (MotorStateMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMotorStateId;
    msg->header.body_size = sizeof(MotorStateMessage::value);
    msg->value = (uint8_t)motorController.GetMotorState();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(MotorStateMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(MotorStateMessage));
    // DEBUG_PRINTF("Sending Motor State: %d\n", (uint8_t)motorController.GetMotorState());
    break;
  }

  case MessageTypes::SetMotorBrakeId: // 0x0118
  {
    MotorBrakeMessage *msg = (MotorBrakeMessage *)&recv_bytes[0];
    motorController.SetMotorBraking((MotorBrake)msg->value);
    SendAck(MessageTypes::SetMotorBrakeId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetMotorBrakeId: // 0x0119
  {
    MotorBrakeMessage *msg = (MotorBrakeMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMotorBrakeId;
    msg->header.body_size = sizeof(MotorBrakeMessage::value);
    msg->value = (uint8_t)motorController.GetMotorBraking();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(MotorBrakeMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(MotorBrakeMessage));
    break;
  }

  case MessageTypes::SetMaxSpeedId: // 0x011A
  {
    MaxSpeedMessage *msg = (MaxSpeedMessage *)&recv_bytes[0];
    motorController.SetMaxSpeed(msg->value);
    SendAck(MessageTypes::SetMaxSpeedId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetMaxSpeedId: // 0x011B
  {
    MaxSpeedMessage *msg = (MaxSpeedMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetMaxSpeedId;
    msg->header.body_size = sizeof(MaxSpeedMessage::value);
    msg->value = motorController.GetMaxSpeed();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(MaxSpeedMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(MaxSpeedMessage));
    break;
  }

  case MessageTypes::SetAccelerationId: // 0x011C
  {
    AccelerationMessage *msg = (AccelerationMessage *)&recv_bytes[0];
    motorController.SetAcceleration(msg->value);
    SendAck(MessageTypes::SetAccelerationId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetAccelerationId: // 0x011D
  {
    AccelerationMessage *msg = (AccelerationMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetAccelerationId;
    msg->header.body_size = sizeof(AccelerationMessage::value);
    msg->value = motorController.GetAcceleration();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(AccelerationMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(AccelerationMessage));
    break;
  }

  case MessageTypes::SetCurrentPositionId: // 0x011E
  {
    CurrentPositionMessage *msg = (CurrentPositionMessage *)&recv_bytes[0];
    motorController.SetPosition(msg->value);
    SendAck(MessageTypes::SetCurrentPositionId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetCurrentPositionId: // 0x011F
  {
    //DEBUG_PRINTLN("Getting Current Position");
    CurrentPositionMessage *msg = (CurrentPositionMessage *)send_buffer;
    msg->header.message_type = (uint16_t)MessageTypes::GetCurrentPositionId;
    msg->header.body_size = sizeof(CurrentPositionMessage::value);
    msg->value = motorController.GetPosition();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(CurrentPositionMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(CurrentPositionMessage));
    break;
  }

  case MessageTypes::SetTargetPositionId: // 0x0120
  {
    TargetPositionMessage *msg = (TargetPositionMessage *)recv_bytes;
    // DEBUG_PRINTF("Setting Motor Position to: %f\n", msg->value);
    motorController.SetPositionTarget(msg->value);
    SendAck(MessageTypes::SetTargetPositionId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetTargetPositionId: // 0x0121
  {
    TargetPositionMessage *msg = (TargetPositionMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetTargetPositionId;
    msg->header.body_size = sizeof(TargetPositionMessage::value);
    msg->value = motorController.GetPositionTarget();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(TargetPositionMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(TargetPositionMessage));
    break;
  }

  case MessageTypes::SetRelativeTargetPositionId: // 0x0122
  {
    RelativeTargetPositionMessage *msg = (RelativeTargetPositionMessage *)recv_bytes;
    motorController.SetPositionTargetRelative(msg->value);
    // DEBUG_PRINTF("Setting Motor Position Relative: %f\n", msg->value);
    SendAck(MessageTypes::SetRelativeTargetPositionId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::SetVelocityId: // 0x0123
  {
    VelocityMessage *msg = (VelocityMessage *)recv_bytes;
    motorController.SetVelocityTarget(msg->value);
    // DEBUG_PRINTF("Setting Motor Velocity to: %d\n", msg->value);
    SendAck(MessageTypes::SetVelocityId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::GetVelocityId: // 0x0124
  {
    VelocityMessage *msg = (VelocityMessage *)&send_buffer[0];
    msg->header.message_type = (uint16_t)MessageTypes::GetVelocityId;
    msg->header.body_size = sizeof(VelocityMessage::value);
    msg->value = motorController.GetVelocityTarget();
    msg->footer.checksum = CalculateChecksum((uint8_t *)msg, sizeof(VelocityMessage) - sizeof(Footer));
    SendMsg(send_buffer, sizeof(VelocityMessage));
    break;
  }

  case MessageTypes::SetVelocityAndStepsId: // 0x0125
  {
    VelocityAndStepsMessage *msg = (VelocityAndStepsMessage *)&recv_bytes[0];
    motorController.AddVelocityStep(msg->velocity, msg->steps, msg->positionMode);
    // DEBUG_PRINTF("Velocity: %d, Steps: %d, Position Mode: %d\n", msg->velocity, msg->steps, msg->positionMode);
    SendAck(MessageTypes::SetVelocityAndStepsId, StatusCodes::SUCCESS);
    break;
  }

  case MessageTypes::StartPathId: // 0x0126
    motorController.StartPath();
    SendAck(MessageTypes::StartPathId, StatusCodes::SUCCESS);
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

  HandleByteMsg(recv_bytes, recv_bytes_size);

}

void MessageProcessor::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
  if (last_interface_ != nullptr)
  {
    addrLedController.AddLedStep(CRGB::Blue, 10);
    addrLedController.AddLedStep(CRGB::Black, 1);

    last_interface_->SendMsg(send_bytes, send_bytes_size);
    return;
  }

  for (auto i : externalInterfaces)
  {
    i->SendMsg(send_bytes, send_bytes_size);
  }
  memset(send_bytes, 0, send_bytes_size);
}

void MessageProcessor::SendAck(MessageTypes msg_type, StatusCodes status)
{
  AckMessage *ack_msg = (AckMessage *)&send_buffer[0];
  ack_msg->header.message_type = (uint16_t)MessageTypes::AckId;
  ack_msg->header.body_size = sizeof(AckMessage::ack_message_type) + sizeof(AckMessage::status);
  ack_msg->ack_message_type = (uint16_t)msg_type;
  ack_msg->status = (uint8_t)status;
  ack_msg->footer.checksum = CalculateChecksum((uint8_t *)ack_msg, sizeof(AckMessage) - sizeof(Footer));
  SendMsg(send_buffer, sizeof(AckMessage));
}

uint16_t MessageProcessor::CalculateChecksum(uint8_t *data, uint32_t size)
{
  uint16_t checksum = 0;
  for (uint32_t i = 0; i < size; ++i)
  {
    checksum += data[i];
  }
  return checksum;
}