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


Python_Header = """
from Axis import *
"""

def GetLen(type):
    print(type.split('[')[1])
    return 0

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
    print(size)
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

def GenerateBaseClass(msg):
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
        ret_string += f"\t\tret += self.{field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n"
    ret_string += "\t\treturn ret\n\n"

    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    counter = 0
    for field in msg['fields']:
        ret_string += f"\t\t{field['name']} = {ConvertCTypeToPythonType(field['type'])}.from_bytes(byte_array[{counter}:{GetBytes(field['type'])+counter}], 'little')\n"
        counter += GetBytes(field['type'])
    ret_string += "\t\treturn cls("
    ret_string += ", ".join([field['name'] for field in msg['fields']])
    ret_string += ")\n\n"

    return ret_string

def GeneratePredefinedClass(msg, predefined):
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

def GenerateCustomClass(msg):
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

    # serialize
    ret_string += "\tdef serialize(self):\n"
    ret_string += "\t\tret = bytearray(0)\n"
    for field in msg['fields']:
        if(field['type'].find('[')!=-1):
            ret_string += f"\t\tret += self.{field['name']}\n"
        else:
            ret_string += f"\t\tret += self.{field['name']}.to_bytes({GetBytes(field['type'])}, 'little')\n"
    ret_string += "\t\treturn ret\n\n"

    #deserialize
    ret_string += "\t@classmethod\n"
    ret_string += "\tdef deserialize(cls, byte_array):\n"
    counter = 0
    for field in msg['fields']:
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

def GenerateGetter(msg_name, msg_def, predefined):
    ret_string = ""
    ret_string += f"def {msg_name}(axis:Axis"
    
    # if(msg_def['type'] == 'Custom'):
    #     ret_string += f"{msg['name'].split('Message')[0]}Body"
    # elif msg_def['type'] != 'Base':
    #     ret_string += f"{msg_def['type']}"
    ret_string += ", timeout = 0.1):\n"
    ret_string += f"\tsend_msg = {msg_name.replace('Get', '').replace('Set', '')+'Message'}({msg_name}, 0)\n"
    
    ret_string += f"\taxis.send_message(send_msg.serialize())\n"
    ret_string += f"\tret = axis.wait_message(timeout)\n"
    ret_string += f"\tif(ret[0]=={msg_name}):\n"
    ret_string += f"\t\tmsg = {msg_name.replace('Get', '').replace('Set', '')+'Message'}.deserialize(ret)\n"
    ret_string += f"\t\treturn msg.body\n"
    ret_string += f"\telse:\n"
    ret_string += "\t\t return None\n"
    ret_string += "\n"
    
    return ret_string

def GenerateSetter(msg_name, msg_def, predefined):
    ret_string = ""
    ret_string += f"def {msg_name}(axis:Axis, body:"
    
    if(msg_def['type'] == 'Custom'):
        ret_string += f"{msg['name'].split('Message')[0]}Body"
    elif msg_def['type'] != 'Base':
        ret_string += f"{msg_def['type']}"
    ret_string += ", timeout = 0.1):\n"
    ret_string += f"\tsend_msg = {msg_name.replace('Get', '').replace('Set', '')+'Message'}({msg_name}, 0)\n"
    ret_string += f"\tsend_msg.body = body\n"
    ret_string += f"\taxis.send_message(send_msg.serialize())\n"
    ret_string += "\n"
    
    return ret_string

with open("interface.json", "r") as file:
    messages = json.load(file)

with open("examples/Python/automessages.py", "w") as file:
    file.write(Python_Header)
    
    #Create Message Defenitions
    id = 0x4F # non zero start
    for m in messages["MessageIds"]:
        file.write(f"{m} = 0x{id:X}\n")
        id = id+1
    file.write("\n\n")
    
    #Generate Classes
    predefined_messages = {}
    for msg in messages["Messages"]:
        if(msg['type'] == 'Base'):
            file.write(GenerateBaseClass(msg))
            predefined_messages[msg['name']] = msg
        elif(msg['type'] == 'Custom'):
            file.write(GenerateCustomClass(msg))
        else:
            file.write(GeneratePredefinedClass(msg, predefined_messages))
    
       
    #Generate Setters
    for msg_id in messages["MessageIds"]:
        for msg in messages["Messages"]: #disgusting and slow, I know
            msg_name = msg_id.replace("Get", "").replace("Set", "")+"Message"
            if msg['name'] == msg_name:
                if(msg_id.find('Set')==-1):
                    print(msg)
                    file.write(GenerateGetter(msg_id, msg, predefined_messages))
                else:
                    file.write(GenerateSetter(msg_id, msg, predefined_messages))
        