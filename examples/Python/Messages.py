# This file is auto-generated from firmware/include/Messages.h

class MessageTypes:
    GetVersion =  0x0002
    SetI2CAddress =  0x0500
    GetI2CAddress =  0x0501
    SetEthernetAddress =  0x0502
    GetEthernetAddress =  0x0503
    SetEthernetPort =  0x0504
    GetEthernetPort =  0x0505
    GetMacAddress =  0x0506
    SetLedState =  0x3001
    SetLedColor =  0x3002
    GetLedColor =  0x3003
    SetMotorState =  0x5000
    GetMotorState =  0x5001
    SetMotorBrake =  0x5004
    GetMotorBrake =  0x5005
    SetMaxSpeed =  0x5006
    GetMaxSpeed =  0x5007
    SetAcceleration =  0x5008
    GetAcceleration =  0x5009
    GetTargetPosition =  0x5010
    GetMotorVelocity =  0x5012
    GetMotorCurrent =  0x5013
    SetTargetPosition =  0x5014
    SetTargetPositionRelative =  0x5015
    SetMotorVelocity =  0x5016
    SetMotorCurrent =  0x5017

class MotorStates:
    OFF = 0
    POSITION = 1
    VELOCITY = 2
    IDLE = 3

class MotorBrake:
    NORMAL =  0
    FREEWHEELING =  1
    STRONG_BRAKING =  2
    BRAKING =  3
