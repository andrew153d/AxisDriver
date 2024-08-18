#pragma once
#include <Arduino.h>
#include "Task.h"
#include <AccelStepper.h>
#include "easyTMC2209.h"
#include "wiring_private.h"
#include "pid.h"

#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.12f

class MotorController : public ITask
{
private:
    HardwareSerial &serial_stream;
    easyTMC2209 driver;
    PIDController pid;
    AccelStepper stepper;
    float *encoder_ptr = nullptr;
    int step = 0;
    float target = 0;
public:
    MotorController(uint32_t period) : serial_stream(Serial1),
                                       pid(50, 0.01, 0, 100000, 100000),
                                       stepper(stepper.DRIVER, MOTOR_STEP, MOTOR_DIR)
    {
        executionPeriod = period;
        driver = easyTMC2209();
    }

    void OnStart();
    void OnStop();
    void OnRun();

    void setEncoderValueSource(float *encoder_value);

    // functions needed by simple foc
};