#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define nullptr NULL

#define US_PER_SEC 1000000
#define TIMER_FREQ 10000
#define TIMER_COUNT (48000000 / 8 / TIMER_FREQ)
#define TICKS_PER_US TIMER_FREQ / US_PER_SEC
#define DOUBLE_BUF_SIZE 4096



/// The current interval between steps in microseconds.
/// 0 means the motor is currently stopped with _speed == 0
unsigned long _stepInterval;
/// The last step time in microseconds
unsigned long _lastStepTime;
/// The current absolution position in steps.
long _currentPos; // Steps
/// Last step size in microseconds
float _cn;
/// The target position in steps. The AccelStepper library will move the
/// motor from the _currentPos to the _targetPos, taking into account the
/// max speed, acceleration and deceleration
long _targetPos; // Steps
/// The current motos speed in steps per second
/// Positive is clockwise
float _speed; // Steps per second
/// The step counter for speed calculations
long _step_counter;
/// The acceleration to use to accelerate or decelerate the motor in steps
/// per second per second. Must be > 0
float _acceleration;    
/// The maximum permitted speed in steps per second. Must be > 0.
float          _maxSpeed;
/// Initial step size in microseconds
float _c0;
/// Min step size in microseconds based on maxSpeed
float _cmin; // at max speed
/// \brief Direction indicator
/// Symbolic names for the direction the motor is turning
typedef enum
{
    DIRECTION_CCW = 0, ///< Counter-Clockwise
    DIRECTION_CW = 1   ///< Clockwise
} Direction;
/// Called to execute a step on a 4 pin half-stepper motor. Only called when a new step is
/// required. Subclasses may override to implement new stepping
/// interfaces. The default sinterfaces spinning in
/// Protected because some peoples subclasses need it to be so
bool _direction; // 1 == CW

unsigned long start_micros;
unsigned long micros()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long)(ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

long distanceToGo()
{
    return _targetPos - _currentPos;
}
// Subclasses can override
unsigned long computeNewSpeed()
{
    long distanceTo = distanceToGo(); // +ve is clockwise from curent location

    long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16

    if (distanceTo == 0 && stepsToStop <= 1)
    {
        // We are at the target and its time to stop
        _stepInterval = 0;
        _speed = 0.0;
        _step_counter = 0;
        return _stepInterval;
    }

    if (distanceTo > 0)
    {
        // We are anticlockwise from the target
        // Need to go clockwise from here, maybe decelerate now
        if (_step_counter > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if ((stepsToStop >= distanceTo) || _direction == DIRECTION_CCW)
                _step_counter = -stepsToStop; // Start deceleration
        }
        else if (_step_counter < 0)
        {
            // Currently decelerating, need to accel again?
            if ((stepsToStop < distanceTo) && _direction == DIRECTION_CW)
                _step_counter = -_step_counter; // Start accceleration
        }
    }
    else if (distanceTo < 0)
    {
        // We are clockwise from the target
        // Need to go anticlockwise from here, maybe decelerate
        if (_step_counter > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if ((stepsToStop >= -distanceTo) || _direction == DIRECTION_CW)
                _step_counter = -stepsToStop; // Start deceleration
        }
        else if (_step_counter < 0)
        {
            // Currently decelerating, need to accel again?
            if ((stepsToStop < -distanceTo) && _direction == DIRECTION_CCW)
                _step_counter = -_step_counter; // Start accceleration
        }
    }

    // Need to accelerate or decelerate
    if (_step_counter == 0)
    {
        // First step from stopped
        _cn = _c0;
        _direction = (distanceTo > 0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        _cn = _cn - ((2.0 * _cn) / ((4.0 * _step_counter) + 1)); // Equation 13
        _cn = (_cn > _cmin) ? _cn : _cmin;
    }
    _step_counter++;
    _stepInterval = _cn;
    _speed = 1000000.0 / _cn;
    if (_direction == DIRECTION_CCW)
        _speed = -_speed;

    return _stepInterval;
}


void setup_stepper()
{
    _targetPos = 200;
    _acceleration = 2000;
    _maxSpeed = 1000;

    start_micros = micros();

    _c0 = 0.676 * sqrt(2.0 / _acceleration) * 1000000.0;
    _cmin = 1000000.0 / _maxSpeed;
    computeNewSpeed();
}


uint8_t buffer1[DOUBLE_BUF_SIZE];
uint8_t buffer2[DOUBLE_BUF_SIZE];
uint8_t *buffer_to_update = nullptr;

void FillBuffer()
{
    if (buffer_to_update != nullptr)
    {
        // need to put some data into the buffer

        uint8_t *start_ptr = buffer_to_update;
        uint8_t *end_ptr = buffer_to_update + DOUBLE_BUF_SIZE;
        uint8_t *ptr = start_ptr;

        memset(start_ptr, 0, DOUBLE_BUF_SIZE);

        float ptr_inc = 0;

        while (ptr < end_ptr)
        {
            _currentPos++;
            unsigned long interval = computeNewSpeed();
            printf("Interval: %lu\n", interval);
            if(interval==0)
            break;
            ptr++;
        }

        for (uint8_t *p = start_ptr; p < end_ptr; p++)
        {
            /// Serial.print(*p);
            // Serial.print(" ");
        }
        // Serial.println();
        buffer_to_update = nullptr;
    }
}

int main()
{
    setup_stepper();
    buffer_to_update = &buffer1[0];
    FillBuffer();
    FillBuffer();

    FillBuffer();

    FillBuffer();
    // for (int i = 0; i < DOUBLE_BUF_SIZE; i++)
    // {
    //     printf("%d ", buffer1[i]);
    // }
    // printf("\n");
    return 0;
}