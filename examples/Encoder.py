import serial
import json
import time
import threading

SERIAL_PORT = 'COM4'

motor_port=serial.Serial()


def listen_serial(ser):
    while True:
        if ser.in_waiting > 0:  # Check if there's data waiting
            message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
            print(f"{message}")

def SetEncoderPosition(pos):
    message = json.dumps({
        "type": "Encoder", 
        "mode": "SetPosition",
        "position":pos
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'{message.decode()}')

def GetEncoderPosition():
    message = json.dumps({
        "type": "Encoder", 
        "mode": "GetPosition",
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'{message.decode()}')

def GetUpdateRate():
    message = json.dumps({
        "type": "Encoder", 
        "mode": "GetUpdateRate",
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'{message.decode()}')

try:
    motor_port = serial.Serial(SERIAL_PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

listener_thread = threading.Thread(target=listen_serial, args=(motor_port,), daemon=True)
listener_thread.start()

GetUpdateRate()
for i in range(0, 10):
    GetEncoderPosition()
    time.sleep(0.1)
time.sleep(1000)



