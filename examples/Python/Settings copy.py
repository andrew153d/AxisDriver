
from AxisMessages import *
from Axis import *

from protobuf import AxisMessages_common_pb2

msg = AxisMessages_common_pb2.Header()
msg.body_size = 1
msg.sync_bytes = 34
msg.message_type = 25
print(msg.SerializeToString())
