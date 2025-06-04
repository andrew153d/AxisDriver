#!/bin/bash

# set a cpp destination directory
SCRIPT_DIR=$(dirname "$0")
CPP_DESTINATION=$SCRIPT_DIR/../../firmware/include/protobuf
PYTHON_DESTINATION=$SCRIPT_DIR/../../examples/Python/protobuf

#make the directories if they do not exist
mkdir -p $CPP_DESTINATION
mkdir -p $PYTHON_DESTINATION

protoc --proto_path=protobuf --cpp_out=$CPP_DESTINATION protobuf/AxisMessages_common.proto
protoc --proto_path=protobuf --cpp_out=$CPP_DESTINATION protobuf/AxisMessages_ha.proto
protoc --proto_path=protobuf --cpp_out=$CPP_DESTINATION protobuf/AxisMessages_led.proto
protoc --proto_path=protobuf --cpp_out=$CPP_DESTINATION protobuf/AxisMessages_motor.proto
protoc --proto_path=protobuf --cpp_out=$CPP_DESTINATION protobuf/AxisMessages_settings.proto

protoc --proto_path=protobuf --python_out=$PYTHON_DESTINATION protobuf/AxisMessages_common.proto
protoc --proto_path=protobuf --python_out=$PYTHON_DESTINATION protobuf/AxisMessages_ha.proto
protoc --proto_path=protobuf --python_out=$PYTHON_DESTINATION protobuf/AxisMessages_led.proto
protoc --proto_path=protobuf --python_out=$PYTHON_DESTINATION protobuf/AxisMessages_motor.proto
protoc --proto_path=protobuf --python_out=$PYTHON_DESTINATION protobuf/AxisMessages_settings.proto
