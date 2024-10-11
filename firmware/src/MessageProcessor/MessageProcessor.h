#pragma once

#include <cstdint>
#include "Messages.h"
#include <ArduinoJson.h>

class IProcessorInterface
{
protected:
public:
  virtual void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) = 0;
  virtual void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size) = 0;
};

class IExternalInterface
{
protected:
  IProcessorInterface *processor_interface_ = nullptr;

public:
  virtual void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) = 0;

  void SetProcessorInterface(IProcessorInterface *proc_interface)
  {
    processor_interface_ = proc_interface;
  }
};

class IInternalInterface
{
protected:
  IProcessorInterface *processor_interface_ = nullptr;

public:
  virtual void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size) = 0;

  void SetProcessorInterface(IProcessorInterface *proc_interface)
  {
    processor_interface_ = proc_interface;
  }
};

struct InterfaceLimits
{
  IInternalInterface *interface;
  String type;
  uint16_t lowLimit;
  uint16_t highLimit;
};

class MessageProcessor : public ITask, public IProcessorInterface
{
private:
  std::vector<IExternalInterface *> externalInterfaces; // Vector to hold pointers to interfaces
  std::vector<InterfaceLimits> controllerInterfaces;    // Vector to hold pointers to interfaces
  IExternalInterface *last_interface_ = nullptr;

public:
  MessageProcessor(uint32_t period)
  {
    executionPeriod = period;
  }

  void AddExternalInterface(IExternalInterface *new_interface)
  {
    externalInterfaces.push_back(new_interface);
    new_interface->SetProcessorInterface(this);
  }

  void AddControllerInterface(IInternalInterface *new_interface, String message_type, MessageTypes low, MessageTypes high)
  {
    InterfaceLimits interface_to_add;
    interface_to_add.interface = new_interface;
    interface_to_add.type = message_type;
    interface_to_add.highLimit = (uint16_t)high;
    interface_to_add.lowLimit = (uint16_t)low;
    controllerInterfaces.push_back(interface_to_add);
    new_interface->SetProcessorInterface(this);
  }

  void OnStart() override
  {
  }

  void OnStop() override
  {
  }

  void OnRun() override
  {
  }

  void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size = 0)
  {
    if (recv_bytes_size == 0)
      return;

    if (recv_bytes[0] == '{')
    {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, recv_bytes, 256);
      if (error)
      {
        Serial.println("Error parsing json");
        return;
      }
      for (InterfaceLimits interface : controllerInterfaces)
      {
        if (interface.type == doc["type"])
        {
          interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
        }
      }
    }
    else
    {
      Header *hdr = (Header *)&recv_bytes[0];

      for (InterfaceLimits interface : controllerInterfaces)
      {
        if ((uint16_t)hdr->message_type > interface.lowLimit && (uint16_t)hdr->message_type < interface.highLimit)
        {
          interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
        }
      }
    }
  }

  void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
  {
    for (auto i : externalInterfaces)
    {
      i->SendMsg(send_bytes, send_bytes_size);
    }
  }
};
