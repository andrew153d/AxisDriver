import serial
import time
import struct
import threading
from AxisProtocol import *
from Axis import AxisSerial


def main():
    
    axis = AxisSerial()
    versionMsg = GetVersionMessage()
    axis.send_message(versionMsg) 
    response = axis.wait_message(timeout=2)
    if response:
        major, minor, patch, build = struct.unpack('<BBBB', response[4:8])
        print(f"\nSuccess! Device is running firmware version: {major}.{minor}.{patch}.{build}")
    else:
        print("\nFailed to retrieve firmware version")

if __name__ == "__main__":
    main()
