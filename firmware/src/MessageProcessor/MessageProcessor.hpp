#pragma once

#include <cstdint>
#include "Messages.h"
#include <ArduinoJson.h>
#include "Task/Task.h"

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

class MessageProcessor : public ITask, public IProcessorInterface
{
private:
  std::vector<IExternalInterface *> externalInterfaces; // Vector to hold pointers to interfaces
  IExternalInterface *last_interface_ = nullptr;

public:
  MessageProcessor(uint32_t period);

  void AddExternalInterface(IExternalInterface *new_interface);

  void OnStart() override;
  void OnStop() override;
  void OnRun() override;

  void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
  void HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
  void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size = 0);

  void SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size);
};
