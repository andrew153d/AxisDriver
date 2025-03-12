import serial
import threading
import time
import socket

class AxisSerial:
    """
    A class to manage serial communication with the Axis Driver.

    """
    def __init__(self, port='/dev/ttyACM0', baudrate=115200):
        self.SERIAL_PORT = port
        self.baudrate = baudrate
        self.motor_port = serial.Serial()
        self.mutex = threading.Lock()
        self.listener_active = True  # Flag to control listener thread
        self.listener_thread = None

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

        self.listener_thread = threading.Thread(target=self.listen_serial, args=(self.motor_port,), daemon=True)
        self.listener_thread.start()

    def listen_serial(self, ser):
        """
        Continuously listens to the serial port for incoming messages.

        Args:
            ser (serial.Serial): The serial port object to listen to.

        Behavior:
            - Sleeps for a short interval to avoid busy-waiting.
            - Acquires a mutex lock to ensure thread-safe access to shared resources.
            - Checks if the listener is active; if not, skips processing.
            - If there is data waiting in the serial buffer, reads and decodes the message.
            - Prints the decoded message for debugging purposes.
        """
        while True:
            time.sleep(0.05)
            with self.mutex:
                if not self.listener_active:  # If listener is paused, skip processing
                    continue
                if ser.in_waiting > 0:  # Check if there's data waiting
                    message = ser.readline().decode('utf-8').rstrip()  # Read and decode the message
                    print(f"DEBUG: {message}")

    def send_message(self, msg):
        """
        Sends a message to the motor via the serial port.

        Parameters:
        msg (bytes): The message to be sent to the motor.

        Returns:
        None
        """
        if self.motor_port.is_open:
            self.motor_port.write(msg)

    def wait_serial_message(self, timeout=1):
        """
        Waits for a serial message from the motor port within a specified timeout period.

        This function continuously checks the motor port for incoming messages until either
        a message is received or the timeout period elapses. If a message is received, it is
        returned; otherwise, the function returns None.

        Args:
            timeout (int, optional): The maximum amount of time (in seconds) to wait for a 
                                     message. Defaults to 1 second.

        Returns:
            str or None: The received message as a string if a message is received within the 
                         timeout period, otherwise None.
        """
        start_time = time.time()
        while time.time() - start_time < timeout:
            with self.mutex:
                if self.motor_port.in_waiting > 0:
                    message = self.motor_port.readline()
                    return message
        return None
    
class AxisUDP:
    """
    A class to manage UDP communication with the Axis Driver.

    """
    def __init__(self, remote_ip, remote_port):
        self.remote_ip = remote_ip
        self.remote_port = remote_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        #self.sock.bind((self.UDP_IP, self.UDP_PORT))
        self.mutex = threading.Lock()
        self.listener_active = True  # Flag to control listener thread
        self.listener_thread = threading.Thread(target=self.listen_udp, daemon=True)
        self.listener_thread.start()

    def listen_udp(self):
        """
        Continuously listens to the UDP socket for incoming messages.

        Behavior:
            - Sleeps for a short interval to avoid busy-waiting.
            - Acquires a mutex lock to ensure thread-safe access to shared resources.
            - Checks if the listener is active; if not, skips processing.
            - If there is data waiting in the socket buffer, reads and decodes the message.
            - Prints the decoded message for debugging purposes.
        """
        while True:
            time.sleep(0.05)
            with self.mutex:
                if not self.listener_active:  # If listener is paused, skip processing
                    continue
                try:
                    data, addr = self.sock.recvfrom(1024)  # Buffer size is 1024 bytes
                    message = data.decode('utf-8').rstrip()
                    print(f"DEBUG: {message}")
                except socket.error as e:
                    print(f"Socket error: {e}")

    def send_message(self, msg):
        """
        Sends a message to the target via the UDP socket.

        Parameters:
        msg (str): The message to be sent.
        target_ip (str): The target IP address.
        target_port (int): The target port number.

        Returns:
        None
        """
        self.sock.sendto(msg, (self.remote_ip, self.remote_port))

    def wait_udp_message(self, timeout=1):
        """
        Waits for a UDP message from the socket within a specified timeout period.

        This function continuously checks the socket for incoming messages until either
        a message is received or the timeout period elapses. If a message is received, it is
        returned; otherwise, the function returns None.

        Args:
            timeout (int, optional): The maximum amount of time (in seconds) to wait for a 
                                     message. Defaults to 1 second.

        Returns:
            str or None: The received message as a string if a message is received within the 
                         timeout period, otherwise None.
        """
        self.sock.settimeout(timeout)
        try:
            data, addr = self.sock.recvfrom(1024)
            return data.decode('utf-8').rstrip()
        except socket.timeout:
            return None
        except socket.error as e:
            print(f"Socket error: {e}")
            return None
