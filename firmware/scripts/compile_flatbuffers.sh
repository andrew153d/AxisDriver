#!/bin/bash

# set a cpp destination directory
SCRIPT_DIR=$(dirname "$0")
CPP_DESTINATION=$SCRIPT_DIR/../firmware/include/FlatBuffers
PYTHON_DESTINATION=$SCRIPT_DIR/../../examples/Python/


flatc --cpp -o $CPP_DESTINATION flatbuffers/AxisMessages_common.fbs
flatc --cpp -o $CPP_DESTINATION flatbuffers/AxisMessages_ha.fbs
flatc --cpp -o $CPP_DESTINATION flatbuffers/AxisMessages_led.fbs
flatc --cpp -o $CPP_DESTINATION flatbuffers/AxisMessages_motor.fbs
flatc --cpp -o $CPP_DESTINATION flatbuffers/AxisMessages_settings.fbs


flatc --python -o $PYTHON_DESTINATION flatbuffers/AxisMessages_common.fbs
flatc --python -o $PYTHON_DESTINATION flatbuffers/AxisMessages_ha.fbs
flatc --python -o $PYTHON_DESTINATION flatbuffers/AxisMessages_led.fbs
flatc --python -o $PYTHON_DESTINATION flatbuffers/AxisMessages_motor.fbs
flatc --python -o $PYTHON_DESTINATION flatbuffers/AxisMessages_settings.fbs
