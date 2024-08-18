#include "SerialTextInterface.h"


byte hexCharToByte(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9') {
        return hexChar - '0';
    } else if (hexChar >= 'A' && hexChar <= 'F') {
        return hexChar - 'A' + 10;
    } else if (hexChar >= 'a' && hexChar <= 'f') {
        return hexChar - 'a' + 10;
    }
    // Error case: Invalid hex character
    return 0; // You may want to handle this case differently depending on your requirements
}

void charArrayToByteArray(const char* charArray, byte* byteArray, uint16_t len, uint16_t* out_len) {
    *out_len = 0;
    for (uint16_t i = 0; i < len; i += 2) {
        char highNibble = charArray[i];
        char lowNibble = charArray[i + 1];

        // Convert characters to corresponding byte
        byte highByte = hexCharToByte(highNibble);
        byte lowByte = hexCharToByte(lowNibble);

        // Combine high and low nibbles into one byte
        byteArray[i / 2] = (highByte << 4) | lowByte;
        *out_len = *out_len + 1;
    }
}


void SerialTextInterface::OnStart()
{
    Serial.begin(115200);
    Serial.setTimeout(10);
}

void SerialTextInterface::OnStop()
{
}

void SerialTextInterface::OnRun()
{
    int bytes_available = Serial.available();
    int bytes_read = 0;
    bytes_available = min(bytes_available, 256);

    while(bytes_available){
        bytes_read = Serial.readBytes(&char_buf[bytes_read], bytes_available);
        bytes_available = Serial.available();
        bytes_available = min(bytes_available, 256-bytes_read);
    }

    if(bytes_read > 0){
        HandleIncomingMsg((uint8_t*)&char_buf[0], bytes_read);
        memset(&char_buf[0], 0, 256);
    }

    // if (bytes_available > 0) {
    //     int bytes_to_read = min(bytes_available, 256); // Limit read to 256 bytes
    //     int bytes_read = Serial.readBytes(char_buf, bytes_to_read);
    //     if (bytes_read > 0) {
    //         HandleIncomingMsg((uint8_t*)&char_buf[0], bytes_read);
    //         memset(&char_buf[0], 0, 256);
    //     }
    // }
}

void SerialTextInterface::HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size = 0)
{
    // if(recv_bytes_size%2){
    //     Serial.println("Character not even");
    //     Serial.println(recv_bytes_size);
    //     Serial.println(char_buf);
    //     return;
    // }

    if(recv_bytes[recv_bytes_size-1]==0x0A){
        recv_bytes_size--;
    }
    if(recv_bytes[recv_bytes_size-1]==0x0D){
        recv_bytes_size--;
    }

    uint16_t out_len = 0;
    charArrayToByteArray((char*)recv_bytes, recv_buf, recv_bytes_size, &out_len);
    
    if(processor_interface_!=nullptr){
        processor_interface_->HandleIncomingMsg(&recv_buf[0], out_len);
    }else{
        Serial.println("processor_interface is null");
    }
}

void SerialTextInterface::SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size){
    Serial.println(" Trying to send message ");
}