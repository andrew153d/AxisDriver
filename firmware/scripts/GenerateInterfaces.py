import json

CPP_Header = """#pragma once

#define FIRMWARE_VERSION "1.0.0.0"

#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t *recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t *send_bytes, uint32_t send_bytes_size);

class IEncoderInterface
{
public:
    virtual float GetVelocityDegreesPerSecond();
    virtual float GetPositionDegrees();
};
"""


# def generate_c_code(messages):
#     c_code = """#include "messages.h"

# """
#     for msg in messages:
#         if msg["type"] == "Custom":
#             fields = msg["fields"]
#             c_code += f"void Create{msg['name']}(uint8_t *buffer) {{\n"
#             c_code += "    Header *header = (Header *)buffer;\n"
#             c_code += f"    header->message_type = {msg['name']}_TYPE;\n"
#             for field in fields:
#                 c_code += f"    *(uint32_t *)(buffer + sizeof(Header)) = {field['name']};\n"
#             c_code += "}\n\n"
#         else:
#             c_code += f"void Create{msg['name']}(uint8_t *buffer, uint32_t value) {{\n"
#             c_code += "    Header *header = (Header *)buffer;\n"
#             c_code += f"    header->message_type = {msg['name']}_TYPE;\n"
#             c_code += "    *(uint32_t *)(buffer + sizeof(Header)) = value;\n"
#             c_code += "}\n\n"
#     return c_code

def GetLen(type):
    print(type.split('[')[1])
    return 0

def GetBytes(size):
    if size == "uint8_t":
        return 1
    if size == "int8_t":
        return 1
    if size == "uint16_t":
        return 2
    if size == "int16_t":
        return 2
    if size == "uint32_t":
        return 4
    if size == "int32_t":
        return 4
    if size == "double":
        return 8
    print(size)
    return 0

def generate_python_code(messages):
    py_code = """import struct\n\n"""

    # make the predefined functions
    for msg in messages["PredefinedMessages"]:
        # function defenition
        py_code += f"def Create{msg}("
        first=True
        for field in messages["PredefinedMessages"][msg]:
            if(first):
                first = False
            else:
                py_code+=", "
            py_code += field['name']
        py_code+='):\n'
        py_code+="\tmessage = bytearray(0)\n"
        
        #fill in the fields
        for field in messages["PredefinedMessages"][msg]:
                py_code += (f"\tmessage += {field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n")

        # return the result
        py_code+="\treturn message\n\n"

    already_defined_messages = []
    for msg in messages["messages"]:
        # function defenition
        py_code += f"def {msg['name']}("
        if msg['type'] == "Custom":
            py_code += "message_id, "
            first=True
            for field in msg['fields']:
                if(first):
                    first = False
                else:
                    py_code+=", "
                py_code += field['name']
            py_code+='):\n'
            py_code += "\tbody = bytearray(0)\n"
            for field in msg['fields']:
                if(field['type'].find('[')!=-1):
                    py_code += f"\tbody+={field['name']}\n"
                else:
                    py_code += f"\tbody += {field['name']}.to_bytes({GetBytes(field['type'])},'little')\n"
        else:
            py_code += "message_id, value):\n"

            #fill in the fields
            py_code += f"\tbody = Create{msg['type']}(value)\n"
        
        py_code += f"\theader = CreateHeader(message_id, len(body))\n"
        py_code += f"\tfooter = CreateFooter(sum(body) & 0xFF)\n"
        # return the result
        py_code+="\treturn header+body+footer\n\n"

    py_code += "def print_bytes(byte_array):\n"
    py_code += "\tprint(''.join(f'0x{byte:02x} ' for byte in byte_array))\n"
    py_code += "print_bytes(CreateSetVelocityAndStepsMessage(0x11, 0x22, 0x33))\n"
    return py_code

with open("interface.json", "r") as file:
    messages = json.load(file)

# with open("messages.c", "w") as file:
#     file.write(generate_c_code(messages))

with open("examples/Python/automessages.py", "w") as file:
    file.write(generate_python_code(messages))
