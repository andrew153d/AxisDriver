#pragma once
#include <Arduino.h>
#include "Task.h"
#include <AccelStepper.h>
#include "../LedController/LedController.h"
#include "MessageProcessor/MessageProcessor.h"
#include "easyTMC2209.h"
#include "wiring_private.h"
#include "pid.h"
#include "Messages.h"

#define SERIAL_PORT Serial1
#define R_SENSE 0.12f

enum ControlMode{
        MOTOR_OFF,
        DETECTSTEPS,
        TEST,
        TEST2,
        POSITION,
        VELOCITY,
        TORQUE,        
    };
union MotorError {
    struct {
        bool lostPower : 1;
        bool TMC_lost_comms : 1;
        uint32_t spares : 30;
    } bits;
    uint32_t errors;
};


class MotorController : public ITask, public IInternalInterface
{
private:
    HardwareSerial &serial_stream;
    easyTMC2209 driver;
    PIDController pid;
    AccelStepper stepper;

    ControlMode controlMode;
    //data that holds encoder data
    IEncoderInterface *encoder_ptr = nullptr;
    int step = 0;
    float target = 0;
    
    int error_flag;
    uint32_t start_millis;

    //error checking stuff
    MotorError motorErrors;
    MotorError previousErrors;
    uint32_t error_check_timer = 0;
    const uint32_t error_check_period = 1000;
    void CheckForErrors();

    uint8_t send_buffer[1024];
public:

    


    MotorController(uint32_t period) : serial_stream(Serial1),
                                       pid(10, 0.01, 0, 100000, 1000),
                                       stepper(stepper.DRIVER, MOTOR_STEP, MOTOR_DIR)
    {
        executionPeriod = period;
        driver = easyTMC2209();
    }

    void OnStart();
    void OnStop();
    void OnRun();
    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size);
    void setEncoderValueSource(IEncoderInterface *encoder_value);
    uint32_t GetErrors();

    void PrintErrorsToSerial();
};

extern MotorController motorController;