import serial
import json
import time
import threading

SERIAL_PORT = 'COM4'

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

    return r, g, b

def listen_serial(ser):
    while True:
        if ser.in_waiting > 0:  # Check if there's data waiting
            message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
            print(f"{message}")

def SetLedColor(r, g, b):
    message = json.dumps({
        "type": "Led", 
        "mode": "Solid",
        "red":r,
        "green":g,
        "blue":b
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'{message.decode()}')

def cycle_hues(brightness=1.0):
    for hue in range(0, 360, 1):  # Change the step for different speed
        r, g, b = hue_to_rgb(hue, brightness)
        SetLedColor(r, g, b)
        time.sleep(0.001)  # Adjust delay for how fast you want to cycle

try:
    motor_port = serial.Serial(SERIAL_PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

listener_thread = threading.Thread(target=listen_serial, args=(motor_port,), daemon=True)
listener_thread.start()

SetLedColor(100, 0, 0)
time.sleep(1)
SetLedColor(0, 100, 0)
time.sleep(1)
SetLedColor(0, 0, 100)
time.sleep(1)
cycle_hues()
cycle_hues()
cycle_hues()
SetLedColor(0, 0, 0)




