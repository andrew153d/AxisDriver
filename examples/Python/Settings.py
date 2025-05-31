
from AxisMessages import *
from Axis import *

SerialComms = AxisSerial('/dev/ttyACM0')


def print_ha_ip(ip_bytes):
    if isinstance(ip_bytes, (bytes, bytearray)) and len(ip_bytes) == 4:
        print(f"HA IP Address: {ip_bytes[0]}.{ip_bytes[1]}.{ip_bytes[2]}.{ip_bytes[3]}")
    else:
        print(f"HA IP Address: {ip_bytes}")

ha_enable = GetHAEnable(SerialComms)
print(f"HA Enable: {ha_enable.value if ha_enable else 'Failed to get'}")

ha_mode = GetHAMode(SerialComms)
print(f"HA Mode: {ha_mode.value if ha_mode else 'Failed to get'}")

ha_ip = GetHAIpAddress(SerialComms)
if ha_ip:
    print_ha_ip(ha_ip.ha_ip_address)
else:
    print("Failed to get HA IP Address")

ha_vel_on = GetHAVelocitySwitchOnSpeed(SerialComms)
print(f"HA Velocity Switch On Speed: {ha_vel_on.value if ha_vel_on else 'Failed to get'}")
ha_vel_off = GetHAVelocitySwitchOffSpeed(SerialComms)
print(f"HA Velocity Switch Off Speed: {ha_vel_off.value if ha_vel_off else 'Failed to get'}")
ha_pos_on = GetHAPositionSwitchOnPosition(SerialComms)
print(f"HA Position Switch On Position: {ha_pos_on.value if ha_pos_on else 'Failed to get'}")
ha_pos_off = GetHAPositionSwitchOffPosition(SerialComms)
print(f"HA Position Switch Off Position: {ha_pos_off.value if ha_pos_off else 'Failed to get'}")
ha_vel_min = GetHAVelocitySliderMin(SerialComms)
print(f"HA Velocity Slider Min: {ha_vel_min.value if ha_vel_min else 'Failed to get'}")
ha_vel_max = GetHAVelocitySliderMax(SerialComms)
print(f"HA Velocity Slider Max: {ha_vel_max.value if ha_vel_max else 'Failed to get'}")
ha_pos_min = GetHAPositionSliderMin(SerialComms)
print(f"HA Position Slider Min: {ha_pos_min.value if ha_pos_min else 'Failed to get'}")
ha_pos_max = GetHAPositionSliderMax(SerialComms)
print(f"HA Position Slider Max: {ha_pos_max.value if ha_pos_max else 'Failed to get'}")
ha_mqtt_user = GetHAMqttUser(SerialComms)
if ha_mqtt_user:
    mqtt_user_str = ha_mqtt_user.value.split(b'\n')[0].decode('utf-8')
    print(f"HA MQTT User: {mqtt_user_str}")
else:
    print("HA MQTT User: Failed to get")
ha_mqtt_password = GetHAMqttPassword(SerialComms)
if ha_mqtt_password:
    mqtt_password_str = ha_mqtt_password.value.split(b'\n')[0].decode('utf-8')
    print(f"HA MQTT Password: {mqtt_password_str}")
else:
    print("HA MQTT Password: Failed to get")
ha_mqtt_name = GetHAMqttName(SerialComms)
if ha_mqtt_name:
    mqtt_name_str = ha_mqtt_name.value.split(b'\n')[0].decode('utf-8')
    print(f"HA MQTT Name: {mqtt_name_str}")
else:
    print("HA MQTT Name: Failed to get")
ha_mqtt_icon = GetHAMqttIcon(SerialComms)
if ha_mqtt_icon:
    mqtt_icon_str = ha_mqtt_icon.value.split(b'\n')[0].decode('utf-8')
    print(f"HA MQTT Icon: {mqtt_icon_str}")
else:
    print("HA MQTT Icon: Failed to get")

time.sleep(1)