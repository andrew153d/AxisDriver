import serial
import json
import time
import threading

SERIAL_PORT = '/dev/ttyACM0'

motor_port=serial.Serial()


def hue_to_rgb(hue, brightness=1.0):
    """Convert hue (0-360) to RGB (0-255) with brightness (0.0-1.0)"""
    if hue < 0 or hue >= 360:
        raise ValueError("Hue must be between 0 and 360")
    if brightness < 0 or brightness > 1:
        raise ValueError("Brightness must be between 0.0 and 1.0")

    c = 255 * brightness  # Chroma adjusted for brightness
    x = c * (1 - abs((hue / 60) % 2 - 1))
    m = 0  # Value offset

    if 0 <= hue < 60:
        r, g, b = c, x, 0
    elif 60 <= hue < 120:
        r, g, b = x, c, 0
    elif 120 <= hue < 180:
        r, g, b = 0, c, x
    elif 180 <= hue < 240:
        r, g, b = 0, x, c
    elif 240 <= hue < 300:
        r, g, b = x, 0, c
    elif 300 <= hue < 360:
        r, g, b = c, 0, x

    # Adjust values to [0, 255] scale
    r = int(r)
    g = int(g)
    b = int(b)

    return r, g, bf

def listen_serial(ser):
    while True:
        if ser.in_waiting > 0:  # Check if there's data waiting
            message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
            print(f"COMMPORT: {message}")

def SetLedColor(msg_id, r, g, b):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([r, g, b])
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum

    if motor_port.is_open:
        motor_port.write(message)
        #print(f'Sent bytes: {[f"0x{byte:02x}" for byte in message]}')

def GetLedColor(msg_id):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([0, 0, 0])
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum

    if motor_port.is_open:
        motor_port.write(message)
        #print(f'Sent bytes: {[f"0x{byte:02x}" for byte in message]}')

def SetLedState(msg_id, s):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([s])
    body_size = (len(body)).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum

    if motor_port.is_open:
        motor_port.write(message)
        #print(f'Sent bytes: {[f"0x{byte:02x}" for byte in message]}')

try:
    motor_port = serial.Serial(SERIAL_PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

listener_thread = threading.Thread(target=listen_serial, args=(motor_port,), daemon=True)
listener_thread.start()

print("Flashing Error")
SetLedState(0x3001, 1)
time.sleep(2)
print("Setting LED state to solid")
SetLedState(0x3001, 5)
print("Setting LED state to blue")
SetLedColor(0x3002, 0, 0, 255)
time.sleep(2)
GetLedColor(0x3003)
time.sleep(2)
print("Setting LED state to off")
SetLedState(0x3002, 0)


time.sleep(2)