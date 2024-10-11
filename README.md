# AxisDriver #


# JsonInterface #
- Several peripherals that accept control
  - LED
  - Position Sensor
  - Motor
    - There are two main modes of control
      - Position
        - 
        ```
        {
            "type": "MOVEMENT", 
            "mode": "POSITION", 
            "steps": -100000, (integer)
            "max_speed": 100, (integer, steps_per_second, optional)
            "accelerration":1000 (int, steps_per_second^2, optional)
            "microsteps": 64, (optional)
            "hold": 0 (optional - hold position when it reaches the end)
        }
        ```
      - Velocity
        - 
        ```
        {
            "type": "MOVEMENT", 
            "mode": "VELOCITY", 
            "speed": 10000, (integer, steps_per_second)
            "duration":10 (float, seconds, optional)
            "accelerration":1000 (integer, optional)
            "microsteps": 64, (integer, optional)
            "hold": 0 (boolean, optional)
        }
        ```
  

