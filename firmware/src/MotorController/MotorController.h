#pragma once
#include <Arduino.h>
#include "Task.h"
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "wiring_private.h"

#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b11 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.12f

class MotorController : public ITask
{
private:
    HardwareSerial &serial_stream;
    AccelStepper stepper;
    TMC2209Stepper driver;

public:
    MotorController(uint32_t period) : serial_stream(Serial1),
                                       stepper(stepper.DRIVER, MOTOR_STEP, MOTOR_DIR),
                                       driver(&serial_stream, R_SENSE, DRIVER_ADDRESS)
    {
        executionPeriod = period;
    }

    void OnStart();

    void OnStop();

    void OnRun();
};