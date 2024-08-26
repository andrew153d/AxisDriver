#pragma once
#include <Arduino.h>
#include "Task.h"
#include <Tlv493d.h>

class EncoderController : public ITask
{
private:
static const uint32_t device_address = 0x5E;

float number_full_turns = 0;
float home_position_offset = 0;
float last_full_shaft_position = 0, full_shaft_position = 0;
float shaft_velocity = 0;
//testing using an averaging filter
static const uint32_t average_filter_length = 100;
float average_filter[average_filter_length] = {};
uint8_t filter_write_pos = 0;

//testing using a sliding window filter
float sliding_window_center = 0, prev_sliding_window_center;
float sliding_window_radius = 0.5; //amplitude of noise when sensor is still
float sliding_window_alpha = 0.02;


float raw_shaft_angle = 0;
float smoothed_shaft_angle;
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

public:
    EncoderController(uint32_t period)
    {
        executionPeriod = period;
    }

    void OnStart();
    void OnStop();
    void OnRun();

    float GetShaftAngle();
    float GetRunningAverageShaftAngle();
    float GetSlidingWindowShaftAngle();
    float GetSpeedDegPerS();
    float* GetShaftAnglePtr();
};