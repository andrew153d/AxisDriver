#pragma once

#include <cstdint>
#include "AxisMessages.h"
#include <ArduinoJson.h>
#include "Task/Task.h"

class IExternalInterface; // Forward declaration

class IProcessorInterface
{
protected:
public:
  virtual void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) = 0;
  virtual void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size, IExternalInterface* calling_interface) = 0;
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

class MessageProcessor : public ITask, public IProcessorInterface
{
private:
  std::vector<IExternalInterface *> externalInterfaces; // Vector to hold pointers to interfaces
  IExternalInterface *last_interface_ = nullptr;
  uint8_t send_buffer[1024];
  void SendAck(MessageTypes msg_type, StatusCodes status);
  uint16_t CalculateChecksum(uint8_t *data, uint32_t size);
public:
  MessageProcessor(uint32_t period);

  void AddExternalInterface(IExternalInterface *new_interface);

  void OnStart() override;
  void OnStop() override;
  void OnRun() override;

  void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
  void HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
  void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size, IExternalInterface* calling_interface);

  void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size);
};
