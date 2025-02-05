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
    SetLedState = 8
    SetLedColor = 9
    GetLedColor = 10
    SetHomeDirection = 11
    GetHomeDirection = 12
    Home = 13
    SetMotorState = 14
    GetMotorState = 15
    SetMotorBrake = 16
    GetMotorBrake = 17
    SetMaxSpeed = 18
    GetMaxSpeed = 19
    SetAcceleration = 20
    GetAcceleration = 21
    SetTargetPosition = 22
    GetTargetPosition = 23
    SetTargetPositionRelative = 24
    SetMotorVelocity = 25
    GetMotorVelocity = 26

class MotorStates:
    OFF = 0
    POSITION = 1
    VELOCITY = 2
    IDLE_ON = 3
    HOME = 4

class MotorBrake:
    NORMAL =  0
    FREEWHEELING =  1
    STRONG_BRAKING =  2
    BRAKING =  3

class HomeDirection:
    CLOCKWISE =  0
    COUNTERCLOCKWISE =  1
