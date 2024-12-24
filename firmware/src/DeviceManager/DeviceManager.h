#pragma once

#include <cstdint>
#include <Arduino.h>
#include "Task/Task.h"
#include "MessageProcessor/MessageProcessor.h"
#include "Messages.hpp"


class DeviceManager : public ITask, public IInternalInterface {
public:
    DeviceManager(uint32_t period){
        executionPeriod = period;
    }
    void OnStart();
    void OnStop();
    void OnRun();

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size);
};