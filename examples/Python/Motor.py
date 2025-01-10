import Comms



Comms.SetU8(0x0500, 0xAD)
ret = Comms.GetU8(0x0501)
if ret is not None:
    print(ret[8:10].decode())
else:
    print("Failed to get i2c address")