# This file is auto-generated from firmware/include/Messages.h

class MessageTypes:
    GetVersion = 0x0
    SetI2CAddress = 0x1
    GetI2CAddress = 0x2
    SetEthernetAddress = 0x3
    GetEthernetAddress = 0x4
    SetEthernetPort = 0x5
    GetEthernetPort = 0x6
    GetMacAddress = 0x7
    SaveSettings = 0x8
    SetLedState = 0x9
    SetLedColor = 0xa
    GetLedColor = 0xb
    SetHomeDirection = 0xc
    GetHomeDirection = 0xd
    Home = 0xe
    SetMotorState = 0xf
    GetMotorState = 0x10
    SetMotorBrake = 0x11
    GetMotorBrake = 0x12
    SetMaxSpeed = 0x13
    GetMaxSpeed = 0x14
    SetAcceleration = 0x15
    GetAcceleration = 0x16
    GetPosition = 0x17
    SetPosition = 0x18
    SetTargetPosition = 0x19
    GetTargetPosition = 0x1a
    SetTargetPositionRelative = 0x1b
    SetVelocity = 0x1c
    GetVelocity = 0x1d
    SetVelocityAndSteps = 0x1e
    StartPath = 0x1f

class MotorStates:
    OFF = 0x0
    POSITION = 0x1
    VELOCITY = 0x2
    VELOCITY_STEP = 0x3
    IDLE_ON = 0x4
    HOME = 0x5

class MotorBrake:
    NORMAL = 0x0
    FREEWHEELING = 0x1
    STRONG_BRAKING = 0x2
    BRAKING = 0x3

class HomeDirection:
    CLOCKWISE = 0x0
    COUNTERCLOCKWISE = 0x1
