#include "EncoderController.h"
#include "DebugPrinter.h"
float Wrap0to360(float input)
{
    input = fmodf(input, 360.0f);
    if (input < 0)
    {
        input += 360.0f;
    }
    return input;
}

void EncoderController::OnStart()
{
    Wire1.setClock(400000);
    Wire1.begin();
    Wire1.beginTransmission(device_address);
    byte error = Wire1.endTransmission();
    if (error)
    {
        Serial.println("Failed to find Encoder");

        addrLedController.AddLedStep(CRGB::Red, 100);
        addrLedController.AddLedStep(CRGB::Black, 100);
    }
    else
    {
        Serial.println("Can communicate with Encoder");

        addrLedController.AddLedStep(0x222200, 100);
        addrLedController.AddLedStep(CRGB::Black, 100);
    }

    Tlv493dMagnetic3DSensor.begin(Wire1);
    Tlv493dMagnetic3DSensor.setAccessMode(Tlv493d::AccessMode_e::MASTERCONTROLLEDMODE);
    Tlv493dMagnetic3DSensor.disableTemp();
    Tlv493dMagnetic3DSensor.updateData();
    home_position_offset = Wrap0to360(degrees(Tlv493dMagnetic3DSensor.getAzimuth()));
}

void EncoderController::OnStop()
{
}

void EncoderController::OnRun()
{
    if (Tlv493dMagnetic3DSensor.getMeasurementDelay() != executionPeriod)
    {
        executionPeriod = Tlv493dMagnetic3DSensor.getMeasurementDelay();
    }

    long m = micros();
    Tlv493d_Error_t err = Tlv493dMagnetic3DSensor.updateData();
    long t = micros();
    // DEBUG_PRINTF("Micros:%lu\n", t-m);
    if (err != Tlv493d_Error_t::TLV493D_NO_ERROR)
    {
        Serial.printf("Encoder Error: %d\n", err);
    }

    raw_shaft_angle = Wrap0to360(degrees(Tlv493dMagnetic3DSensor.getAzimuth()));
    // round to 1 decimal place
    raw_shaft_angle = roundf(raw_shaft_angle * 10.0f) / 10.0f;
    // Serial.printf("%8.5f\n", raw_shaft_angle);
    //  load the sliding window filter
    //  to avoid invalid values at the ends, apply an offset

    {
        float offset = 180 - raw_shaft_angle;
        sliding_window_center += offset;
        raw_shaft_angle += offset;

        if (sliding_window_center - raw_shaft_angle > sliding_window_radius)
        { // center > shaft_angle
            sliding_window_center = raw_shaft_angle + sliding_window_radius;
        }
        else if (raw_shaft_angle - sliding_window_center > sliding_window_radius)
        { // shaft_angle > center
            sliding_window_center = raw_shaft_angle - sliding_window_radius;
        }
        else
        {
            sliding_window_center = sliding_window_alpha * raw_shaft_angle + (1 - sliding_window_alpha) * sliding_window_center;
        }

        sliding_window_center -= offset;
        raw_shaft_angle -= offset;
    }

    // keep track of the full turns
    float delta = sliding_window_center - prev_sliding_window_center;
    if (delta < -180)
    {
        number_full_turns++;
    }

    if (delta > 180)
    {
        number_full_turns--;
    }

    full_shaft_position = Wrap0to360(sliding_window_center) + (360 * number_full_turns) + home_position_offset;
    prev_sliding_window_center = sliding_window_center;

    float vel = (full_shaft_position - last_full_shaft_position) / ((float)(micros() - last_read_time) / 1000000);
    shaft_velocity = vel * velocity_alpha + shaft_velocity * (1 - velocity_alpha);
    // DEBUG_PRINTF("%8.1f %8.1f %8.1f %lu \n", shaft_velocity, full_shaft_position, last_full_shaft_position, micros()-last_read_time);
    //DEBUG_PRINTF("%8.1f %8.1f %8.1f %8.1f %8.1f\n", sliding_window_center, delta, number_full_turns, full_shaft_position, shaft_velocity);

    last_full_shaft_position = full_shaft_position;

    update_rate_ticker++;

    if (millis() - update_rate_timer > update_rate_poll_period)
    {
        update_rate_timer = millis();
        update_rate = (float)update_rate_ticker * (1000 / (float)update_rate_poll_period);
        update_rate_ticker = 0;
    }

    last_read_time = micros();
}

float EncoderController::GetShaftAngle()
{
    return full_shaft_position;
}

float EncoderController::GetVelocityDegreesPerSecond()
{
    return shaft_velocity;
}

float EncoderController::GetPositionDegrees()
{
    return full_shaft_position;
}

float *EncoderController::GetShaftAnglePtr()
{
    return &full_shaft_position;
}

void EncoderController::SetPosition(float position)
{
    home_position_offset += (position - GetShaftAngle());

    // Serial.printf("raw %f, offset %f, home %f \n", GetShaftAngle(), position, home_position_offset);
}

float EncoderController::GetUpdateRate()
{
    return update_rate;
}

EncoderController encoderController(500);
