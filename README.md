# AxisDriver #

# Interfaces #
- USB
- I2C

# Messaging #

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
</table>
