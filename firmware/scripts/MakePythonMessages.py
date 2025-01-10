import re
import os

# Path to the input and output files
input_file = os.path.join("firmware", "include", "Messages.h")
output_file = os.path.join("examples", "Python", "Messages.py")

# Regular expression to match the enum definition
enum_pattern = re.compile(r'enum\s+class\s+MessageTypes\s*:\s*uint16_t\s*{([^}]*)}', re.MULTILINE | re.DOTALL)

# Read the input file
with open(input_file, 'r') as f:
    content = f.read()

# Find the enum definition
match = enum_pattern.search(content)
if not match:
    raise ValueError("Could not find MessageTypes enum in the input file")

# Extract the enum members
enum_body = match.group(1)
enum_members = [line.strip().strip(',') for line in enum_body.splitlines() if line.strip()]

# Write the Python version of the enum
with open(output_file, 'w') as f:
    f.write("# This file is auto-generated from firmware/include/Messages.h\n")
    #f.write("from enum import Enum\n\n")
    f.write("class MessageTypes:\n")
    for member in enum_members:
        if member.startswith('//'):
            continue
        name, value = member.split('=')
        f.write(f"    {name.strip()} = {value.strip()}\n")

print(f"Python enum written to {output_file}")