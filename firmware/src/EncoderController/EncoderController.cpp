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
    Wire.setClock(400000);
    Tlv493dMagnetic3DSensor.begin(Wire1);
    Tlv493dMagnetic3DSensor.setAccessMode(Tlv493d::AccessMode_e::FASTMODE);
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
    Tlv493d_Error_t err = Tlv493dMagnetic3DSensor.updateData();
    if(err!=Tlv493d_Error_t::TLV493D_NO_ERROR){
        Serial.printf("Error: %d\n", err);
    }


    raw_shaft_angle = Wrap0to360(degrees(Tlv493dMagnetic3DSensor.getAzimuth()));
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

    shaft_velocity = (full_shaft_position - last_full_shaft_position) / ((float)executionPeriod / 1000);
    // Serial.printf("%8.1f %8.1f %8.1f %8.1f %8.1f\n", sliding_window_center, delta, number_full_turns, full_shaft_position, shaft_velocity);

    last_full_shaft_position = full_shaft_position;
    
    update_rate_ticker++;

    if(millis()-update_rate_timer>update_rate_poll_period){
        update_rate_timer = millis();
        update_rate = (float)update_rate_ticker*(1000/(float)update_rate_poll_period);
        update_rate_ticker = 0;
    }
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

void EncoderController::SetPosition(float position){
    home_position_offset += (position - GetShaftAngle());
    
    Serial.printf("raw %f, offset %f, home %f \n", GetShaftAngle(), position, home_position_offset);
}

float EncoderController::GetUpdateRate()
{
return update_rate;
}

void EncoderController::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    if (recv_bytes_size == 0)
        return;

    if ((char)recv_bytes[0] == '{')
        HandleJsonMsg(recv_bytes, recv_bytes_size);
    else
        HandleByteMsg(recv_bytes, recv_bytes_size);
}

void EncoderController::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    DeserializationError error = deserializeJson(recvd_json, recv_bytes, 256);
    if (error)
    {
        Serial.println("Error parsing json in motor controller");
        return;
    }

    String command = "";

    if (!TryParseJson<String>("mode", &command, recvd_json))
    {
        Serial.println("Failed to parse encoder mode");
        return;
    }

    EncoderCommands encoder_command = ToEncoderCommandsEnum(command);

    switch (encoder_command)
    {
    case EncoderCommands::SetPosition:
    {
        float offset = 0;
        if (!TryParseJson<float>("position", &offset, recvd_json))
        {
            Serial.println("Failed to parse position");
            return;
        }
        SetPosition(offset);
        break;
    }
    case EncoderCommands::GetPosition:
    {
        recvd_json.clear();
        recvd_json["type"] = ENCODERTYPESTRING;
        recvd_json["mode"] = GETPOSITIONCOMMAND;
        recvd_json["position"] = GetShaftAngle();
        String output;
        serializeJson(recvd_json, output);
        Serial.println(output);
        break;
    }
    case EncoderCommands::GetUpdateRate:
    {

        recvd_json.clear();
        recvd_json["type"] = ENCODERTYPESTRING;
        recvd_json["mode"] = GETUPDATERATECOMMAND;
        recvd_json["rate"] = GetUpdateRate();
        String output;
        serializeJson(recvd_json, output);
        Serial.println(output);
        break;
    }
    }
}

void EncoderController::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
    {
        Serial.println("Size too small");
    }

    Header *header = (Header *)recv_bytes;
    if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
    {
        Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
        return;
    }

    //   switch (header->message_type)
    //   {
    //   break;
    //   default:
    //     break;
    //   }
}
