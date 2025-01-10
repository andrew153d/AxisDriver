import serial
import json
import time
import threading

class MessageTypes:
    # General Device Info
    GetVersion = 0x0002

    # Device Settings
    SetI2CAddress = 0x0500
    GetI2CAddress = 0x0501
    SetEthernetAddress = 0x0502
    GetEthernetAddress = 0x0503
    SetEthernetPort = 0x0504
    GetEthernetPort = 0x0505
    GetMacAddress = 0x0506

    # LED control
    SetLedState = 0x3001
    SetLedColor = 0x3002
    GetLedColor = 0x3003

    # Ethernet Configuration

    # Drive Configuration

    # Motor Driving
    GetMotorPosition = 0x5001
    GetMotorVelocity = 0x5002
    GetMotorCurrent = 0x5003

    SetMotorPosition = 0x5004
    SetMotorVelocity = 0x5005
    SetMotorCurrent = 0x5006

    SetControlMode = 0x5007
    GetControlMode = 0x5008

    GetMotorStatus = 0x5009
    StartMotor = 0x500A

SERIAL_PORT = '/dev/ttyACM0'

motor_port = serial.Serial()
mutex = threading.Lock()
listener_active = True  # Flag to control listener thread

def listen_serial(ser):
    while True:
        time.sleep(0.05)
        with mutex:
            if not listener_active:  # If listener is paused, skip processing
                continue
            if ser.in_waiting > 0:  # Check if there's data waiting
                message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
                print(f"DEBUG: {message}")

try:
    motor_port = serial.Serial(
    port=SERIAL_PORT,
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout = 1
)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

listener_thread = threading.Thread(target=listen_serial, args=(motor_port,), daemon=True)
listener_thread.start()

def SendMessage(msg):
    if motor_port.is_open:
        motor_port.write(msg)

def WaitSerialMessage(timeout=1):
    start_time = time.time()
    while time.time() - start_time < timeout:
        with mutex:
            if motor_port.in_waiting > 0:
                message = motor_port.readline()
                return message
    return None

def SetU8(msg_id, value):
    message_type = msg_id.to_bytes(2, 'little')
    body = (value).to_bytes(1, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    SendMessage(message)

def GetU8(msg_id):
    global listener_active
    listener_active = False  # Pause listener thread
    message_type = msg_id.to_bytes(2, 'little')
    body = (0).to_bytes(1, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    SendMessage(message)
    time.sleep(0.1)
    response = WaitSerialMessage()
    listener_active = True  # Resume listener thread
    return response

def GetU32(msg_id):
    global listener_active
    listener_active = False  # Pause listener thread
    message_type = msg_id.to_bytes(2, 'little')
    body = (0).to_bytes(4, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    SendMessage(message)
    time.sleep(0.1)
    response = WaitSerialMessage()
    listener_active = True  # Resume listener thread
    return response

def SetU32(msg_id, value):
    message_type = msg_id.to_bytes(2, 'little')
    body = (value).to_bytes(4, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    SendMessage(message)

def print_bytes(byte_array):
    print("".join(f"{byte:02x}" for byte in byte_array))
