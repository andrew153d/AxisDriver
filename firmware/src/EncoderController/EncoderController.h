#pragma once
#include <Arduino.h>
#include "Task/Task.h"
#include <Tlv493d.h>
#include "Messages.h"
#include "MessageProcessor/MessageProcessor.hpp"
class EncoderController : public ITask, public IEncoderInterface
{
private:
    static const uint32_t device_address = 0x5E;

    float number_full_turns = 0;
    float home_position_offset = 0;
    float last_full_shaft_position = 0, full_shaft_position = 0;
    float shaft_velocity = 0;
    // testing using an averaging filter
    static const uint32_t average_filter_length = 100;
    float average_filter[average_filter_length] = {};
    uint8_t filter_write_pos = 0;

    // testing using a sliding window filter
    float sliding_window_center = 0, prev_sliding_window_center;
    float sliding_window_radius = 0.5; // amplitude of noise when sensor is still
    float sliding_window_alpha = 0.02;

    float raw_shaft_angle = 0;
    float smoothed_shaft_angle;
    Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

    JsonDocument recvd_json;

    // update rate tracking
    const uint32_t update_rate_poll_period = 2000;
    uint32_t update_rate_timer = 0;
    long update_rate_ticker = 0;
    float update_rate;

public:
    EncoderController(uint32_t period)
    {
        executionPeriod = period;
    }

    void OnStart();
    void OnStop();
    void OnRun();

    // messageHandlers
    void HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);
    void HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size);

    float GetVelocityDegreesPerSecond();
    float GetPositionDegrees();

    float GetShaftAngle();
    float GetRunningAverageShaftAngle();
    float GetSlidingWindowShaftAngle();
    float GetSpeedDegPerS();
    float *GetShaftAnglePtr();

    void SetPosition(float position);
    float GetUpdateRate();
};

extern EncoderController encoderController;