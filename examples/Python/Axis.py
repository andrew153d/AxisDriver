import serial
import threading
import time
import socket
import re
from AxisMessages import *

class Axis:
    """
    A base class for Axis communication, providing common functionality.
    """
    def __init__(self):
        self.mutex = threading.Lock()
        self.listener_active = True  # Flag to control listener thread
        self.listener_thread = None

    def listen(self):
        """
        Abstract method to be implemented by subclasses for listening to messages.
        """
        raise NotImplementedError("Subclasses must implement this method.")

    def send_message(self, msg):
        """
        Abstract method to be implemented by subclasses for sending messages.
        """
        raise NotImplementedError("Subclasses must implement this method.")

    def wait_message(self, timeout=1):
        """
        Abstract method to be implemented by subclasses for waiting for messages.
        """
        raise NotImplementedError("Subclasses must implement this method.")


class AxisSerial(Axis):
    """
    A class to manage serial communication with the Axis Driver.
    """
    def __init__(self, port='/dev/ttyACM0', baudrate=115200):
        super().__init__()
        self.SERIAL_PORT = port
        self.baudrate = baudrate
        self.motor_port = serial.Serial()

        try:
            self.motor_port = serial.Serial(
                port=self.SERIAL_PORT,
                baudrate=self.baudrate,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=1
            )
            print(f"Connected to serial port {self.SERIAL_PORT}")
        except serial.SerialException as e:
            print(f"Error: {e}")
        except Exception as e:
            print(f"An error occurred: {e}")

        self.listener_thread = threading.Thread(target=self.listen, args=(self.motor_port,), daemon=True)
        self.listener_thread.start()

    def listen(self, ser):
        while True:
            time.sleep(0.05)
            with self.mutex:
                if not self.listener_active:
                    continue
                if ser.in_waiting > 0:
                    message = ser.readline().decode('utf-8').rstrip()
                    print(f"DEBUG: {message}")

    def send_message(self, msg):
        if self.motor_port.is_open:
            self.motor_port.write(msg)

    def wait_message(self, timeout=1):
        start_time = time.time()
        while time.time() - start_time < timeout:
            with self.mutex:
                if self.motor_port.in_waiting > 0:
                    message = self.motor_port.readline()
                    return message
        return None


class AxisUDP(Axis):
    """
    A class to manage UDP communication with the Axis Driver.
    """
    def __init__(self, remote_ip, remote_port):
        super().__init__()
        self.remote_ip = remote_ip
        self.remote_port = remote_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def listen(self):
        while True:
            time.sleep(0.05)
            with self.mutex:
                if not self.listener_active:
                    continue
                try:
                    data, addr = self.sock.recvfrom(1024)
                    message = data.decode('utf-8').rstrip()
                    print(f"DEBUG: {message}")
                except socket.error as e:
                    print(f"Socket error: {e}")

    def send_message(self, msg):
        self.sock.sendto(msg, (self.remote_ip, self.remote_port))

    def wait_message(self, timeout=1):
        self.sock.settimeout(timeout)
        try:
            data, addr = self.sock.recvfrom(1024)
            return data
        except socket.timeout:
            print(f"Socket timeout")
            return None
        except socket.error as e:
            print(f"Socket error: {e}")
            return None