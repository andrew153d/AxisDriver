#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.h"
#include <stdlib.h>
#include <ArduinoJson.h>

#define CR 0x0D
#define LR 0x0A
#define RECV_BUF_SIZE 256

class SerialTextInterface : public ITask, public IExternalInterface{
private:
  char char_buf[RECV_BUF_SIZE];
  uint8_t recv_buf[RECV_BUF_SIZE];
  int bytes_read = 0;
public:
    SerialTextInterface(uint32_t period){
      executionPeriod = period;
    }

    void OnStart();

    void OnStop();

    void OnRun();

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);
};