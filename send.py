import serial
 

data_to_send = [0x41, 0x03, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00]


try:
        # Open COM6 with baudrate 9600, you may need to adjust baudrate if different
        ser = serial.Serial('COM6', 115200, timeout=1)
        print(f"Opened {ser.name} successfully.")

        # Convert data array to bytes
        bytes_to_send = bytes(data_to_send)

        # Send the bytes
        for byte in bytes_to_send:
            byte_hex = hex(byte)  # Convert byte to hexadecimal string
            print(f"Sending byte: {byte_hex}")
            ser.write(bytes([byte]))  # Send the byte
        
        # Close the serial port
        ser.close()
        print("Closed the port.")

except serial.SerialException as e:
    print(f"Error opening or writing to COM6: {e}")