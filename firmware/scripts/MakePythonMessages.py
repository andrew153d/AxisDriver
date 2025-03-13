import re
import os
# Path to the input and output files
input_file = os.path.join("firmware", "include", "Messages.h")
output_file = os.path.join("examples", "Python", "Messages.py")

# Regular expression to match the enum definitions
enum_pattern = re.compile(r'enum\s+class\s+(\w+)\s*:\s*\w+\s*{([^}]*)}', re.MULTILINE | re.DOTALL)

# Read the input file
with open(input_file, 'r') as f:
    content = f.read()

# Find all enum definitions
matches = enum_pattern.findall(content)
if not matches:
    raise ValueError("Could not find any enums in the input file")

# Write the Python versions of the enums
with open(output_file, 'w') as f:
    f.write("# This file is auto-generated from firmware/include/Messages.h\n")
    for enum_name, enum_body in matches:
        f.write(f"\nclass {enum_name}:\n")
        enum_members = [line.strip().strip(',') for line in enum_body.splitlines() if line.strip()]
        value_counter = 0
        for member in enum_members:
            if member.startswith('//'):
                continue
            if '=' in member:
                name, value = member.split('=')
                if 'x' in value:
                    value_counter = int(value.strip(), base=16)
                else:
                    value_counter = int(value.strip())
            else:
                name = member
                value = value_counter
            f.write(f"    {name.strip()} = 0x{int(value):x}\n")
            value_counter += 1

print(f"Python enums written to {output_file}")