import json

CPP_Header = """#pragma once
#include <Arduino.h>
#define FIRMWARE_VERSION "1.0.0.0"

#define PACKEDSTRUCT struct __attribute__((packed))

typedef void (*HandleIncomingMsgPtrType)(uint8_t *recv_bytes, uint32_t recv_bytes_size);
typedef void (*SendMsgPtrType)(uint8_t *send_bytes, uint32_t send_bytes_size);

class IEncoderInterface
{
public:
    virtual float GetVelocityDegreesPerSecond();
    virtual float GetPositionDegrees();
	virtual float GetUpdateRate();
};
"""


Python_Header = """
from Axis import *
import struct\n
"""

def GetStructFormat(type):
    if type == "uint8_t":
        return 'B'
    if type == "int8_t":
        return 'b'
    if type == "uint16_t":
        return 'H'
    if type == "int16_t":
        return 'h'
    if type == "uint32_t":
        return 'I'
    if type == "int32_t":
        return 'i'
    if type == "double":
        return 'd'
    return None

def GetBytes(size:str):
    if(size.find("[")!=-1):
        arr_len = int(size[size.find('[')+1:size.find(']')])
        return arr_len * GetBytes(size[:size.find('[')])
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
    #print(size)
    return 0

def ConvertCTypeToPythonType(input_type):
    if input_type == "uint8_t":
        return "int"
    if input_type == "int8_t":
        return "int"
    if input_type == "uint16_t":
        return "int"
    if input_type == "int16_t":
        return "int"
    if input_type == "uint32_t":
        return "int"
    if input_type == "int32_t":
        return "int"
    if input_type == "double":
        return "float"
    if input_type.find('[')!=-1:
        return "bytearray"
    return None

def GeneratePythonEnum(enum_name, enum_def):
    ret_string = ''
    
    ret_string += f"#{enum_name}\n"
    
    count = 0x00
    for enum_member in enum_def:
        if(enum_member.find('=')!=-1):
            ret_string += f"{enum_member.split('=')[0].strip()} = {enum_member.split('=')[1].strip()}\n"
        else:
            ret_string += f"{enum_member} = 0x{count:X}\n"
        count = count + 1
    ret_string+="\n"
    return ret_string

def GeneratePythonBaseClass(msg):
    ret_string = ""
    ret_string += f"class {msg['name']}: \n"
    
    #__init__
    ret_string += f"\tdef __init__(self, "
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += "):\n"
    for field in msg['fields']:
        ret_string += f"\t\tself.{field['name']} = {field['name']}\n"
    ret_string += "\n"

    # serialize
    ret_string += "\tdef serialize(self):\n"
    ret_string += "\t\tret = bytearray(0)\n"
    for field in msg['fields']:
        #bytearray(struct.pack('d', 100*8))
        if field['type'] == 'double':
            ret_string += f"\t\tret += bytearray(struct.pack('d', self.value))\n"
        else:
            ret_string += f"\t\tret += self.{field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n"
    ret_string += "\t\treturn ret\n\n"

    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    counter = 0
    for field in msg['fields']:
        if(field['type']!='double'):
            ret_string += f"\t\t{field['name']} = {ConvertCTypeToPythonType(field['type'])}.from_bytes(byte_array[{counter}:{GetBytes(field['type'])+counter}], 'little')\n"
        else:
            ret_string += f"\t\t{field['name']} = struct.unpack('d', byte_array[{counter}:{GetBytes(field['type'])+counter}])[0]\n"
            #struct.unpack('d', message[4:12])[0]
            
        counter += GetBytes(field['type'])
    ret_string += "\t\treturn cls("
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += ")\n\n"

    return ret_string

def GeneratePythonPredefinedClass(msg, predefined):
    body_size = GetBytes(predefined[msg['type']]['fields'][0]['type'])
    ret_string = ""
    ret_string += f"class {msg['name']}: \n"
    
    #__init__
    ret_string += f"\tdef __init__(self, msg_id, value):\n"
    ret_string += f"\t\tself.body = {msg['type']}(value)\n"
    ret_string += f"\t\tself.header = Header(msg_id, {body_size})\n"
    ret_string += f"\t\tself.footer = Footer(sum(self.body.serialize())%0xFFFF)\n\n"

    # serialize
    ret_string+="\tdef serialize(self):\n"
    ret_string+=    "\t\tret = bytearray(0)\n"
    ret_string+=    "\t\tret += self.header.serialize()\n"
    ret_string+=    "\t\tret += self.body.serialize()\n"
    ret_string+=    "\t\tret += self.footer.serialize()\n"
    ret_string+=    "\t\treturn ret\n\n"
    
    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    ret_string += "\t\theader = Header.deserialize(byte_array[0:4])\n"
    ret_string += f"\t\tbody = {msg['type']}.deserialize(byte_array[4:{4+body_size}])\n"
    ret_string += f"\t\tfooter = Footer.deserialize(byte_array[{4+body_size}:{4+body_size+2}])\n"
    ret_string += f"\t\tmsg = cls(header.message_type, body.value)\n"
    ret_string += f"\t\tmsg.body = body\n"
    ret_string += f"\t\tmsg.header = header\n"
    ret_string += f"\t\tmsg.footer = footer\n"
    ret_string += "\t\treturn msg\n\n"
    return ret_string

def GeneratePythonCustomClass(msg):
    body_size = 0 #GetBytes(predefined[msg['type']]['fields'][0]['type'])
    for field in msg['fields']:
        body_size += GetBytes(field['type'])
    
    ret_string = ""
    ret_string += f"class {msg['name'].split('Message')[0]}Body: \n"
    #__init__
    ret_string += f"\tdef __init__(self, "
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += "):\n"
    for field in msg['fields']:
        ret_string += f"\t\tself.{field['name']} = {field['name']}\n"
    ret_string += "\n"
    # struct.pack('<i', vel) + struct.pack('<i', steps)
    # serialize
    ret_string += "\tdef serialize(self):\n"
    ret_string += "\t\tret = bytearray(0)\n"
    for field in msg['fields']:
        if(field['type'].find('[')!=-1):
            ret_string += f"\t\tret += bytearray(self.{field['name']})\n"
        else:   
            ret_string += f"\t\tret += struct.pack('<{GetStructFormat(field['type'])}', self.{field['name']})#self.{field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n"
            #ret_string += f"\t\tret += self.{field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n"
    ret_string += "\t\treturn ret\n\n"

    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    counter = 0
    for field in msg['fields']:
        if(field['type'].find('[')!=-1):
            ret_string += f"\t\t{field['name']} = {ConvertCTypeToPythonType(field['type'])}(byte_array[{counter}:{GetBytes(field['type'])+counter}])\n"
        else:
            ret_string += f"\t\t{field['name']} = {ConvertCTypeToPythonType(field['type'])}.from_bytes(byte_array[{counter}:{GetBytes(field['type'])+counter}], 'little')\n"
        counter += GetBytes(field['type'])
    ret_string += f"\t\tmsg = cls("
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += ")\n"
    ret_string += "\t\treturn msg\n\n"
       
    ret_string += f"class {msg['name']}: \n"
    #__init__
    ret_string += f"\tdef __init__(self, msg_id, "
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += "):\n"
    ret_string += f"\t\tself.header = Header(msg_id, {body_size})\n"
    ret_string += f"\t\tself.body = {msg['name'].split('Message')[0]}Body("
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += ")\n"
    ret_string += f"\t\tself.footer = Footer(sum(self.body.serialize())%0xFFFF)\n\n"

    # serialize
        # serialize
    ret_string+="\tdef serialize(self):\n"
    ret_string+=    "\t\tret = bytearray(0)\n"
    ret_string+=    "\t\tret += self.header.serialize()\n"
    ret_string+=    "\t\tret += self.body.serialize()\n"
    ret_string+=    "\t\tret += self.footer.serialize()\n"
    ret_string+=    "\t\treturn ret\n\n"

    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    ret_string += "\t\theader = Header.deserialize(byte_array[0:4])\n"
    ret_string += f"\t\tbody = {msg['name'].split('Message')[0]}Body.deserialize(byte_array[4:{4+body_size}])\n"
    ret_string += f"\t\tfooter = Footer.deserialize(byte_array[{4+body_size}:{4+body_size+2}])\n"
    ret_string += f"\t\tmsg = cls(header.message_type"
    for field in msg['fields']:
        ret_string += f", body.{field['name']}"
    ret_string += ")\n"
    ret_string += f"\t\tmsg.body = body\n"
    ret_string += f"\t\tmsg.header = header\n"
    ret_string += f"\t\tmsg.footer = footer\n"
    ret_string += "\t\treturn msg\n\n"

    return ret_string

def GeneratePythonGetter(msg_id, msg_def, predefined):
    getter_fun = msg_id.replace("Id","")
    message_class = msg_id.replace("Id","").replace('Get', '').replace('Set', '')+'Message'
    
    ret_string = ""
    ret_string += f"def {getter_fun}(axis:Axis"
    ret_string += ", timeout = 0.1):\n"
    ret_string += f"\tsend_msg = {message_class}({msg_id}"
    #print(msg_def)
    if(msg_def['type'] == 'Custom'):
        for field in msg_def['fields']:
            ret_string += f", 0"
    else:
        ret_string += ", 0"
    ret_string += ")\n"
    
    ret_string += f"\taxis.send_message(send_msg.serialize())\n"
    ret_string += f"\tret = axis.wait_message(timeout)\n"
    ret_string += f"\tif(ret[0]=={msg_id}):\n"
    ret_string += f"\t\tmsg = {message_class}.deserialize(ret)\n"
    ret_string += f"\t\treturn msg.body\n"
    ret_string += f"\telse:\n"
    ret_string += "\t\t return None\n"
    ret_string += "\n"
    
    return ret_string

def GeneratePythonSetter(msg_id, msg_def, predefined):
    setter_fun = msg_id.replace("Id","")
    message_class = msg_id.replace("Id","").replace('Get', '').replace('Set', '')+'Message'
    body_class = f"{msg['name'].split('Message')[0]}Body" if msg_def['type'] == 'Custom' else msg_def.get('type', '')
    #print(msg_def)
    ret_string = ""
    ret_string += f"def {setter_fun}(axis:Axis, "
    
    if(msg_def['type'] == 'Custom'):
        ret_string += ", ".join([field['name'] for field in msg_def['fields']])
    else:
        ret_string += "value"
    
    ret_string += "):\n"
    
    ret_string += f"\tsend_msg = {message_class}({msg_id}, "
    if(msg_def['type'] == 'Custom'):
        ret_string += ", ".join([field['name'] for field in msg_def['fields']])
    else:
        ret_string += "value"
    ret_string += ")\n"
    # for field in msg_def['fields']:
    #     print(field)
    #ret_string += f"\tsend_msg.body = body\n"
    ret_string += f"\taxis.send_message(send_msg.serialize())\n"
    ret_string += "\n"
    
    return ret_string

def GenerateCppEnum(enum_name, enum_def):
    ret_string = ''
    
    ret_string += f"enum class {enum_name}"
    ret_string += "{\n"
    
    count = 0x00
    for enum_member in enum_def:
        if(enum_member.find('=')!=-1):
            print(enum_member)
            ret_string += f"\t{enum_member.split('=')[0].strip()} = {enum_member.split('=')[1].strip()},\n"
        else:
            ret_string += f"\t{enum_member} = 0x{count:X},\n"
        count = count + 1
    ret_string+="};\n\n"
    return ret_string

def GenerateCppPredefinedClass(msg, predefined):
    ret_string = ""
    ret_string += f"typedef {msg['type']} {msg['name']};\n"
    
    return ret_string

def GenerateCppBaseClass(msg):
    #print(msg)
    ret_string = ""
    ret_string += f"PACKEDSTRUCT {msg['name']}\n"
    ret_string += "{\n"
    if(msg['name'] != "Header" and msg['name'] != "Footer"):
        ret_string += "\tHeader header;\n"
    for field in msg['fields']:
        ret_string += f"\t{field['type']} {field['name']};\n"
    if(msg['name'] != "Header" and msg['name'] != "Footer"):
        ret_string += "\tFooter footer;\n"
    ret_string += "};\n"
    return ret_string

def GenerateCppCustomClass(msg):
    ret_string = ""
    ret_string += f"PACKEDSTRUCT {msg['name']}\n"
    ret_string += "{\n"
    ret_string += "\tHeader header;\n"
    for field in msg['fields']:
        if field['type'].find('[')!=-1:
            ret_string += f"\t{field['type'].split('[')[0]} {field['name']}{field['type'][field['type'].find('['):]};\n"
        else:
            ret_string += f"\t{field['type'].split('[')[0]} {field['name']};\n"
    ret_string += "\tFooter footer;\n"
    ret_string += "};\n"
    return ret_string
    return ret_string

with open("AxisMessages.json", "r") as file:
    messages = json.load(file)

python_file = open("examples/Python/AxisMessages.py", "w")
cpp_file = open("firmware/include/AxisMessages.h", "w")

python_file.write(Python_Header)
cpp_file.write(CPP_Header)

#Create Message Defenitions
id = 0x00 # non zero start
cpp_file.write(f"enum class MessageTypes : uint16_t\n")
cpp_file.write("{\n")
for m in messages["MessageIds"]:
    python_file.write(f"{m} = 0x{id:X}\n")
    cpp_file.write(f"\t{m} = 0x{id:X},\n")
    id = id+1
cpp_file.write("};\n\n")
python_file.write("\n\n")

#Generate Enums
for enum in messages["Enums"]:
    python_file.write(GeneratePythonEnum(enum, messages["Enums"][enum]))
    cpp_file.write(GenerateCppEnum(enum, messages["Enums"][enum]))
#Generate Classes
predefined_messages = {}
for msg in messages["Messages"]:
    if(msg['type'] == 'Base'):
        python_file.write(GeneratePythonBaseClass(msg))
        cpp_file.write(GenerateCppBaseClass(msg))
        predefined_messages[msg['name']] = msg
    elif(msg['type'] == 'Custom'):
        python_file.write(GeneratePythonCustomClass(msg))
        cpp_file.write(GenerateCppCustomClass(msg))
    else:
        python_file.write(GeneratePythonPredefinedClass(msg, predefined_messages))
        cpp_file.write(GenerateCppPredefinedClass(msg, predefined_messages))


#Generate Setters
for msg_id in messages["MessageIds"]:
    for msg in messages["Messages"]: #disgusting and slow, I know
        msg_name = msg_id.replace("Get", "").replace("Set", "").replace("Id","")+"Message"
        if msg['name'] == msg_name:
            if(msg_id.find('Get')==-1):
                python_file.write(GeneratePythonSetter(msg_id, msg, predefined_messages))
            else:
                python_file.write(GeneratePythonGetter(msg_id, msg, predefined_messages))
        

python_file.close()
cpp_file.close()