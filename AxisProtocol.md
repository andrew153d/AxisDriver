# Axis Driver Protocol Documentation

This document describes the communication protocol for the Axis Driver firmware (Version 1.0.0.0).

## Protocol Overview

The protocol uses a structured message format with a header, body, and footer. All messages are packed structures without padding.

### Message Structure

All messages follow this basic structure:
- **Header** (4 bytes): Message type (2 bytes) + Body size (2 bytes)
- **Body** (variable): Message-specific data
- **Footer** (2 bytes): Checksum

### Data Types

- `uint8_t`: 1 byte unsigned integer
- `int8_t`: 1 byte signed integer
- `uint16_t`: 2 bytes unsigned integer
- `uint32_t`: 4 bytes unsigned integer
- `int32_t`: 4 bytes signed integer
- `double`: 8 bytes floating point

## Message Types

### 0x0100 - ACK (AckId)

**Description**: Acknowledgment message for successful command reception and execution.

| Byte Offset | Size | Field            | Description                                         |
| ----------- | ---- | ---------------- | --------------------------------------------------- |
| 0-1         | 2    | message_type     | 0x0100                                              |
| 2-3         | 2    | body_size        | 3                                                   |
| 4-5         | 2    | ack_message_type | Message type being acknowledged                     |
| 6           | 1    | status           | Status code (0=SUCCESS, 1=ERROR, 2=INVALID_COMMAND) |
| 7-8         | 2    | checksum         | Message checksum                                    |

**Status Codes:**
- 0x0: SUCCESS - Command executed successfully
- 0x1: ERROR - Command failed during execution
- 0x2: INVALID_COMMAND - Unknown or malformed command

### 0x0101 - Get Version (GetVersionId)

**Description**: Request firmware version information.

| Byte Offset | Size | Field        | Description                  |
| ----------- | ---- | ------------ | ---------------------------- |
| 0-1         | 2    | message_type | 0x0101                       |
| 2-3         | 2    | body_size    | 4                            |
| 4-7         | 4    | version      | Firmware version as uint32_t |
| 8-9         | 2    | checksum     | Message checksum             |


### 0x0102 - Set I2C Address (SetI2CAddressId)

**Description**: Set the I2C address for the device.

| Byte Offset | Size | Field        | Description         |
| ----------- | ---- | ------------ | ------------------- |
| 0-1         | 2    | message_type | 0x0102              |
| 2-3         | 2    | body_size    | 1                   |
| 4           | 1    | address      | I2C address (0-127) |
| 5-6         | 2    | checksum     | Message checksum    |

### 0x0103 - Get I2C Address (GetI2CAddressId)

**Description**: Request current I2C address.

| Byte Offset | Size | Field        | Description         |
| ----------- | ---- | ------------ | ------------------- |
| 0-1         | 2    | message_type | 0x0103              |
| 2-3         | 2    | body_size    | 1                   |
| 4           | 1    | address      | Current I2C address |
| 5-6         | 2    | checksum     | Message checksum    |

### 0x0104 - Set Ethernet Address (SetEthernetAddressId)

**Description**: Set the Ethernet IP address for the device.

| Byte Offset | Size | Field        | Description            |
| ----------- | ---- | ------------ | ---------------------- |
| 0-1         | 2    | message_type | 0x0104                 |
| 2-3         | 2    | body_size    | 4                      |
| 4-7         | 4    | ip_address   | IP address as uint32_t |
| 8-9         | 2    | checksum     | Message checksum       |

### 0x0105 - Get Ethernet Address (GetEthernetAddressId)

**Description**: Request current Ethernet IP address.

| Byte Offset | Size | Field        | Description                    |
| ----------- | ---- | ------------ | ------------------------------ |
| 0-1         | 2    | message_type | 0x0105                         |
| 2-3         | 2    | body_size    | 4                              |
| 4-7         | 4    | ip_address   | Current IP address as uint32_t |
| 8-9         | 2    | checksum     | Message checksum               |

### 0x0106 - Set Ethernet Port (SetEthernetPortId)

**Description**: Set the Ethernet port number for the device.

| Byte Offset | Size | Field        | Description             |
| ----------- | ---- | ------------ | ----------------------- |
| 0-1         | 2    | message_type | 0x0106                  |
| 2-3         | 2    | body_size    | 4                       |
| 4-7         | 4    | port         | Port number as uint32_t |
| 8-9         | 2    | checksum     | Message checksum        |

### 0x0107 - Get Ethernet Port (GetEthernetPortId)

**Description**: Request current Ethernet port number.

| Byte Offset | Size | Field        | Description                     |
| ----------- | ---- | ------------ | ------------------------------- |
| 0-1         | 2    | message_type | 0x0107                          |
| 2-3         | 2    | body_size    | 4                               |
| 4-7         | 4    | port         | Current port number as uint32_t |
| 8-9         | 2    | checksum     | Message checksum                |

### 0x0108 - Get MAC Address (GetMacAddressId)

**Description**: Request device MAC address.

| Byte Offset | Size | Field        | Description       |
| ----------- | ---- | ------------ | ----------------- |
| 0-1         | 2    | message_type | 0x0108            |
| 2-3         | 2    | body_size    | 6                 |
| 4-9         | 6    | mac_address  | MAC address bytes |
| 10-11       | 2    | checksum     | Message checksum  |

### 0x0109 - Save Configuration (SaveConfigurationId)

**Description**: Save current configuration to non-volatile memory.

| Byte Offset | Size | Field        | Description                |
| ----------- | ---- | ------------ | -------------------------- |
| 0-1         | 2    | message_type | 0x0109                     |
| 2-3         | 2    | body_size    | 1                          |
| 4           | 1    | save_flag    | 1 to save, 0 for no action |
| 5-6         | 2    | checksum     | Message checksum           |


### 0x0200 - Set LED Color (SetLedColorId)

**Description**: Set the LED color (RGB values).

| Byte Offset | Size | Field        | Description             |
| ----------- | ---- | ------------ | ----------------------- |
| 0-1         | 2    | message_type | 0x0200                  |
| 2-3         | 2    | body_size    | 3                       |
| 4           | 1    | red          | Red component (0-255)   |
| 5           | 1    | green        | Green component (0-255) |
| 6           | 1    | blue         | Blue component (0-255)  |
| 7-8         | 2    | checksum     | Message checksum        |


### 0x0201 - Get LED Color (GetLedColorId)

**Description**: Request current LED color.

| Byte Offset | Size | Field        | Description             |
| ----------- | ---- | ------------ | ----------------------- |
| 0-1         | 2    | message_type | 0x0201                  |
| 2-3         | 2    | body_size    | 3                       |
| 4           | 1    | red          | Current red component   |
| 5           | 1    | green        | Current green component |
| 6           | 1    | blue         | Current blue component  |
| 7-8         | 2    | checksum     | Message checksum        |


### 0x0202 - Add Led Step (AddLedStepId)

**Description**: Add a timed LED step with a duration and color. Useful for sequencing LED patterns step-by-step.

| Byte Offset | Size | Field        | Description                                |
| ----------- | ---- | ------------ | ------------------------------------------ |
| 0-1         | 2    | message_type | 0x0202                                     |
| 2-3         | 2    | body_size    | 7                                          |
| 4-7         | 4    | time_ms      | Duration/time in milliseconds (uint32_t)   |
| 8           | 1    | red          | Red component (0-255)                      |
| 9           | 1    | green        | Green component (0-255)                    |
| 10          | 1    | blue         | Blue component (0-255)                     |
| 11-12       | 2    | checksum     | Message checksum                           |


### 0x0300 - Set Home Direction (SetHomeDirectionId)

**Description**: Set the homing direction for the motor.

| Byte Offset | Size | Field        | Description                                      |
| ----------- | ---- | ------------ | ------------------------------------------------ |
| 0-1         | 2    | message_type | 0x0300                                           |
| 2-3         | 2    | body_size    | 1                                                |
| 4           | 1    | direction    | Home direction (0=CLOCKWISE, 1=COUNTERCLOCKWISE) |
| 5-6         | 2    | checksum     | Message checksum                                 |


### 0x0301 - Get Home Direction (GetHomeDirectionId)

**Description**: Request current homing direction.

| Byte Offset | Size | Field        | Description            |
| ----------- | ---- | ------------ | ---------------------- |
| 0-1         | 2    | message_type | 0x0301                 |
| 2-3         | 2    | body_size    | 1                      |
| 4           | 1    | direction    | Current home direction |
| 5-6         | 2    | checksum     | Message checksum       |


### 0x0302 - Set Home Threshold (SetHomeThresholdId)

**Description**: Set the threshold value for homing detection.

| Byte Offset | Size | Field        | Description                          |
| ----------- | ---- | ------------ | ------------------------------------ |
| 0-1         | 2    | message_type | 0x0302                               |
| 2-3         | 2    | body_size    | 4                                    |
| 4-7         | 4    | threshold    | Home detection threshold as uint32_t |
| 8-9         | 2    | checksum     | Message checksum                     |


### 0x0303 - Get Home Threshold (GetHomeThresholdId)

**Description**: Request current home threshold value.

| Byte Offset | Size | Field        | Description            |
| ----------- | ---- | ------------ | ---------------------- |
| 0-1         | 2    | message_type | 0x0303                 |
| 2-3         | 2    | body_size    | 4                      |
| 4-7         | 4    | threshold    | Current home threshold |
| 8-9         | 2    | checksum     | Message checksum       |


### 0x0304 - Set Home Speed (SetHomeSpeedId)

**Description**: Set the speed for homing operations.

| Byte Offset | Size | Field        | Description            |
| ----------- | ---- | ------------ | ---------------------- |
| 0-1         | 2    | message_type | 0x0304                 |
| 2-3         | 2    | body_size    | 4                      |
| 4-7         | 4    | speed        | Home speed as uint32_t |
| 8-9         | 2    | checksum     | Message checksum       |


### 0x0305 - Get Home Speed (GetHomeSpeedId)

**Description**: Request current homing speed.

| Byte Offset | Size | Field        | Description        |
| ----------- | ---- | ------------ | ------------------ |
| 0-1         | 2    | message_type | 0x0305             |
| 2-3         | 2    | body_size    | 4                  |
| 4-7         | 4    | speed        | Current home speed |
| 8-9         | 2    | checksum     | Message checksum   |


### 0x0306 - Get Homed State (GetHomedStateId)

**Description**: Request whether the motor has been homed.

| Byte Offset | Size | Field        | Description                |
| ----------- | ---- | ------------ | -------------------------- |
| 0-1         | 2    | message_type | 0x0306                     |
| 2-3         | 2    | body_size    | 1                          |
| 4           | 1    | homed        | 1 if homed, 0 if not homed |
| 5-6         | 2    | checksum     | Message checksum           |


### 0x0400 - Home (HomeId)

**Description**: Initiate homing sequence.

| Byte Offset | Size | Field        | Description        |
| ----------- | ---- | ------------ | ------------------ |
| 0-1         | 2    | message_type | 0x0400             |
| 2-3         | 2    | body_size    | 4                  |
| 4-7         | 4    | home_command | Home command value |
| 8-9         | 2    | checksum     | Message checksum   |


### 0x0307 - Set Motor State (SetMotorStateId)

**Description**: Set the motor control state/mode.

| Byte Offset | Size | Field        | Description                        |
| ----------- | ---- | ------------ | ---------------------------------- |
| 0-1         | 2    | message_type | 0x0307                             |
| 2-3         | 2    | body_size    | 1                                  |
| 4           | 1    | motor_state  | Motor state (see MotorStates enum) |
| 5-6         | 2    | checksum     | Message checksum                   |

**Motor States:**
- 0x0: OFF
- 0x1: POSITION
- 0x2: VELOCITY
- 0x3: VELOCITY_STEP
- 0x4: IDLE_ON
- 0x5: HOME


### 0x0308 - Get Motor State (GetMotorStateId)

**Description**: Request current motor state.

| Byte Offset | Size | Field        | Description         |
| ----------- | ---- | ------------ | ------------------- |
| 0-1         | 2    | message_type | 0x0308              |
| 2-3         | 2    | body_size    | 1                   |
| 4           | 1    | motor_state  | Current motor state |
| 5-6         | 2    | checksum     | Message checksum    |


### 0x0309 - Set Motor Brake (SetMotorBrakeId)

**Description**: Set the motor braking mode.

| Byte Offset | Size | Field        | Description                      |
| ----------- | ---- | ------------ | -------------------------------- |
| 0-1         | 2    | message_type | 0x0309                           |
| 2-3         | 2    | body_size    | 1                                |
| 4           | 1    | brake_mode   | Brake mode (see MotorBrake enum) |
| 5-6         | 2    | checksum     | Message checksum                 |

**Motor Brake Modes:**
- 0x0: NORMAL
- 0x1: FREEWHEELING
- 0x2: STRONG_BRAKING
- 0x3: BRAKING


### 0x030A - Get Motor Brake (GetMotorBrakeId)

**Description**: Request current motor brake mode.

| Byte Offset | Size | Field        | Description        |
| ----------- | ---- | ------------ | ------------------ |
| 0-1         | 2    | message_type | 0x030A             |
| 2-3         | 2    | body_size    | 1                  |
| 4           | 1    | brake_mode   | Current brake mode |
| 5-6         | 2    | checksum     | Message checksum   |


### 0x030B - Set Max Speed (SetMaxSpeedId)

**Description**: Set the maximum motor speed.

| Byte Offset | Size | Field        | Description               |
| ----------- | ---- | ------------ | ------------------------- |
| 0-1         | 2    | message_type | 0x030B                    |
| 2-3         | 2    | body_size    | 4                         |
| 4-7         | 4    | max_speed    | Maximum speed as uint32_t |
| 8-9         | 2    | checksum     | Message checksum          |


### 0x030C - Get Max Speed (GetMaxSpeedId)

**Description**: Request current maximum speed setting.

| Byte Offset | Size | Field        | Description           |
| ----------- | ---- | ------------ | --------------------- |
| 0-1         | 2    | message_type | 0x030C                |
| 2-3         | 2    | body_size    | 4                     |
| 4-7         | 4    | max_speed    | Current maximum speed |
| 8-9         | 2    | checksum     | Message checksum      |


### 0x030D - Set Acceleration (SetAccelerationId)

**Description**: Set the motor acceleration value.

| Byte Offset | Size | Field        | Description              |
| ----------- | ---- | ------------ | ------------------------ |
| 0-1         | 2    | message_type | 0x030D                   |
| 2-3         | 2    | body_size    | 4                        |
| 4-7         | 4    | acceleration | Acceleration as uint32_t |
| 8-9         | 2    | checksum     | Message checksum         |


### 0x030E - Get Acceleration (GetAccelerationId)

**Description**: Request current acceleration setting.

| Byte Offset | Size | Field        | Description          |
| ----------- | ---- | ------------ | -------------------- |
| 0-1         | 2    | message_type | 0x030E               |
| 2-3         | 2    | body_size    | 4                    |
| 4-7         | 4    | acceleration | Current acceleration |
| 8-9         | 2    | checksum     | Message checksum     |


### 0x030F - Set Current Position (SetCurrentPositionId)

**Description**: Set the current position (zero/reference the position).

| Byte Offset | Size | Field        | Description                   |
| ----------- | ---- | ------------ | ----------------------------- |
| 0-1         | 2    | message_type | 0x030F                        |
| 2-3         | 2    | body_size    | 8                             |
| 4-11        | 8    | position     | Position in degrees as double |
| 12-13       | 2    | checksum     | Message checksum              |


### 0x0310 - Get Current Position (GetCurrentPositionId)

**Description**: Request current motor position.

| Byte Offset | Size | Field        | Description                           |
| ----------- | ---- | ------------ | ------------------------------------- |
| 0-1         | 2    | message_type | 0x0310                                |
| 2-3         | 2    | body_size    | 8                                     |
| 4-11        | 8    | position     | Current position in degrees as double |
| 12-13       | 2    | checksum     | Message checksum                      |


### 0x0311 - Set Target Position (SetTargetPositionId)

**Description**: Set the target position for position control mode.

| Byte Offset | Size | Field        | Description                          |
| ----------- | ---- | ------------ | ------------------------------------ |
| 0-1         | 2    | message_type | 0x0311                               |
| 2-3         | 2    | body_size    | 8                                    |
| 4-11        | 8    | position     | Target position in degrees as double |
| 12-13       | 2    | checksum     | Message checksum                     |


### 0x0312 - Get Target Position (GetTargetPositionId)

**Description**: Request current target position.

| Byte Offset | Size | Field        | Description                                  |
| ----------- | ---- | ------------ | -------------------------------------------- |
| 0-1         | 2    | message_type | 0x0312                                       |
| 2-3         | 2    | body_size    | 8                                            |
| 4-11        | 8    | position     | Current target position in degrees as double |
| 12-13       | 2    | checksum     | Message checksum                             |


### 0x0401 - Set Relative Target Position (SetRelativeTargetPositionId)

**Description**: Set a target position relative to the current position.

| Byte Offset | Size | Field             | Description                            |
| ----------- | ---- | ----------------- | -------------------------------------- |
| 0-1         | 2    | message_type      | 0x0401                                 |
| 2-3         | 2    | body_size         | 8                                      |
| 4-11        | 8    | relative_position | Relative position in degrees as double |
| 12-13       | 2    | checksum          | Message checksum                       |


### 0x0313 - Set Velocity (SetVelocityId)

**Description**: Set the motor velocity for velocity control mode.

| Byte Offset | Size | Field        | Description                          |
| ----------- | ---- | ------------ | ------------------------------------ |
| 0-1         | 2    | message_type | 0x0313                               |
| 2-3         | 2    | body_size    | 8                                    |
| 4-11        | 8    | velocity     | Velocity in degrees/second as double |
| 12-13       | 2    | checksum     | Message checksum                     |


### 0x0314 - Get Velocity (GetVelocityId)

**Description**: Request current motor velocity.

| Byte Offset | Size | Field        | Description                                  |
| ----------- | ---- | ------------ | -------------------------------------------- |
| 0-1         | 2    | message_type | 0x0314                                       |
| 2-3         | 2    | body_size    | 8                                            |
| 4-11        | 8    | velocity     | Current velocity in degrees/second as double |
| 12-13       | 2    | checksum     | Message checksum                             |


### 0x0402 - Set Velocity and Steps (SetVelocityAndStepsId)

**Description**: Set velocity and number of steps with position mode.

| Byte Offset | Size | Field         | Description                            |
| ----------- | ---- | ------------- | -------------------------------------- |
| 0-1         | 2    | message_type  | 0x0402                                 |
| 2-3         | 2    | body_size     | 9                                      |
| 4-7         | 4    | velocity      | Velocity as int32_t                    |
| 8-11        | 4    | steps         | Number of steps as int32_t             |
| 12          | 1    | position_mode | Position mode (0=ABSOLUTE, 1=RELATIVE) |
| 13-14       | 2    | checksum      | Message checksum                       |


### 0x0403 - Start Path (StartPathId)

**Description**: Start executing a predefined motion path.

| Byte Offset | Size | Field        | Description                |
| ----------- | ---- | ------------ | -------------------------- |
| 0-1         | 2    | message_type | 0x0403                     |
| 2-3         | 2    | body_size    | 1                          |
| 4           | 1    | path_id      | Path identifier to execute |
| 5-6         | 2    | checksum     | Message checksum           |

## Checksum Calculation

The checksum is calculated as a 16-bit CRC or sum of all bytes in the header and body. The specific algorithm should be documented based on the firmware implementation.

## Error Handling

Messages with invalid checksums, unknown message types, or incorrect body sizes should be ignored or responded to with an error message.

## LLM Prompt for File Generation

Use the following prompt to instruct an LLM to generate implementation files based on this protocol documentation:

"Based on the Axis Driver Protocol Documentation provided in AxisProtocol.md, update firmware/include/AxisMessages.h, examples/Python/AxisProtocol.py, and web_configurator/AxisProtocol.js that implement the message structures, parsing, and serialization for the communication protocol. Include:

- Definitions for all message types with their exact byte offsets, sizes, and field names.
- Length definitions for each message type
- Enums for status codes, motor states, brake modes, etc.
- Update any existing functionality with any changes to the protocol

Ensure the generated code is compatible with the data types specified (uint8_t, int32_t, double, etc.) and follows the packed structure without padding. Provide complete, runnable code snippets or files that can be integrated into firmware or client applications."
