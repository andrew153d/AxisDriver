import time
from AxisMessages import *
from Axis import *

SerialComms = AxisSerial('/dev/ttyACM0')#AxisUDP("192.168.12.156", 4568)

addr = "192.168.12.156"
test_addr = 0xAF
port = 4568


ret = GetMacAddress(SerialComms).mac
print(ret)

exit()
print("---------- Testing Version ----------")

ret = GetVersion(SerialComms).value

if ret is not None:
    major = (ret >> 24) & 0xFF
    minor = (ret >> 16) & 0xFF
    patch = (ret >> 8) & 0xFF
    build = ret & 0xFF
    print(f"Received Version: {major}.{minor}.{patch}.{build}")
else:
    print("Failed to get version")

print("---------- Testing I2C Address ----------")
print(f"Setting I2C Address to 0x{test_addr:02X}")
ret = GetI2CAddress(SerialComms).value

if ret is not None:
    print(f"Received I2C Address: 0x{ret:02X}")
else:
    print("Failed to get I2C address")
time.sleep(0.01)
SetI2CAddress(SerialComms, test_addr)

def ip_to_uint32(ip):
    parts = ip.split('.')
    return (int(parts[3]) << 24) + (int(parts[2]) << 16) + (int(parts[1]) << 8) + int(parts[0])

print("---------- Testing IP Address ----------")

print(f"Setting IP Address to {addr}")
ip_uint32 = ip_to_uint32(addr)

ret = GetEthernetAddress(SerialComms).value
if ret is not None:
    ip_parts = [
        (ret >> 24) & 0xFF,
        (ret >> 16) & 0xFF,
        (ret >> 8) & 0xFF,
        ret & 0xFF
    ]
    print(f"Received IP Address: {ip_parts[3]}.{ip_parts[2]}.{ip_parts[1]}.{ip_parts[0]}")
else:
    print("Failed to get IP address")

time.sleep(0.1)

SetEthernetAddress(SerialComms, ip_uint32)

print("---------- Testing Port ----------")
print(f"Setting Port to {port}")
ret = GetEthernetPort(SerialComms).value
if ret is not None:
    print(f"Received Port: {ret}")
else:
    print("Failed to get port")
    
SetEthernetPort(SerialComms, port)


print("Saving Settings")
SaveConfiguration(SerialComms, 0)
time.sleep(0.1)
