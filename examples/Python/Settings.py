import DriverComms
import Serial
import Messages
import time

SerialComms = Serial.AxisSerial('/dev/ttyACM0')

print("---------- Testing Version ----------")
msg = DriverComms.SetU8(Messages.MessageTypes.GetVersion, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
if ret is not None:
    print(f"Received Version: {ret[4]}.{ret[5]}.{ret[6]}.{ret[7]}")
else:
    print("Failed to get version")

print("---------- Testing I2C Address ----------")
test_addr = 0xAD
print(f"Setting I2C Address to 0x{test_addr:02X}")
msg = DriverComms.SetU8(Messages.MessageTypes.SetI2CAddress, test_addr)
SerialComms.send_message(msg)
time.sleep(0.01)
msg = DriverComms.SetU8(Messages.MessageTypes.GetI2CAddress, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message()
if ret is not None:
    ret = DriverComms.GetU8(ret)
    print(f"Received I2C Address: 0x{ret:02X}")
else:
    print("Failed to get I2C address")

def ip_to_uint32(ip):
    parts = ip.split('.')
    return (int(parts[3]) << 24) + (int(parts[2]) << 16) + (int(parts[1]) << 8) + int(parts[0])

print("---------- Testing IP Address ----------")
addr = "192.168.12.155"
print(f"Setting IP Address to {addr}")
ip_uint32 = ip_to_uint32(addr)
msg = DriverComms.SetU32(Messages.MessageTypes.SetEthernetAddress, ip_uint32)
SerialComms.send_message(msg)
time.sleep(0.01)
msg = DriverComms.SetU32(Messages.MessageTypes.GetEthernetAddress, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message(10)
if ret is not None:
    ret = DriverComms.GetU32(ret)
    ip_parts = [
        (ret >> 24) & 0xFF,
        (ret >> 16) & 0xFF,
        (ret >> 8) & 0xFF,
        ret & 0xFF
    ]
    print(f"Received IP Address: {ip_parts[3]}.{ip_parts[2]}.{ip_parts[1]}.{ip_parts[0]}")
else:
    print("Failed to get IP address")


print("---------- Testing Port ----------")
port = 4567
print(f"Setting Port to {port}")
msg = DriverComms.SetU32(Messages.MessageTypes.SetEthernetPort, port)
SerialComms.send_message(msg)
time.sleep(0.01)
msg = DriverComms.SetU32(Messages.MessageTypes.GetEthernetPort, 0)
SerialComms.send_message(msg)
ret = SerialComms.wait_serial_message(10)
if ret is not None:
    ret = DriverComms.GetU32(ret)
    print(f"Received Port: {ret}")
else:
    print("Failed to get port")
