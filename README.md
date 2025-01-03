# AxisDriver #

# Interfaces #
- USB
- I2C

# Messaging #

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

```json
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

```json
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

```json
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

```json
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

```json
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

```json
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

```json
{
    "MessageType": <MessageTypeString>,
    "Value": <value>
}
```

</td>
</tr>


</table>

## Device Specific Messages ##
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

```json
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

```json
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


