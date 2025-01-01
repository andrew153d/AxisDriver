import json
import time
import threading
import socket
import struct

# Assuming MessageTypes is just a byte value (could be a simple enum-like type)
MessageType = 1  # Example: 1 for a specific message type

# Example Header and Footer structures
HeaderFormat = "B H"  # MessageType (1 byte), body_size (2 bytes)
FooterFormat = "H"    # checksum (2 bytes)
# Network details
SERVER_IP = '192.168.12.111'
SERVER_PORT = 12001
BUFFER_SIZE = 1024

# Create a UDP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def create_led_color_message(red=None, green=None, blue=None, raw=None):
    led_color = struct.pack("BBB", red, green, blue)
    
    # Create the body (led_color)
    body_data = led_color
    
    # Calculate body size (excluding header and footer)
    body_size = len(body_data)
    
    # Pack the header with message type and body size
    header = struct.pack(HeaderFormat, MessageType, body_size)
    
    # Calculate checksum (header + body + footer)
    message_without_footer = header + body_data
    checksum = calculate_checksum(message_without_footer)
    
    # Pack the footer with the checksum
    footer = struct.pack(FooterFormat, checksum)
    
    # Combine header, body, and footer to form the complete message
    message = header + body_data + footer
    
    return message

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

def send_to_server(message):
    """Send message to the server via UDP"""
    try:
        # Send the message as a UDP packet to the specified IP and port
        client_socket.sendto(message.encode('utf-8'), (SERVER_IP, SERVER_PORT))
        print(f"Sent: {message}")
    except Exception as e:
        print(f"Error sending message: {e}")

def SetLedColor(r, g, b):
    """Send LED color data to the server"""
    message = json.dumps({
        "type": "Led", 
        "mode": "Solid",
        "red": r,
        "green": g,
        "blue": b
    }) + "\r\n"
    
    send_to_server(message)

def cycle_hues(brightness=1.0):
    """Cycle through hues and send the corresponding RGB values"""
    for hue in range(0, 360, 1):  # Change the step for different speed
        r, g, b = hue_to_rgb(hue, brightness)
        SetLedColor(r, g, b)
        time.sleep(0.001)  # Adjust delay for how fast you want to cycle

# Main execution
try:
    # Example color changes
    SetLedColor(100, 0, 0)
    time.sleep(1)
    SetLedColor(0, 100, 0)
    time.sleep(1)
    SetLedColor(0, 0, 100)
    time.sleep(1)

    # Cycle through hues multiple times
    cycle_hues()
    cycle_hues()
    cycle_hues()

    # Turn off the LED at the end
    SetLedColor(0, 0, 0)

finally:
    # Close the socket when done
    client_socket.close()
    print("Connection closed.")
