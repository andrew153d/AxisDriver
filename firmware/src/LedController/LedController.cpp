#include "LedController.h"

void AddrLedController::OnStart()
{
    FastLED.addLeds<NEOPIXEL, NEOPIX>(leds, 1);
    prevState = LedStates::OFF;
}

void AddrLedController::OnStop()
{
}

void AddrLedController::OnRun()
{
    switch (ledState)
    {
    case OFF:
        break;
    case FLASH_ERROR:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB(0x100000);
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB(0x100000);
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            //SetLedState(prevState);
            break;
        }
        step_counter++;
        break;
    case ERROR:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB::Red;
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        }
        step_counter++;
        step_counter %= 2;
        break;
    case BOOTUP:
        switch (step_counter)
        {
        case 0:
            leds[0] = CRGB(0x110000);
            FastLED.show();
            break;
        case 1:
            leds[0] = 0;
            FastLED.show();
            break;
        case 2:
            leds[0] = CRGB(0x001100);
            FastLED.show();
            break;
        case 3:
            leds[0] = 0;
            FastLED.show();
            break;
        case 4:
            leds[0] = CRGB(0x000011);
            FastLED.show();
            break;
        case 5:
            leds[0] = 0;
            FastLED.show();
            break;
        case 6:
            //SetLedState(prevState);
            break;
        }
        step_counter++;
        break;
    case RAINBOW:
        hue = (hue + 1);
        leds[0] = CHSV(hue, 255, ledConfig.rainbow_brightness);
        FastLED.show();
        break;
    }
}

void AddrLedController::SetLedState(LedStates state)
{

    if(state == ledState)
        return;

    prevState = ledState;
    ledState = state;
    switch (ledState)
    {
    case OFF:
        leds[0] = CRGB::Black;
        FastLED.show();
        executionPeriod = 0xFFFFFFFF;
        break;
    case FLASH_ERROR:
        step_counter = 0;
        executionPeriod = FLASH_ERROR_EXEC_PERIOD;
        break;
    case ERROR:
        step_counter = 0;
        executionPeriod = ERROR_EXEC_PERIOD;
        break;
    case BOOTUP:
        step_counter = 0;
        executionPeriod = BOOTUP_EXEC_PERIOD;
        break;
    case RAINBOW:
        hue = (hue + 1) % 255;
        leds[0] = CHSV(hue, 255, 50);
        FastLED.show();
        executionPeriod = RAINBOW_EXEC_PERIOD;
        break;
    case SOLID:
        executionPeriod = 0xFFFFFFFF;
    }
}

void AddrLedController::SetLEDColor(CHSV color)
{
    leds[0] = color;
    FastLED.show();
}

void AddrLedController::SetLEDColor(CRGB color)
{
    leds[0] = color;
    FastLED.show();
}

void AddrLedController::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  if (recv_bytes_size == 0)
    return;

  if ((char)recv_bytes[0] == '{')
    HandleJsonMsg(recv_bytes, recv_bytes_size);
  else
    HandleByteMsg(recv_bytes, recv_bytes_size);
}

void AddrLedController::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
  DeserializationError error = deserializeJson(received_json, recv_bytes, 256);
  if (error)
  {
    Serial.println("Error parsing json in motor controller");
    return;
  }

  if (received_json.containsKey("mode"))
  {
     String mode_string = received_json["mode"];
     
     if(mode_string=="Solid"){
        leds[0] = CRGB(received_json["red"], received_json["green"], received_json["blue"]);
        FastLED.show();
        SetLedState(SOLID);
     }
  }

//   switch (controlMode)
//   {
//   case ControlMode::POSITION:
//   {
//     int max_speed = 1000;
//     int acceleration = 1000;
//     int microsteps = 64;
//     bool hold = false;

//     if (received_json.containsKey("steps"))
//     {
//       target_position = received_json["steps"];
//     }
//     else
//     {
//       Serial.println("Failed to parse steps");
//       return;
//     }

//     if (received_json.containsKey("max_speed"))
//       max_speed = received_json["max_speed"];

//     if (received_json.containsKey("acceleration"))
//       acceleration = received_json["acceleration"];
    
//     if (driver.getMicrostepsPerStep() != microsteps)
//       driver.setMicrostepsPerStep(microsteps);
    
//     stepper.move(target_position);
//     stepper.setMaxSpeed(max_speed);
//     stepper.setAcceleration(acceleration);
//     stepper.enableOutputs();
//     break;
//   }
//   case ControlMode::VELOCITY:
//   {
//     // long target_velocity = 0;
//     // uint32_t target_velocity_duration = 0;
//     // uint32_t target_velocity_timer = 0;
//     int acceleration = 10000;
//     int microsteps = 64;
//     target_velocity_duration = 0;

//     if (received_json.containsKey("speed"))
//     {
//       target_velocity = received_json["speed"];
//     }
//     else
//     {
//       Serial.println("Failed to parse speed");
//       return;
//     }

//     if (received_json.containsKey("duration"))
//       target_velocity_duration = received_json["duration"];
    
//     if (received_json.containsKey("acceleration"))
//       acceleration = received_json["acceleration"];
    

//     if (received_json.containsKey("microsteps"))
//       microsteps = received_json["microsteps"];

//     if (driver.getMicrostepsPerStep() != microsteps)
//       driver.setMicrostepsPerStep(microsteps);
//     Serial.printf("Setting speed to %d\n", target_velocity);
//     stepper.setSpeed(target_velocity);
//     //stepper.setAcceleration(acceleration);
//     stepper.enableOutputs();
//     target_velocity_timer = millis();
//     break;
//   }
//   default:
//     Serial.println("Invalid Mode");
//     break;
//   }
}

void AddrLedController::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size = 0)
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

    switch (header->message_type)
    {
    case MessageTypes::SetLedColor:
        if (header->body_size == 3)
        {
            leds[0].r = recv_bytes[HEADER_SIZE];
            leds[0].g = recv_bytes[HEADER_SIZE+1];
            leds[0].b = recv_bytes[HEADER_SIZE+2];
            FastLED.show();
        }
        else
        {
            Serial.println("SetLed body size incorrect");
        }
        break;
    case MessageTypes::GetLedColor:

        break;
    case MessageTypes::SetLedState:
        {
            LedStates s = (LedStates)recv_bytes[HEADER_SIZE];
            Serial.printf("Setting LED Mode: %d", (uint8_t)s);
            SetLedState(s);
        }
    break;
    default:
        break;
    }
}

void AddrLedController::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
{
    Serial.println(" Trying to send message ");
}

AddrLedController addrLedController(500);
