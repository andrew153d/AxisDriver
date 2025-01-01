import serial
import json
import time

SERIAL_PORT = 'COM6'

motor_port=serial.Serial()
try:
    motor_port = serial.Serial(SERIAL_PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

def SendMotorToPosition(steps, speed = 20000, accel = 20000, microsteps=64):
    message = json.dumps({
        "type": "Motor",
        "mode": "SetPosition",
        "steps": steps,
        "max_speed": speed,
        "acceleration":accel,
        "microsteps": microsteps,
        "hold": 0
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'Sent message: {message.decode()}')


#SendMotorToPosition(50000, 20000, 5000)
def SendMotorVelocity(speed, microsteps=64, duration = 0):
    message = json.dumps({
        "type": "Motor", 
        "mode": "SetVelocity", 
        "speed": speed,
        "duration":duration,
        "microsteps": microsteps
    }).encode('utf-8') + "\r\n ".encode('utf-8')

    if(motor_port.is_open):
        motor_port.write(message)  # Send the JSON message
        print(f'{message.decode()}')

#SendMotorVelocity(500, duration=99000)
#time.sleep(3)
SendMotorToPosition(200*64*3, speed = 6000, accel=8000)



