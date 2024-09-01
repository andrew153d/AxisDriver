#include "EncoderController.h"

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
    
    Wire1.begin();
    Tlv493dMagnetic3DSensor.begin(Wire1);
    Tlv493dMagnetic3DSensor.setAccessMode(Tlv493dMagnetic3DSensor.MASTERCONTROLLEDMODE);
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
    Tlv493dMagnetic3DSensor.updateData();
    raw_shaft_angle = Wrap0to360(degrees(Tlv493dMagnetic3DSensor.getAzimuth()) - home_position_offset);
    //Serial.printf("%d\n", raw_shaft_angle);
    // load the sliding window filter
    // to avoid invalid values at the ends, apply an offset
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

    //keep track of the full turns
    float delta = sliding_window_center - prev_sliding_window_center;
    if(delta < -180){
        number_full_turns ++;
    }

    if(delta > 180){
        number_full_turns --;
    }

    full_shaft_position = Wrap0to360(sliding_window_center) + 360*number_full_turns;
    prev_sliding_window_center = sliding_window_center;

    shaft_velocity = (full_shaft_position-last_full_shaft_position)/((float)executionPeriod/1000);
    //Serial.printf("%8.1f %8.1f %8.1f %8.1f %8.1f\n", sliding_window_center, delta, number_full_turns, full_shaft_position, shaft_velocity);
    
    last_full_shaft_position = full_shaft_position;


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
