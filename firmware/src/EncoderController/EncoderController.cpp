#include "EncoderController.h"

float Wrap0to360(float input) {
    input = fmodf(input, 360.0f);
    if (input < 0) {
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
    shaft_angle = Wrap0to360(degrees(Tlv493dMagnetic3DSensor.getAzimuth()));

    // load the average filter
    filter_write_pos++;
    average_filter[filter_write_pos %= average_filter_length] = shaft_angle;

    // load the sliding window filter
    
    //to avoid invalid values at the ends, apply an offset
    float offset = 180-shaft_angle;
    sliding_window_center +=offset;
    shaft_angle+=offset;
    

    if (sliding_window_center - shaft_angle > sliding_window_radius)
    { // center > shaft_angle
        sliding_window_center = shaft_angle + sliding_window_radius;
    }else
    if (shaft_angle - sliding_window_center > sliding_window_radius)
    { // shaft_angle > center
        sliding_window_center = shaft_angle - sliding_window_radius;
    }else{
        sliding_window_center = sliding_window_alpha*shaft_angle+(1-sliding_window_alpha)*sliding_window_center;
    }

    sliding_window_center -=offset;
    shaft_angle-=offset;
}

float EncoderController::GetShaftAngle()
{
    return shaft_angle;
}

float EncoderController::GetRunningAverageShaftAngle()
{
    float sum = 0;
    for (auto measurement : average_filter)
    {
        sum += measurement;
    }
    return sum / average_filter_length;
}

float EncoderController::GetSlidingWindowShaftAngle(){
    return sliding_window_center;
}