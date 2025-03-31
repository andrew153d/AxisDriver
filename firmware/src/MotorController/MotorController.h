#pragma once
#include <Arduino.h>
#include "Task/Task.h"
#include "MotorController/AccelStepper.h"
#include "LedController/LedController.h"
#include "easyTMC2209.h"
#include "wiring_private.h"
#include "pid.h"
#include "AxisMessages.h"
#include "DebugPrinter.h"

#define SERIAL_PORT Serial1
#define R_SENSE 0.12f

#define DEFAULT_HOMING_SPEED 100
#define DEFAULT_HOMING_DIRECTION 1
#define DEFAULT_HOMING_THRESHOLD 2

#define US_PER_SEC 1000000
#define TIMER_FREQ 10000 // ticks per second
#define TIMER_PERIOD (1.0/TIMER_FREQ) // 1ms period
constexpr uint32_t TIMER_COUNT = (48000000 / 8) * TIMER_PERIOD;
#define US_TO_TIMER_COUNT(us) (48000000 / 8) * (us) / 1000000


#define TICKS_PER_US TIMER_FREQ / US_PER_SEC

static_assert(TIMER_COUNT > 0 && TIMER_COUNT < 0xFFFF, "TIMER_COUNT must be greater than 0 and less than 0xFFFF");

#define DOUBLE_BUF_SIZE 1024


enum HomeState
{
    RUN1,
    BACKUP,
    RUN2
};

union MotorError
{
    struct
    {
        bool lostPower : 1;
        bool TMC_lost_comms : 1;
        uint32_t spares : 30;
    } bits;
    uint32_t errors;
};

class MotorStep
{
public:
    MotorStep()
    {
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
    MotorBrake motorBrake;
    String modeString = "";

    //Velocity Step variables
    struct VelocityStep
    {
        PositionMode positionMode;
        int32_t velocity;
        int32_t step;
    };
    std::deque<VelocityStep> velocity_steps;
    long velocity_step_end = 0;

    // data that holds encoder data
    IEncoderInterface *encoder_ptr = nullptr;
    int step = 0;
    bool hold = false;

    // targets and timers
    int target_position = 0;
    int target_velocity = 0;
    uint32_t target_velocity_duration = 0;
    uint32_t target_velocity_timer = 0;

    HomeState home_state_ = HomeState::RUN1;
    uint32_t homing_speed_ = DEFAULT_HOMING_SPEED;
    bool homing_direction = DEFAULT_HOMING_DIRECTION;
    uint16_t homing_threshold = DEFAULT_HOMING_THRESHOLD;
    
    int error_flag;
    uint32_t state_change_time_;

    uint8_t send_buffer[1024];

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
    void OnTimer();

    void SetMotorState(MotorStates state);
    MotorStates GetMotorState();

    void SetMotorBraking(MotorBrake brake);
    MotorBrake GetMotorBraking();

    void SetMaxSpeed(uint32_t spd);
    uint32_t GetMaxSpeed();

    void SetAcceleration(uint32_t acl);
    uint32_t GetAcceleration();

    void SetPosition(double position);
    double GetPosition();

    void SetPositionTarget(double position);
    void SetPositionTargetRelative(double position);
    double GetPositionTarget();

    void SetVelocityTarget(double velocity);
    double GetVelocityTarget();

    void AddVelocityStep(int32_t velocity, int32_t step, uint8_t position_mode);
    void StartPath();

    void setEncoderValueSource(IEncoderInterface *encoder_value);
    uint32_t GetErrors();

    void SetHomeDirection(HomeDirection direction);
    HomeDirection GetHomeDirection();
    void SetHomingSpeed(uint32_t speed);
    uint32_t GetHomingSpeed();
    void SetHomeThreshold(uint16_t threshold);
    uint16_t GetHomeThreshold();
    void Home();
    

    void PrintErrorsToSerial();
};

extern MotorController motorController;