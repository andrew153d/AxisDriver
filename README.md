# AxisDriver #

# Interfaces #
- USB
- I2C
- Ethernet

# Messaging #

- The Axis Driver allows for two message sets to be used. On all interfaces, both the json and binary messages may be used interchangebly. The JSON messages allow for faster development and ease of use while the binary messages provide a high speed and effecient method of data transmission.
- All binary messages are made up of a header, body, and footer. All json messages have a header and body.

## Generic Messages ##

- The generic messages are used for most getters and setters, with unique messages being described in their specific device section.

<table>
<tr>
<td> </td> <td> Byte Messages </td> <td> JSON interface </td>
</tr>
<tr>
<td>
Header
</td>
<td> 

```
MessageTypes message_type
uint16_t body_size
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
}
```

</td>
</tr>
<!--  -->
<tr>
<td>
Footer
</td>
<td> 

```
uint16_t checksum
```
</td>
<td>

```
N/A
```

</td>
</tr>
<!--  -->
<tr>
<td>
Set Mode
</td>
<td> 

```
Header
uin8_t
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <mode>
}
```

</td>
</tr>
<!--  -->
<tr>
<td>
Set uint8_t
</td>
<td> 

```
Header
uin8_t
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>
<!--  -->
<tr>
<td>
Set int8_t
</td>
<td> 

```
Header
int8_t
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>

<!--  -->
<tr>
<td>
Set uint32_t
</td>
<td> 

```
Header
uint32_t
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>

<!--  -->
<tr>
<td>
Set int32_t
</td>
<td> 

```
Header
int32_t
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>

<!--  -->
<tr>
<td>
Set double
</td>
<td> 

```
Header
double
Footer
```
</td>
<td>

```
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>


</table>

## Device Specific Messages ##

### Settings and Configuration
---
#### Set Device Version
- Message Structure: uin32_t
- MessageType: 0x0111
- MessageTypeString: "GetVersion"
---
#### Set I2C Address
- Message Structure: uin8_t
- MessageType: 0x0500
- MessageTypeString: "SetI2CAddress"
---
#### Get I2C Address
- Message Structure: uint8_t
- MessageType: 0x0501
- MessageTypeString: "GetI2CAddress"
---
#### Set IP Address
- Message Structure: uint32_t
- MessageType: 0x0502
- MessageTypeString: "SetIPAddress"
---
#### Get IP Address
- Message Structure: uint32_t
- MessageType: 0x0503
- MessageTypeString: "GetIPAddress"
---
#### Set UDP Port
- Message Structure: uint32_t
- MessageType: 0x0504
- MessageTypeString: "SetUDPPort"
- **Values higher than 65535 are invalid**
---
#### Get UDP Port
- Message Structure: uint32_t
- MessageType: 0x0505
- MessageTypeString: "GetUDPPort"
---
#### Get MAC Address

- MessageType: 0x0506
- MessageTypeString: "GetMacAddress"

<table>
<tr>
<td>

```
Header
uint8_t[6]
Footer
```

</td>
<td>

```
{
  "MessageType": <MessageTypeString>,
  "Value": <mac address>
}
```

</td>
</tr>
</table>



### Led ###
---

#### Set Led Mode 
- Message Structure: Set Mode
- MessageType: 0x3001
- MessageTypeString: "SetLedMode"

The body of the set uint8_t message should contain the enum value for the mode, the body of the json message should contain the string equivalent.
##### Led Mode Enum
<table>
<tr>
<td>

```
  OFF = 0,
  FLASH_ERROR = 1,
  ERROR = 2,
  BOOTUP = 3,
  RAINBOW = 4,
  SOLID = 5,
```

</td>
<td>

```
  "OFF"
  "FLASH_ERROR"
  "ERROR"
  "BOOTUP"
  "RAINBOW"
  "SOLID"
```

</td>
</tr>
</table>

---

#### Set Led Color

- MessageType: 0x3002
- MessageTypeString: "SetLedColor"

<table>
<tr>
<td>

```
Header
uint8_t red;
uint8_t green;
uint8_t blue;
Footer
```

</td>
<td>

```
{
  "MessageType": <MessageTypeString>,
  "Red": <value>,
  "Green": <value>,
  "Blue": <value>
}
```

</td>
</tr>
</table>

---
#### Get Led Color

- MessageType: 0x3003
- MessageTypeString: "GetLedColor"

Values sent to the device in this message will be ignored, the returned message will contain the data in the respective fields.
<table>
<tr>
<td>

```
Header
uint8_t red;
uint8_t green;
uint8_t blue;
Footer
```

</td>
<td>

```
{
  "MessageType": <MessageTypeString>,
  "Red": <value>,
  "Green": <value>,
  "Blue": <value>
}
```

</td>
</tr>
</table>


