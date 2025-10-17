import serial
import time
import struct
import threading
from AxisProtocol import *
from Axis import AxisSerial


def main():
    
    axis = AxisSerial()
    
    # Test Version Message
    versionMsg = GetVersionMessage()
    axis.send_message(versionMsg) 
    response = axis.wait_message(timeout=2)
    if response:
        major, minor, patch, build = struct.unpack('<BBBB', response[4:8])
        print(f"\nSuccess! Device is running firmware version: {major}.{minor}.{patch}.{build}")
    else:
        print("\nFailed to retrieve firmware version")

    # Set I2C Address Test, expects Ack
    new_i2c_address = 0x42
    setI2CMsg = SetI2CAddressMessage(new_i2c_address)
    axis.send_message(setI2CMsg)
    response = axis.wait_message(timeout=2)
    if response:
        ack_code = struct.unpack('<B', response[6:7])[0]
        if ack_code == 0:
            print(f"\nSuccess! I2C address set to: {hex(new_i2c_address)}")
        else:
            # Print raw response bytes and hex for debugging
            print("\nResponse bytes:", response)
            print("Ack code (hex):", response[6])
            print("\nFailed to set I2C address, NACK received")
    else:
        print("\nNo response received for Set I2C Address")
        
    # Get I2C Address Test
    getI2CMsg = GetI2CAddressMessage()
    axis.send_message(getI2CMsg)
    response = axis.wait_message(timeout=2)
    if response:
        current_i2c_address = struct.unpack('<B', response[4:5])[0]
        print(f"\nSuccess! Current I2C address is: {hex(current_i2c_address)}")
    else:
        print("\nFailed to retrieve I2C address")
        
    # Set Ethernet Address Test, expects Ack
    ipAddr = "192.168.12.1"
    ip_bytes = bytes(int(octet) for octet in ipAddr.split('.'))
    ip_int = int.from_bytes(ip_bytes, byteorder='little', signed=False)
    setEthMsg = SetEthernetAddressMessage(ip_int)
    axis.send_message(setEthMsg)
    response = axis.wait_message(timeout=2)
    if response:
        ack_code = struct.unpack('<B', response[6:7])[0]
        if ack_code == 0:
            print(f"\nSuccess! Ethernet address set to: {'.'.join(f'{b:d}' for b in ip_bytes)}")
        else:
            print("\nFailed to set Ethernet address, NACK received")
    else:
        print("\nNo response received for Set Ethernet Address")
        
    # Get Ethernet Address Test
    getEthMsg = GetEthernetAddressMessage()
    axis.send_message(getEthMsg)
    response = axis.wait_message(timeout=2)
    if response:
        eth_address = struct.unpack('<I', response[4:8])[0]
        print(f"\nSuccess! Current Ethernet address is: {'.'.join(str((eth_address >> (8 * i)) & 0xFF) for i in range(4))}")
    else:
        print("\nFailed to retrieve Ethernet address")
    

if __name__ == "__main__":
    main()
