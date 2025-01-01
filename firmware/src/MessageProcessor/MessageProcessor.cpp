#include "MessageProcessor.hpp"
#include "DebugPrinter.h"
#include "LedController/LedController.h"

MessageProcessor::MessageProcessor(uint32_t period)
{
executionPeriod = period;
}

  void MessageProcessor::AddExternalInterface(IExternalInterface *new_interface)
  {
    externalInterfaces.push_back(new_interface);
    new_interface->SetProcessorInterface(this);
  }

  void MessageProcessor::AddControllerInterface(IInternalInterface *new_interface, JsonMessageTypes message_type, MessageTypes low, MessageTypes high)
  {
    InterfaceLimits interface_to_add;
    interface_to_add.interface = new_interface;
    interface_to_add.type = message_type;
    interface_to_add.highLimit = (uint16_t)high;
    interface_to_add.lowLimit = (uint16_t)low;
    controllerInterfaces.push_back(interface_to_add);
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
    // DeserializationError error = deserializeJson(received_json, recv_bytes, 256);
    // if (error)
    // {
    //     Serial.println("Error parsing json in motor controller");
    //     return;
    // }

    // if (received_json.containsKey("mode"))
    // {
    //     String mode_string = received_json["mode"];

    //     if (mode_string == "Solid")
    //     {
    //     }
    // }
  }


  void MessageProcessor::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
  {
    Header *hdr = (Header *)&recv_bytes[0];

    //TODO: check for checksum

    DEBUG_PRINTF("Received message type: %x\n", hdr->message_type);
    
    switch(hdr->message_type)
    {
      case MessageTypes::SetLedColor:
      {
        SetLedColorMessage* msg = (SetLedColorMessage*)recv_bytes;
        DEBUG_PRINTF("Setting LED color to: R=%d, G=%d, B=%d\n", msg->ledColor.r, msg->ledColor.g, msg->ledColor.b);
        addrLedController.SetLEDColor(CRGB(msg->ledColor.r, msg->ledColor.g, msg->ledColor.b));
      break;
      }

      case MessageTypes::SetLedState:
      {
        SetLedStateMessage* msg = (SetLedStateMessage*)recv_bytes;
        DEBUG_PRINTF("Setting LED state to: %d\n", msg->ledState);
        addrLedController.SetLedState(msg->ledState);
      break;
      }
      default:
      DEBUG_PRINTLN("Unable to handle message type");
      break;
    }

  }

  void MessageProcessor::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
  {
    if (recv_bytes_size == 0)
      return;

    if (recv_bytes[0] == '{')
    {
      HandleJsonMsg(recv_bytes, recv_bytes_size);
      // JsonDocument doc;
      // DeserializationError error = deserializeJson(doc, recv_bytes, 256);
      // if (error)
      // {
      //   Serial.println("Error parsing json");
      //   return;
      // }
      // String type;
      // if(!TryParseJson("type", &type, doc)){
      //   Serial.println("Failed to find type");
      //   return;
      // }
      // for (InterfaceLimits interface : controllerInterfaces)
      // {
      //   if (interface.type == ToJsonMessageTypes(type))
      //   {
      //     interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
      //   }
      // }
    }
    else
    {
      HandleByteMsg(recv_bytes, recv_bytes_size);

      // Header *hdr = (Header *)&recv_bytes[0];

      // for (InterfaceLimits interface : controllerInterfaces)
      // {
      //   if ((uint16_t)hdr->message_type > interface.lowLimit && (uint16_t)hdr->message_type < interface.highLimit)
      //   {
      //     interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
      //   }
      // }
    }
  }

  void MessageProcessor::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
  {
    for (auto i : externalInterfaces)
    {
      i->SendMsg(send_bytes, send_bytes_size);
    }
  }



// void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
// {
//     recv_bytes_size++;
//     // DeserializationError error = deserializeJson(received_json, recv_bytes, 256);
//     // if (error)
//     // {
//     //     Serial.println("Error parsing json in motor controller");
//     //     return;
//     // }

//     // if (received_json.containsKey("mode"))
//     // {
//     //     String mode_string = received_json["mode"];

//     //     if (mode_string == "Solid")
//     //     {
//     //     }
//     // }
// }