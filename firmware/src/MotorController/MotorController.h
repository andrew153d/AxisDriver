#pragma once
#include <Arduino.h>
#include "Task/Task.h"
#include <AccelStepper.h>
#include "LedController/LedController.h"
#include "easyTMC2209.h"
#include "wiring_private.h"
#include "pid.h"
#include "Messages.h"
#include "DebugPrinter.h"

#define SERIAL_PORT Serial1
#define R_SENSE 0.12f

#define DEFAULT_HOMING_SPEED 800
#define DEFAULT_HOMING_DIRECTION 1

enum HomeState{
        RUN1,
        BACKUP,
        RUN2
    };
union MotorError {
    struct {
        bool lostPower : 1;
        bool TMC_lost_comms : 1;
        uint32_t spares : 30;
    } bits;
    uint32_t errors;
};

class MotorStep{
    public:
    

    MotorStep(){

    }


};

class MotorController : public ITask
{
private:
    HardwareSerial &serial_stream;
    easyTMC2209 driver;
    PIDController pid;
    public:
    AccelStepper stepper;
    
    MotorStates controlMode;
    HomeDirection homeDirection = HomeDirection::CLOCKWISE;
    MotorBrake motorBrake;
    String modeString = "";



    //data that holds encoder data
    IEncoderInterface *encoder_ptr = nullptr;
    int step = 0;
    bool hold = false;
    //targets and timers
    int target_position = 0;
    int target_velocity = 0;
    uint32_t target_velocity_duration = 0;
    uint32_t target_velocity_timer = 0;
    
    HomeState home_state_ = HomeState::RUN1;
    uint32_t homing_speed_ = DEFAULT_HOMING_SPEED;
    bool homing_direction = DEFAULT_HOMING_DIRECTION;
    int error_flag;
    uint32_t state_change_time_;

    //error checking stuff
    MotorError motorErrors;
    MotorError previousErrors;
    uint32_t error_check_timer = 0;
    const uint32_t error_check_period = 1000;
    void CheckForErrors();

    uint8_t send_buffer[1024];
    JsonDocument received_json;
    //ControlMode GetModeFromString(String mode);

    long degreesToSteps(double degrees);  
    double stepsToDegrees(long steps);

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

    void SetMotorState(MotorStates state);
    MotorStates GetMotorState();

    void SetMotorBraking(MotorBrake brake);
    MotorBrake GetMotorBraking();

    void SetMaxSpeed(uint32_t spd);
    uint32_t GetMaxSpeed();

    void SetAcceleration(uint32_t acl);
    uint32_t GetAcceleration();

    void SetPositionTarget(double position);
    void SetPositionTargetRelative(double position);
    double GetPositionTarget();

    void SetVelocityTarget(double velocity);
    double GetVelocityTarget();
    
    void setEncoderValueSource(IEncoderInterface *encoder_value);
    uint32_t GetErrors();

    void SetHomeDirection(HomeDirection direction);
    HomeDirection GetHomeDirection();
    void Home();

    void PrintErrorsToSerial();
};

extern MotorController motorController;