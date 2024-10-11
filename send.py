import serial
import json
import time

# Configuration for the serial port
SERIAL_PORT = 'COM4'  # Replace with your serial port (e.g., '/dev/ttyUSB0' on Linux)
BAUD_RATE = 115200       # Set the appropriate baud rate

# Create the JSON message
message = json.dumps({
    "type": "MOVEMENT",
    "mode": "POSITION",
    "steps": -91000,
    "speed": 100,
    "stepmode": 64,
    "hold": 0
}).encode('utf-8') + b'\n'  # Append newline for Arduino to detect end of message

#print(message) #{"type": "MOVEMENT", "mode": "POSITION", "steps": 1000, "speed": 100, "stepmode": 64, "hold": 0}

try:
    # Open the serial port
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2) as ser:
        if(ser.is_open):
            ser.write(message)  # Send the JSON message
            print(f'Sent message: {message.decode()}')
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")
