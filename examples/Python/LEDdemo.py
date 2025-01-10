import serial
import json
import time
import threading

SERIAL_PORT = '/dev/ttyACM0'

motor_port=serial.Serial()

def listen_serial(ser):
    while True:
        if ser.in_waiting > 0:  # Check if there's data waiting
            message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
            print(f"COMMPORT: {message}")

try:
    motor_port = serial.Serial(SERIAL_PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

listener_thread = threading.Thread(target=listen_serial, args=(motor_port,), daemon=True)
listener_thread.start()

def SendMessage(msg):
    if motor_port.is_open:
        motor_port.write(msg)

def SetU8(msg_id, s):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([s])
    body_size = (len(body)).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum
    SendMessage(message)

def SetLedColor(msg_id, r, g, b):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([r, g, b])
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum
    SendMessage(message)

def GetLedColor(msg_id):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([0, 0, 0])
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum
    SendMessage(message)




print("Flashing Error")
SetU8(0x3001, 1)
time.sleep(2)
print("Setting LED state to solid")
SetU8(0x3001, 5)
print("Setting LED state to blue")
SetLedColor(0x3002, 0, 0, 255)
time.sleep(2)
GetLedColor(0x3003)
time.sleep(2)
print("Setting LED state to off")
SetU8(0x3002, 0)


time.sleep(2)