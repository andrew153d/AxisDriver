#include "MessageProcessor.hpp"
#include "DebugPrinter.h"
#include "LedController/LedController.h"
#include "FlashStorage/FlashStorage.h"
#include "MotorController/MotorController.h"
#include "Ethernet.h"
#include "AxisMessages_common.pb.h"

uint16_t calculateChecksum(uint8_t *data, uint32_t size)
{
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        checksum += data[i];
        //DEBUG_PRINTF("Adding byte %u: %02x, sum: %u\n", i, data[i], checksum);
    }
    return checksum % 0xFFFF;
}

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



  // Parse FlatBuffer header from bytes
  // flatbuffers::FlatBufferBuilder builder(1024);
  // auto flatbuf_hdr = flatbuffers::GetRoot<AxisDriver::Header>(recv_bytes);
  // if (flatbuf_hdr->sync_bytes() != SYNC_BYTES)
  // {
  //   DEBUG_PRINTLN("Invalid sync bytes in message");
  //   return;
  // }

  // TODO: check for checksum

  //DEBUG_PRINTF("Received message type: 0x%x\n", hdr->message_type);

  // switch ((AxisDriver::MessageId)flatbuf_hdr->message_type())
  // {
  //   case AxisDriver::MessageId::MessageId_SetEthernetPortId:
  //   {
  //     AxisDriver::EthernetPortMessage *msg = (AxisDriver::EthernetPortMessage *)(recv_bytes+sizeof(AxisDriver::Header));
  //     FlashStorage::GetEthernetSettings()->port = msg->value();
  //     DEBUG_PRINTF("Setting Ethernet Port: %d\n", FlashStorage::GetEthernetSettings()->port);
  //     break;
  //   }

  //   /*
  // case MessageTypes::SetLedColorId:
  // {
  //   LedColorMessage *msg = (LedColorMessage *)recv_bytes;
  //   // DEBUG_PRINTF("Setting LED color to: R=%d, G=%d, B=%d\n", msg->ledColor.r, msg->ledColor.g, msg->ledColor.b);
  //   addrLedController.SetLEDColor(CRGB(msg->ledColor[0], msg->ledColor[1], msg->ledColor[2]));
  //   break;
  // }

  // case MessageTypes::GetLedColorId:
  // {
  //   LedColorMessage *msg = (LedColorMessage *)recv_bytes;
  //   msg->header.message_type = (uint16_t)MessageTypes::GetLedColorId;
  //   msg->header.body_size = sizeof(LedColorMessage::ledColor);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->ledColor[0] = addrLedController.GetLedColor().r;
  //   msg->ledColor[1] = addrLedController.GetLedColor().g;
  //   msg->ledColor[2] = addrLedController.GetLedColor().b;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(LedColorMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(recv_bytes, sizeof(LedColorMessage));
  //   break;
  // }

  // case MessageTypes::GetVersionId:
  // {
  //   VersionMessage *msg = (VersionMessage *)&send_buffer[0];
    
  //   msg->header.message_type = (uint16_t)MessageTypes::GetVersionId;
  //   msg->header.body_size = sizeof(VersionMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   uint8_t *v_buf = (uint8_t *)&msg->value;
  //   sscanf(FIRMWARE_VERSION, "%u.%u.%u.%u", &v_buf[0], &v_buf[1], &v_buf[2], &v_buf[3]);
  //   // DEBUG_PRINTF("0x%8X\n", msg->value);
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(VersionMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(VersionMessage));
  //   break;
  // }

  // case MessageTypes::SetI2CAddressId:
  // {
  //   I2CAddressMessage *msg = (I2CAddressMessage *)recv_bytes;
  //   FlashStorage::GetI2CSettings()->address = msg->value;
  //   break;
  // }

  // case MessageTypes::GetI2CAddressId:
  // {
  //   I2CAddressMessage *msg = (I2CAddressMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetI2CAddressId;
  //   msg->header.body_size = sizeof(I2CAddressMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetI2CSettings()->address;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(I2CAddressMessage) - sizeof(msg->footer.checksum));
  //   // DEBUG_PRINTF("Sending I2C Address: 0x%x\n", msg->value);
  //   SendMsg(send_buffer, sizeof(I2CAddressMessage));
  //   break;
  // }

  // case MessageTypes::SetEthernetAddressId:
  // {
  //   EthernetAddressMessage *msg = (EthernetAddressMessage *)recv_bytes;
  //   FlashStorage::GetEthernetSettings()->ip_address = msg->value;
  //   // DEBUG_PRINT("Setting IP Address: ");
  //   // DEBUG_PRINTLN(IPAddress(msg->value));
  //   break;
  // }

  // case MessageTypes::GetEthernetAddressId:
  // {
  //   EthernetAddressMessage *msg = (EthernetAddressMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetEthernetAddressId;
  //   msg->header.body_size = sizeof(EthernetAddressMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetEthernetSettings()->ip_address;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(EthernetAddressMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(EthernetAddressMessage));
  //   // DEBUG_PRINTF("Sending IP Address: 0x%x\n", sizeof(IPAddressMessage));
  //   break;
  // }

  // case MessageTypes::SetEthernetPortId:
  // {
  //   EthernetPortMessage *msg = (EthernetPortMessage *)recv_bytes;
  //   FlashStorage::GetEthernetSettings()->port = (uint16_t)(msg->value);
  //   // DEBUG_PRINTF("Setting Port: 0x%x\n", FlashStorage::GetEthernetSettings()->port);
  //   break;
  // }

  // case MessageTypes::GetEthernetPortId:
  // {
  //   EthernetPortMessage *msg = (EthernetPortMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetEthernetPortId;
  //   msg->header.body_size = sizeof(EthernetPortMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetEthernetSettings()->port;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(EthernetPortMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(EthernetPortMessage));
  //   //DEBUG_PRINTF("Sending Port: %d\n", msg->value);
  //   // DEBUG_PRINTF("Port msg size: %d\n", sizeof(EthernetPortMessage));
  //   break;
  // }

  // case MessageTypes::GetMacAddressId:
  // {
  //   MacAddressMessage *msg = (MacAddressMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetMacAddressId;
  //   msg->header.body_size = sizeof(MacAddressMessage::mac);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   memcpy(&(msg->mac[0]), FlashStorage::GetMacAddress(), 6);
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(MacAddressMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(MacAddressMessage));
  //   break;
  // }

  // case MessageTypes::SaveConfigurationId:
  // {
  //   FlashStorage::WriteFlash();
  //   // DEBUG_PRINTLN("Writing flash");
  //   break;
  // }

  // case MessageTypes::SetMotorStateId:
  // {
  //   MotorStateMessage *msg = (MotorStateMessage *)recv_bytes;
  //   // DEBUG_PRINTF("Setting Motor State to: %d\n", msg->value);
  //   motorController.SetMotorState((MotorStates)msg->value);
  //   break;
  // }

  // case MessageTypes::GetMotorStateId:
  // {
  //   MotorStateMessage *msg = (MotorStateMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetMotorStateId;
  //   msg->header.body_size = sizeof(MotorStateMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = (uint8_t)motorController.GetMotorState();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(MotorStateMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(MotorStateMessage));
  //   // DEBUG_PRINTF("Sending Motor State: %d\n", (uint8_t)motorController.GetMotorState());
  //   break;
  // }

  // case MessageTypes::SetMotorBrakeId:
  // {
  //   MotorBrakeMessage *msg = (MotorBrakeMessage *)&recv_bytes[0];
  //   motorController.SetMotorBraking((MotorBrake)msg->value);
  //   break;
  // }

  // case MessageTypes::SetMaxSpeedId:
  // {
  //   MaxSpeedMessage *msg = (MaxSpeedMessage *)&recv_bytes[0];
  //   motorController.SetMaxSpeed(msg->value);
  //   break;
  // }

  // case MessageTypes::GetMaxSpeedId:
  // {
  //   MaxSpeedMessage *msg = (MaxSpeedMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetMaxSpeedId;
  //   msg->header.body_size = sizeof(MaxSpeedMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetMaxSpeed();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(MaxSpeedMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(CurrentPositionMessage));
  //   break;
  // }

  // case MessageTypes::SetAccelerationId:
  // {
  //   AccelerationMessage *msg = (AccelerationMessage *)&recv_bytes[0];
  //   motorController.SetAcceleration(msg->value);
  //   break;
  // }

  // case MessageTypes::GetAccelerationId:
  // {
  //   AccelerationMessage *msg = (AccelerationMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetAccelerationId;
  //   msg->header.body_size = sizeof(AccelerationMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetAcceleration();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(AccelerationMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(AccelerationMessage));
  //   break;
  // }

  // case MessageTypes::SetCurrentPositionId:
  // {
  //   CurrentPositionMessage *msg = (CurrentPositionMessage *)&recv_bytes[0];
  //   motorController.SetPosition(msg->value);
  //   break;
  // }

  // case MessageTypes::GetCurrentPositionId:
  // {
  //   DEBUG_PRINTLN("Getting Current Position");
  //   CurrentPositionMessage *msg = (CurrentPositionMessage*)send_buffer;
  //   msg->header.message_type = (uint16_t)MessageTypes::GetCurrentPositionId;
  //   msg->header.body_size = sizeof(CurrentPositionMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetPosition();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(CurrentPositionMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(CurrentPositionMessage));
  //   break;
  // }

  // case MessageTypes::SetTargetPositionId:
  // {
  //   TargetPositionMessage *msg = (TargetPositionMessage *)recv_bytes;
  //   //DEBUG_PRINTF("Setting Motor Position to: %f\n", msg->value);
  //   motorController.SetPositionTarget(msg->value);
  //   break;
  // }

  // // case MessageTypes::GetTargetPositionId:
  // // {
  // //   PositionMessage *msg = (GetPositionMessage *)&send_buffer[0];
  // //   msg->header.message_type = (uint16_t)MessageTypes::GetTargetPositionId;
  // //   msg->header.body_size = sizeof(GetPositionMessage::value);
  // //   msg->value = motorController.GetPositionTarget();
  // //   msg->footer.checksum = 0;
  // //   SendMsg(send_buffer, sizeof(GetPositionMessage));
  // //   // DEBUG_PRINTF("Sending Motor Position: %f\n", motorController.GetPositionTarget());
  // //   break;
  // // }

  // case MessageTypes::SetRelativeTargetPositionId:
  // {
  //   RelativeTargetPositionMessage *msg = (RelativeTargetPositionMessage *)recv_bytes;
  //   motorController.SetPositionTargetRelative(msg->value);
  //   //DEBUG_PRINTF("Setting Motor Position Relative: %f\n", msg->value);
  //   break;
  // }

  // case MessageTypes::SetVelocityId:
  // {
  //   VelocityMessage *msg = (VelocityMessage *)recv_bytes;
  //   motorController.SetVelocityTarget(msg->value);
  //   // DEBUG_PRINTF("Setting Motor Velocity to: %d\n", msg->value);
  //   break;
  // }

  // case MessageTypes::GetVelocityId:
  // {
  //   VelocityMessage *msg = (VelocityMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetVelocityId;
  //   msg->header.body_size = sizeof(VelocityMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetVelocityTarget();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(VelocityMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(VelocityMessage));
  //   break;
  // }

  // case MessageTypes::SetHomeDirectionId:
  // {
  //   HomeDirectionMessage *msg = (HomeDirectionMessage *)recv_bytes;
  //   motorController.SetHomeDirection((HomeDirection)msg->value);
  //   break;
  // }

  // case MessageTypes::GetHomeDirectionId:
  // {
  //   HomeDirectionMessage *msg = (HomeDirectionMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHomeDirectionId;
  //   msg->header.body_size = sizeof(HomeDirectionMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = (uint8_t)motorController.GetHomeDirection();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HomeDirectionMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HomeDirectionMessage));
  //   break;
  // }

  // case MessageTypes::SetHomeSpeedId:
  // {
  //   HomeSpeedMessage *msg = (HomeSpeedMessage *)&recv_bytes[0];
  //   motorController.SetHomingSpeed(msg->value);
  //   // DEBUG_PRINTF("Setting Home Speed to: %d\n", msg->value);
  //   break;
  // }

  // case MessageTypes::GetHomeSpeedId:
  // {
  //   HomeSpeedMessage *msg = (HomeSpeedMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHomeSpeedId;
  //   msg->header.body_size = sizeof(HomeSpeedMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetHomingSpeed();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HomeSpeedMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HomeSpeedMessage));
  //   break;
  // }

  // case MessageTypes::SetHomeThresholdId:
  // {
  //   HomeThresholdMessage *msg = (HomeThresholdMessage *)&recv_bytes[0];
  //   motorController.SetHomeThreshold(msg->value);
  //   // DEBUG_PRINTF("Setting Home Threshold to: %d\n", msg->value);
  //   break;
  // }

  // case MessageTypes::GetHomeThresholdId:
  // {
  //   HomeThresholdMessage *msg = (HomeThresholdMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHomeThresholdId;
  //   msg->header.body_size = sizeof(HomeThresholdMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = motorController.GetHomeThreshold();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HomeThresholdMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HomeThresholdMessage));
  //   break;
  // }

  // case MessageTypes::GetHomedStateId:
  // {
  //   HomedStateMessage *msg = (HomedStateMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHomedStateId;
  //   msg->header.body_size = sizeof(HomedStateMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = (uint8_t)motorController.GetHomeState();
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HomedStateMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HomedStateMessage));
  //   DEBUG_PRINTF("Sending Homed State: %d\n", msg->value);
  //   break;
  // }

  // case MessageTypes::HomeId:
  //   motorController.Home();
  //   break;

  // case MessageTypes::SetVelocityAndStepsId:
  // {
  //   VelocityAndStepsMessage *msg = (VelocityAndStepsMessage *)&recv_bytes[0];
  //   motorController.AddVelocityStep(msg->velocity, msg->steps, msg->positionMode);
  //   //DEBUG_PRINTF("Velocity: %d, Steps: %d, Position Mode: %d\n", msg->velocity, msg->steps, msg->positionMode);
  //   break;
  // }

  // case MessageTypes::StartPathId:
  //   motorController.StartPath();
  //   break;
  // case MessageTypes::SetHAEnableId:
  // {
  //   HAEnableMessage *msg = (HAEnableMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->enable = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAEnableId:
  // {
  //   HAEnableMessage *msg = (HAEnableMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAEnableId;
  //   msg->header.body_size = sizeof(HAEnableMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->enable;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAEnableMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAEnableMessage));
  //   break;
  // }

  // case MessageTypes::SetHAModeId:
  // {
  //   HAModeMessage *msg = (HAModeMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->mode = (HAMode)msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAModeId:
  // {
  //   HAModeMessage *msg = (HAModeMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAModeId;
  //   msg->header.body_size = sizeof(HAModeMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = (uint8_t)FlashStorage::GetHASettings()->mode;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAModeMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAModeMessage));
  //   break;
  // }

  // case MessageTypes::SetHAIpAddressId:
  // {
  //   HAIpAddressMessage *msg = (HAIpAddressMessage *)recv_bytes;
  //   memcpy(FlashStorage::GetHASettings()->ha_ip_address, msg->ha_ip_address, 4);
  //   break;
  // }

  // case MessageTypes::GetHAIpAddressId:
  // {
  //   HAIpAddressMessage *msg = (HAIpAddressMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAIpAddressId;
  //   msg->header.body_size = sizeof(HAIpAddressMessage::ha_ip_address);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   memcpy(msg->ha_ip_address, FlashStorage::GetHASettings()->ha_ip_address, 4);
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAIpAddressMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAIpAddressMessage));
  //   break;
  // }

  // case MessageTypes::SetHAVelocitySwitchOnSpeedId:
  // {
  //   HAVelocitySwitchOnSpeedMessage *msg = (HAVelocitySwitchOnSpeedMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->velocity_switch_on_speed = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAVelocitySwitchOnSpeedId:
  // {
  //   HAVelocitySwitchOnSpeedMessage *msg = (HAVelocitySwitchOnSpeedMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAVelocitySwitchOnSpeedId;
  //   msg->header.body_size = sizeof(HAVelocitySwitchOnSpeedMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->velocity_switch_on_speed;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAVelocitySwitchOnSpeedMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAVelocitySwitchOnSpeedMessage));
  //   break;
  // }

  // case MessageTypes::SetHAVelocitySwitchOffSpeedId:
  // {
  //   HAVelocitySwitchOffSpeedMessage *msg = (HAVelocitySwitchOffSpeedMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->velocity_switch_off_speed = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAVelocitySwitchOffSpeedId:
  // {
  //   HAVelocitySwitchOffSpeedMessage *msg = (HAVelocitySwitchOffSpeedMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAVelocitySwitchOffSpeedId;
  //   msg->header.body_size = sizeof(HAVelocitySwitchOffSpeedMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->velocity_switch_off_speed;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAVelocitySwitchOffSpeedMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAVelocitySwitchOffSpeedMessage));
  //   break;
  // }

  // case MessageTypes::SetHAPositionSwitchOnPositionId:
  // {
  //   HAPositionSwitchOnPositionMessage *msg = (HAPositionSwitchOnPositionMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->position_switch_on_position = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAPositionSwitchOnPositionId:
  // {
  //   HAPositionSwitchOnPositionMessage *msg = (HAPositionSwitchOnPositionMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAPositionSwitchOnPositionId;
  //   msg->header.body_size = sizeof(HAPositionSwitchOnPositionMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->position_switch_on_position;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAPositionSwitchOnPositionMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAPositionSwitchOnPositionMessage));
  //   break;
  // }

  // case MessageTypes::SetHAPositionSwitchOffPositionId:
  // {
  //   HAPositionSwitchOffPositionMessage *msg = (HAPositionSwitchOffPositionMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->position_switch_off_position = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAPositionSwitchOffPositionId:
  // {
  //   HAPositionSwitchOffPositionMessage *msg = (HAPositionSwitchOffPositionMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAPositionSwitchOffPositionId;
  //   msg->header.body_size = sizeof(HAPositionSwitchOffPositionMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->position_switch_off_position;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAPositionSwitchOffPositionMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAPositionSwitchOffPositionMessage));
  //   break;
  // }

  // case MessageTypes::SetHAVelocitySliderMinId:
  // {
  //   HAVelocitySliderMinMessage *msg = (HAVelocitySliderMinMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->velocity_slider_min = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAVelocitySliderMinId:
  // {
  //   HAVelocitySliderMinMessage *msg = (HAVelocitySliderMinMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAVelocitySliderMinId;
  //   msg->header.body_size = sizeof(HAVelocitySliderMinMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->velocity_slider_min;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAVelocitySliderMinMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAVelocitySliderMinMessage));
  //   break;
  // }

  // case MessageTypes::SetHAVelocitySliderMaxId:
  // {
  //   HAVelocitySliderMaxMessage *msg = (HAVelocitySliderMaxMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->velocity_slider_max = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAVelocitySliderMaxId:
  // {
  //   HAVelocitySliderMaxMessage *msg = (HAVelocitySliderMaxMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAVelocitySliderMaxId;
  //   msg->header.body_size = sizeof(HAVelocitySliderMaxMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->velocity_slider_max;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAVelocitySliderMaxMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAVelocitySliderMaxMessage));
  //   break;
  // }

  // case MessageTypes::SetHAPositionSliderMinId:
  // {
  //   HAPositionSliderMinMessage *msg = (HAPositionSliderMinMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->position_slider_min = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAPositionSliderMinId:
  // {
  //   HAPositionSliderMinMessage *msg = (HAPositionSliderMinMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAPositionSliderMinId;
  //   msg->header.body_size = sizeof(HAPositionSliderMinMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->position_slider_min;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAPositionSliderMinMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAPositionSliderMinMessage));
  //   break;
  // }

  // case MessageTypes::SetHAPositionSliderMaxId:
  // {
  //   HAPositionSliderMaxMessage *msg = (HAPositionSliderMaxMessage *)recv_bytes;
  //   FlashStorage::GetHASettings()->position_slider_max = msg->value;
  //   break;
  // }

  // case MessageTypes::GetHAPositionSliderMaxId:
  // {
  //   HAPositionSliderMaxMessage *msg = (HAPositionSliderMaxMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAPositionSliderMaxId;
  //   msg->header.body_size = sizeof(HAPositionSliderMaxMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   msg->value = FlashStorage::GetHASettings()->position_slider_max;
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAPositionSliderMaxMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAPositionSliderMaxMessage));
  //   break;
  // }

  // case MessageTypes::SetHAMqttUserId:
  // {
  //   HAMqttUserMessage *msg = (HAMqttUserMessage *)recv_bytes;
    
  //   strncpy(FlashStorage::GetHASettings()->mqtt_user, (char*)msg->value, sizeof(FlashStorage::GetHASettings()->mqtt_user));
  //   break;
  // }

  // case MessageTypes::GetHAMqttUserId:
  // {
  //   HAMqttUserMessage *msg = (HAMqttUserMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAMqttUserId;
  //   msg->header.body_size = sizeof(HAMqttUserMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   strncpy((char*)msg->value, FlashStorage::GetHASettings()->mqtt_user, sizeof(msg->value));
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAMqttUserMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAMqttUserMessage));
  //   break;
  // }

  // case MessageTypes::SetHAMqttPasswordId:
  // {
  //   HAMqttPasswordMessage *msg = (HAMqttPasswordMessage *)recv_bytes;
  //   strncpy(FlashStorage::GetHASettings()->mqtt_password, (char*)msg->value, sizeof(FlashStorage::GetHASettings()->mqtt_password));
  //   break;
  // }

  // case MessageTypes::GetHAMqttPasswordId:
  // {
  //   HAMqttPasswordMessage *msg = (HAMqttPasswordMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAMqttPasswordId;
  //   msg->header.body_size = sizeof(HAMqttPasswordMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   strncpy((char*)msg->value, FlashStorage::GetHASettings()->mqtt_password, sizeof(msg->value));
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAMqttPasswordMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAMqttPasswordMessage));
  //   break;
  // }

  // case MessageTypes::SetHAMqttNameId:
  // {
  //   HAMqttNameMessage *msg = (HAMqttNameMessage *)recv_bytes;
  //   strncpy(FlashStorage::GetHASettings()->mqtt_name, (char*)msg->value, sizeof(FlashStorage::GetHASettings()->mqtt_name));
  //   break;
  // }

  // case MessageTypes::GetHAMqttNameId:
  // {
  //   HAMqttNameMessage *msg = (HAMqttNameMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAMqttNameId;
  //   msg->header.body_size = sizeof(HAMqttNameMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   strncpy((char*)msg->value, FlashStorage::GetHASettings()->mqtt_name, sizeof(msg->value));
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAMqttNameMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAMqttNameMessage));
  //   break;
  // }

  // case MessageTypes::SetHAMqttIconId:
  // {
  //   HAMqttIconMessage *msg = (HAMqttIconMessage *)recv_bytes;
  //   strncpy(FlashStorage::GetHASettings()->mqtt_icon, (char*)msg->value, sizeof(FlashStorage::GetHASettings()->mqtt_icon));
  //   break;
  // }

  // case MessageTypes::GetHAMqttIconId:
  // {
  //   HAMqttIconMessage *msg = (HAMqttIconMessage *)&send_buffer[0];
  //   msg->header.message_type = (uint16_t)MessageTypes::GetHAMqttIconId;
  //   msg->header.body_size = sizeof(HAMqttIconMessage::value);
  //   msg->header.sync_bytes = SYNC_BYTES;
  //   strncpy((char*)msg->value, FlashStorage::GetHASettings()->mqtt_icon, sizeof(msg->value));
  //   msg->footer.checksum = calculateChecksum((uint8_t *)&msg->header, sizeof(HAMqttIconMessage) - sizeof(msg->footer.checksum));
  //   SendMsg(send_buffer, sizeof(HAMqttIconMessage));
  //   break;
  // }
  //   */
  // default:
  //   DEBUG_PRINTF("Unable to handle message type: 0x%x", flatbuf_hdr->message_type());
  //   break;
  // }
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
    //DEBUG_PRINTF("Sending message with checksum 0x%x\n", calculateChecksum(send_bytes, send_bytes_size-FOOTER_SIZE));
    last_interface_->SendMsg(send_bytes, send_bytes_size);
    return;
  }

  for (auto i : externalInterfaces)
  {
    i->SendMsg(send_bytes, send_bytes_size);
  }
  memset(send_bytes, 0, send_bytes_size);
}