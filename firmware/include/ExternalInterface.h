#pragma once

#include <cstdint>

typedef void (*HandleIncomingMsgPtrType)(uint8_t* recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t* send_bytes, uint32_t send_bytes_size);

class ISenderInterface {
    protected:
        ISenderInterface* processor_interface_ = nullptr;
    public:
        virtual void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size) = 0;
        virtual void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size) = 0;
        
        void SetProcessorInterface(ISenderInterface* proc_interface){
            processor_interface_ = proc_interface;
        }
};

