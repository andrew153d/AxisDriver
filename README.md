# AxisDriver #

# Interfaces #
- USB
- I2C


# JsonInterface #
- Several peripherals that accept control
  - LED
  - Position Sensor
    - ```
    {
      "type": "Encoder", 
      "mode": "GetPosition",
      "position":0(optional float - device will return with this value)
    }
    ```
    - ```
    {
      "type": "Encoder", 
      "mode": "SetPosition",
      "position":0
    }
    ```
    - ```
    {
      "type": "Encoder", 
      "mode": "GetUpdateRate",
      "rate":0(optional float Hz- device will return with this value)
    }
    ```
  - Motor
    - There are two main modes of control
      - Position
        - 
        ```
        {
          "type": "Movement", 
          "mode": "SetPosition", 
          "steps": -100000, (integer)
          "max_speed": 100, (integer, steps_per_second)
          "accelerration":1000 (int, steps_per_second^2, optional)
          "microsteps": 64, (optional)
          "hold": 0 (optional - hold position when it reaches the end)
        }
        ```
      - Velocity
        - 
        ```
        {
          "type": "Movement", 
          "mode": "SetVelocity", 
          "speed": 10000, (integer, steps_per_second)
          "duration":10 (float, seconds, optional)
          "microsteps": 64, (integer, optional)
        }
        ```


## Future Additions ##

- Ethernet
- RS485
- RS422
- RS232
- CAN
- Wifi
- Bluetooth
- ModBus

