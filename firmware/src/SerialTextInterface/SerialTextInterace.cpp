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

    if(bytes_available == 0)
    return;

    bytes_available = min(bytes_available, RECV_BUF_SIZE);

    if(Serial.readBytes(&char_buf[0], bytes_available) == 0)
    return;

    HandleIncomingMsg((uint8_t*)&char_buf[0], bytes_available);
    memset(&char_buf[0], 0, 256);
    
}

void SerialTextInterface::HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size = 0)
{
    if(processor_interface_!=nullptr){
        processor_interface_->HandleIncomingMsg(recv_bytes, recv_bytes_size);
    }else{
        Serial.println("processor_interface is null");
    }
}

void SerialTextInterface::SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size) {
    for (uint32_t i = 0; i < send_bytes_size; ++i) {
        uint8_t byte = send_bytes[i];
        
        // Print each byte as a two-digit hexadecimal number
        // Serial.print(byte, HEX) prints the byte in hexadecimal format.
        // Using "print" with HEX format directly might not pad single digits with leading zero.
        
        // Ensure we have two digits for each byte
        if (byte < 16) {
            Serial.print('0');  // Print leading zero if necessary
        }
        Serial.print(byte, HEX);  // Print the byte as hexadecimal
    }
    
    Serial.println();  // Optionally add a newline at the end
}