# This file is auto-generated from firmware/include/Messages.h

class MessageTypes:
    GetVersion = 0
    SetI2CAddress = 1
    GetI2CAddress = 2
    SetEthernetAddress = 3
    GetEthernetAddress = 4
    SetEthernetPort = 5
    GetEthernetPort = 6
    GetMacAddress = 7
    SaveSettings = 8
    SetLedState = 9
    SetLedColor = 10
    GetLedColor = 11
    SetHomeDirection = 12
    GetHomeDirection = 13
    Home = 14
    SetMotorState = 15
    GetMotorState = 16
    SetMotorBrake = 17
    GetMotorBrake = 18
    SetMaxSpeed = 19
    GetMaxSpeed = 20
    SetAcceleration = 21
    GetAcceleration = 22
    SetTargetPosition = 23
    GetTargetPosition = 24
    SetTargetPositionRelative = 25
    SetMotorVelocity = 26
    GetMotorVelocity = 27
    SetVelocityAndSteps = 28
    StartPath = 29

class MotorStates:
    OFF = 0
    POSITION = 1
    VELOCITY = 2
    VELOCITY_STEP = 3
    IDLE_ON = 4
    HOME = 5

class MotorBrake:
    NORMAL =  0
    FREEWHEELING =  1
    STRONG_BRAKING =  2
    BRAKING =  3

class HomeDirection:
    CLOCKWISE =  0
    COUNTERCLOCKWISE =  1
