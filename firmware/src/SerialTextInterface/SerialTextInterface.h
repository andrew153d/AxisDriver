#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task.h"
#include "ExternalInterface.h"
#include <stdlib.h>

#define CR 0x0D
#define LR 0x0A


class SerialTextInterface : public ITask, public ISenderInterface{
private:
  char char_buf[256];
  uint8_t recv_buf[256];
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