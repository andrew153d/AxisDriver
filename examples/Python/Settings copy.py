
import flatbuffers
import AxisDriver.VersionMessage as VersionMessage
import AxisDriver.MessageId as MessageId
import AxisDriver.Header as Header
import AxisDriver.Footer as Footer
import AxisDriver.HeaderStruct as HeaderStruct
import time
from Axis import AxisSerial
import struct


# Build the Body (VersionMessage)
builder = flatbuffers.Builder(0)
VersionMessage.VersionMessageStart(builder)
VersionMessage.VersionMessageAddMajor(builder, 1)  # Major version
VersionMessage.VersionMessageAddMinor(builder, 2)  # Minor version
VersionMessage.VersionMessageAddPatch(builder, 5)  # Patch version
ver_msg = VersionMessage.VersionMessageEnd(builder)
builder.Finish(ver_msg)
body_buf = builder.Output()


# Build the Header
builder = flatbuffers.Builder(0)
Header.HeaderStart(builder)
Header.HeaderAddSyncBytes(builder, 0xDEADBABE)  # Sync bytes
Header.HeaderAddMessageType(builder, MessageId.MessageId.GetVersionId)  # Message type
header_offset = Header.HeaderEnd(builder)
builder.Finish(header_offset)
header_buf = builder.Output()

# Pack with 4-byte size prefixes
def pack_with_size(buf):
	return struct.pack('<I', len(buf)) + buf

header_packed = pack_with_size(header_buf)
body_packed = pack_with_size(body_buf)

# Footer is calculated over header+body
footer_input = header_packed + body_packed
builder = flatbuffers.Builder(0)
Footer.FooterStart(builder)
Footer.FooterAddChecksum(builder, sum(footer_input) % 0xFFFF)
footer_offset = Footer.FooterEnd(builder)
builder.Finish(footer_offset)
footer_buf = builder.Output()

# Final message: [header_size][header][body_size][body][footer]
final_message = header_packed + body_packed + footer_buf

print("\nGetVersion FlatBuffer message:\n", final_message)

SerialComms = AxisSerial('/dev/ttyACM1')
SerialComms.send_message(final_message)
time.sleep(1)


