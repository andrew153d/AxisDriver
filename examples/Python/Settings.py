import Comms
import time

print("---------- Testing Version ----------")
ret = Comms.GetU32(Comms.MessageTypes.GetVersion)
if ret is not None:
    print(f"Received Version: {ret[4]}.{ret[5]}.{ret[6]}.{ret[7]}")
else:
    print("Failed to get version")

print("---------- Testing I2C Address ----------")
test_addr = 0xAD
print("Setting I2C Address to 0x{:02X}".format(test_addr))
Comms.SetU8(Comms.MessageTypes.SetI2CAddress, test_addr)
time.sleep(0.1)
ret = Comms.GetU32(Comms.MessageTypes.GetI2CAddress)
if ret is not None:
    print("Received I2C Address: 0x{:02X}".format(ret[4]))
else:
    print("Failed to get i2c address")

def ip_to_uint32(ip):
    parts = ip.split('.')
    return (int(parts[3]) << 24) + (int(parts[2]) << 16) + (int(parts[1]) << 8) + int(parts[0])

print("---------- Testing Ip Address ----------")
# when in uint32_t, ip addresses are stored with th
addr = "192.168.12.155"
print("Setting IP Address to {}".format(addr))
ip_uint32 = ip_to_uint32(addr)
Comms.SetU32(Comms.MessageTypes.SetEthernetAddress, ip_uint32)
time.sleep(0.1)
ret = Comms.GetU32(Comms.MessageTypes.GetEthernetAddress)
if ret is not None:
    print(f"Received IP Address: {ret[4]}.{ret[5]}.{ret[6]}.{ret[7]}")
    #print(ret)
    #print("Received IP Address: {}".format(".".join(str((ret[4] >> i) & 0xff) for i in (24, 16, 8, 0))))
else:
    print("Failed to get ip address")


print("---------- Testing Port ----------")
port = 4567
print("Setting Port to {}".format(port))
Comms.SetU32(Comms.MessageTypes.SetEthernetPort, port)
time.sleep(0.1)
ret = Comms.GetU32(Comms.MessageTypes.GetEthernetPort)
if ret is not None:
    print("Received Port: {}".format(int.from_bytes(ret[4:8], byteorder='little')))
    #print(int.from_bytes(ret[4:8], byteorder='little'))
else:
    print("Failed to get port")
time.sleep(3)