import serial
import json
import time
import threading
import struct



def SetU8(msg_id, value):
    message_type = msg_id.to_bytes(2, 'little')
    body = (value).to_bytes(1, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    return message

def GetU8(message):
    return message[4]


def SetU32(msg_id, value):
    message_type = msg_id.to_bytes(2, 'little')
    body = (value).to_bytes(4, 'little')
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum    
    return message

def GetU32(message):
    return int.from_bytes(message[4:8], byteorder='little')

def SetDouble(msg_id, value):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray(struct.pack('d', value))
    body_size = (len(body)).to_bytes(2, 'little')
    checksum = (sum(body) & 0xFF).to_bytes(2, 'little')
    message = message_type + body_size + body + checksum
    return message

def GetDouble(message):
    return struct.unpack('d', message[4:12])[0]

def SetLedColor(msg_id, r, g, b):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray([r, g, b])
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum
    return message

def GetMacAddressMessage(msg_id):
    message_type = msg_id.to_bytes(2, 'little')
    body = bytearray(6)
    body_size = (3).to_bytes(2, 'big')
    checksum = (sum(body) & 0xFF).to_bytes(1, 'big')
    message = message_type + body_size + body + checksum
    return message

def print_bytes(byte_array):
    print("".join(f"{byte:02x}" for byte in byte_array))
