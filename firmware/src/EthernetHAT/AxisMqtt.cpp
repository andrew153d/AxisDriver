#include "AxisMqtt.h"
#include "utility/w5100.h"
#include "EncoderController/EncoderController.h"
#include "LedController/LedController.h"

MQTTTask::MQTTTask(int p)
{
    //executionPeriod = p;
}

MQTTTask::~MQTTTask()
{
    if (mqtt != nullptr)
    {
        delete mqtt;
        mqtt = nullptr;
    }
    if (device != nullptr)
    {
        delete device;
        device = nullptr;
    }
}

void MQTTTask::OnStart()
{
    device = new HADevice(FlashStorage::GetMacAddress(), sizeof(FlashStorage::GetMacAddress()));
    
    mqtt = new HAMqtt(client, *device);

    ha_settings = FlashStorage::GetHASettings();

    ha_settings->mode = HAMode::POSITION_SWITCH; // Default mode

    strncpy(ha_settings->mqtt_user, "MQTT_User", sizeof(ha_settings->mqtt_user) - 1);
    ha_settings->mqtt_user[sizeof(ha_settings->mqtt_user) - 1] = '\0';

    strncpy(ha_settings->mqtt_password, "1234", sizeof(ha_settings->mqtt_password) - 1);
    ha_settings->mqtt_password[sizeof(ha_settings->mqtt_password) - 1] = '\0';

    strncpy(ha_settings->mqtt_name, "DogFeeder", sizeof(ha_settings->mqtt_name) - 1);
    ha_settings->mqtt_name[sizeof(ha_settings->mqtt_name) - 1] = '\0';

    strncpy(ha_settings->mqtt_icon, "mdi:bowl", sizeof(ha_settings->mqtt_icon) - 1);
    ha_settings->mqtt_icon[sizeof(ha_settings->mqtt_icon) - 1] = '\0';

    ha_settings->velocity_switch_on_speed = 400;
    ha_settings->position_switch_on_position = 900;

    DEBUG_PRINTF("HA Settings:\n");
    DEBUG_PRINTF("  enable: %d\n", ha_settings->enable);
    DEBUG_PRINTF("  mode: %d\n", static_cast<int>(ha_settings->mode));
    DEBUG_PRINTF("  ha_ip_address: %d.%d.%d.%d\n",
                 ha_settings->ha_ip_address[0],
                 ha_settings->ha_ip_address[1],
                 ha_settings->ha_ip_address[2],
                 ha_settings->ha_ip_address[3]);
    DEBUG_PRINTF("  velocity_switch_on_speed: %.2f\n", ha_settings->velocity_switch_on_speed);
    DEBUG_PRINTF("  velocity_switch_off_speed: %.2f\n", ha_settings->velocity_switch_off_speed);
    DEBUG_PRINTF("  position_switch_on_position: %.2f\n", ha_settings->position_switch_on_position);
    DEBUG_PRINTF("  position_switch_off_position: %.2f\n", ha_settings->position_switch_off_position);
    DEBUG_PRINTF("  velocity_slider_min: %.2f\n", ha_settings->velocity_slider_min);
    DEBUG_PRINTF("  velocity_slider_max: %.2f\n", ha_settings->velocity_slider_max);
    DEBUG_PRINTF("  position_slider_min: %.2f\n", ha_settings->position_slider_min);
    DEBUG_PRINTF("  position_slider_max: %.2f\n", ha_settings->position_slider_max);
    DEBUG_PRINTF("  mqtt_user: %s\n", ha_settings->mqtt_user);
    DEBUG_PRINTF("  mqtt_password: %s\n", ha_settings->mqtt_password);
    DEBUG_PRINTF("  mqtt_name: %s\n", ha_settings->mqtt_name);
    DEBUG_PRINTF("  mqtt_icon: %s\n", ha_settings->mqtt_icon);

    device->setName(ha_settings->mqtt_name);

    switch (ha_settings->mode)
    {
    case HAMode::VELOCITY_SWITCH:
        velocity_switch = new HASwitch("Velocity_Switch_Id");
        velocity_switch->setName(ha_settings->mqtt_name);
        velocity_switch->setIcon(ha_settings->mqtt_icon);
        velocity_switch->setState(false);

        velocity_switch->onCommand(
            [](bool state, HASwitch *sender)
            {
                mqttTask.OnVelocitySwitchCommand(state);
            });

        DEBUG_PRINTLN("Velocity switch mode");

        break;
    case HAMode::POSITION_SWITCH:
        position_switch = new HASwitch("Position_Switch_Id");
        position_switch->setName(ha_settings->mqtt_name);
        position_switch->setIcon(ha_settings->mqtt_icon);
        position_switch->setState(false);
        position_switch->onCommand(
            [](bool state, HASwitch *sender)
            {
                mqttTask.OnPositionSwitchCommand(state);
            });

        break;
    case HAMode::VELOCITY_SLIDER:
        velocity_slider = new HANumber("Velocity_Slider_Id", HANumber::PrecisionP0);
        velocity_slider->setName(ha_settings->mqtt_name);
        velocity_slider->setIcon(ha_settings->mqtt_icon);
        velocity_slider->setMin(ha_settings->velocity_slider_min);
        velocity_slider->setMax(ha_settings->velocity_slider_max);
        velocity_slider->setUnitOfMeasurement("deg/s");
        velocity_slider->setStep(1);
        velocity_slider->onCommand(
            [](HANumeric value, HANumber *sender)
            {
                mqttTask.OnVelocitySliderCommand(value);
            });
        break;
    case HAMode::POSITION_SLIDER:
        position_slider = new HANumber("Position_Slider_Id", HANumber::PrecisionP0);
        position_slider->setName(ha_settings->mqtt_name);
        position_slider->setIcon(ha_settings->mqtt_icon);
        position_slider->setMin(ha_settings->position_slider_min);
        position_slider->setMax(ha_settings->position_slider_max);
        position_slider->setUnitOfMeasurement("deg");
        position_slider->setStep(1);
        position_slider->onCommand(
            [](HANumeric value, HANumber *sender)
            {
                mqttTask.OnPositionSliderCommand(value);
            });
        break;
    default:
        DEBUG_PRINTLN("Unsupported mode");
    }

    mqtt->begin(BROKER_ADDR,
               ha_settings->mqtt_user,
               ha_settings->mqtt_password);
}

void MQTTTask::OnRun()
{
    if (mqtt == nullptr || device == nullptr)
    {
        return;
    }
    if (mqtt->isConnected() && !connected)
    {
        connected = true;
        DEBUG_PRINTLN("MQTT connected");
    }
    Ethernet.maintain();
    mqtt->loop();
}

void MQTTTask::OnVelocitySwitchCommand(bool state)
{
    if (velocity_switch != nullptr)
    {
        velocity_switch->setState(state);
        VelocityMessage msg;
        msg.header.message_type = (uint16_t)MessageTypes::SetVelocityId;
        msg.header.body_size = sizeof(msg.value);
        msg.value = state ? ha_settings->velocity_switch_on_speed : ha_settings->velocity_switch_off_speed;
        msg.footer.checksum = 0;
        HandleIncomingMsg((uint8_t *)&msg, sizeof(msg));
    }
}

void MQTTTask::OnPositionSwitchCommand(bool state)
{
    if (position_switch != nullptr)
    {
        position_switch->setState(state);
        TargetPositionMessage msg;
        msg.header.message_type = (uint16_t)MessageTypes::SetTargetPositionId;
        msg.header.body_size = sizeof(msg.value);
        msg.value = state ? ha_settings->position_switch_on_position : ha_settings->position_switch_off_position;
        msg.footer.checksum = 0;
        HandleIncomingMsg((uint8_t *)&msg, sizeof(msg));
    }
}

void MQTTTask::OnVelocitySliderCommand(const HANumeric &value)
{
    if (velocity_slider != nullptr)
    {
        velocity_slider->setState(value);
        DEBUG_PRINTF("Velocity slider state changed to: %.2f\n", value.toFloat());
    }
}

void MQTTTask::OnPositionSliderCommand(const HANumeric &value)
{
    if (position_slider != nullptr)
    {
        position_slider->setState(value);
        DEBUG_PRINTF("Position slider state changed to: %.2f\n", value.toFloat());
    }
}

void MQTTTask::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    if (processor_interface_ != nullptr)
    {
        processor_interface_->HandleIncomingMsg(recv_bytes, recv_bytes_size, this);
    }
    else
    {
        Serial.println("processor_interface is null");
    }
}

void MQTTTask::SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) {

};

void MQTTTask::OnStop()
{
}

MQTTTask mqttTask(20);
