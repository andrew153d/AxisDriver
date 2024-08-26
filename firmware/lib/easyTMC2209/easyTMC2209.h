#ifndef TMC2209_H
#define TMC2209_H
#include <Arduino.h>
#include "defines.h"

class easyTMC2209 : public TMC2209base
{
public:
  easyTMC2209();

// Alternate rx and tx pins may be specified for certain microcontrollers e.g.
// ESP32
#ifdef ESP32
  void setup(HardwareSerial &serial,
             long serial_baud_rate = 115200,
             SerialAddress serial_address = SERIAL_ADDRESS_0,
             int16_t alternate_rx_pin = -1,
             int16_t alternate_tx_pin = -1);
#else
  // Identify which microcontroller serial port is connected to the TMC2209 e.g.
  // Serial1, Serial2, etc. Optionally identify which serial address is assigned
  // to the TMC2209 if not the default of SERIAL_ADDRESS_0.
  void setup(HardwareSerial &serial,
             long serial_baud_rate = 115200,
             SerialAddress serial_address = SERIAL_ADDRESS_0);
#endif

#if SOFTWARE_SERIAL_IMPLEMENTED
  // Software serial ports should only be used for unidirectional communication
  // The RX pin does not need to be connected, but it must be specified when
  // creating an instance of a SoftwareSerial object
  void setup(SoftwareSerial &serial,
             long serial_baud_rate = 9600,
             SerialAddress serial_address = SERIAL_ADDRESS_0);
#endif

  // unidirectional methods

  // driver must be enabled before use it is disabled by default
  void setHardwareEnablePin(uint8_t hardware_enable_pin);
  void enable();
  void disable();

  // valid values = 1,2,4,8,...128,256, other values get rounded down
  void setMicrostepsPerStep(uint16_t microsteps_per_step);

  // valid values = 0-8, microsteps = 2^exponent, 0=1,1=2,2=4,...8=256
  // https://en.wikipedia.org/wiki/Power_of_two
  void setMicrostepsPerStepPowerOfTwo(uint8_t exponent);

  // range 0-100
  void setRunCurrent(uint8_t percent);
  // range 0-100
  void setHoldCurrent(uint8_t percent);
  // range 0-100
  void setHoldDelay(uint8_t percent);
  // range 0-100
  void setAllCurrentValues(uint8_t run_current_percent,
                           uint8_t hold_current_percent,
                           uint8_t hold_delay_percent);

  void enableInverseMotorDirection();
  void disableInverseMotorDirection();

  void setStandstillMode(StandstillMode mode);

  void enableAutomaticCurrentScaling();
  void disableAutomaticCurrentScaling();
  void enableAutomaticGradientAdaptation();
  void disableAutomaticGradientAdaptation();
  // range 0-255
  void setPwmOffset(uint8_t pwm_amplitude);
  // range 0-255
  void setPwmGradient(uint8_t pwm_amplitude);

  // default = 20
  // mimimum of 2 for StealthChop auto tuning
  void setPowerDownDelay(uint8_t power_down_delay);

  // mimimum of 2 when using multiple serial addresses
  // in bidirectional communication
  const static uint8_t REPLY_DELAY_MAX = 15;
  void setReplyDelay(uint8_t delay);

  void moveAtVelocity(int32_t microsteps_per_period);
  void moveUsingStepDirInterface();

  void enableStealthChop();
  void disableStealthChop();

  void setStealthChopDurationThreshold(uint32_t duration_threshold);

  void setStallGuardThreshold(uint8_t stall_guard_threshold);

  // lower_threshold: min = 1, max = 15
  // upper_threshold: min = 0, max = 15, 0-2 recommended
  void enableCoolStep(uint8_t lower_threshold = 1,
                      uint8_t upper_threshold = 0);
  void disableCoolStep();

  void setCoolStepCurrentIncrement(CurrentIncrement current_increment);

  void setCoolStepMeasurementCount(MeasurementCount measurement_count);
  void setCoolStepDurationThreshold(uint32_t duration_threshold);

  void enableAnalogCurrentScaling();
  void disableAnalogCurrentScaling();

  void useExternalSenseResistors();
  void useInternalSenseResistors();

  // bidirectional methods
  uint8_t getVersion();

  // if driver is not communicating, check power and communication connections
  bool isCommunicating();

  // check to make sure TMC2209 is properly setup and communicating
  bool isSetupAndCommunicating();

  // driver may be communicating but not setup if driver power is lost then
  // restored after setup so that defaults are loaded instead of setup options
  bool isCommunicatingButNotSetup();

  // driver may also be disabled by the hardware enable input pin
  // this pin must be grounded or disconnected before driver may be enabled
  bool hardwareDisabled();

  uint16_t getMicrostepsPerStep();

  Settings getSettings();

  Status getStatus();

  GlobalStatus getGlobalStatus();

  uint8_t getInterfaceTransmissionCounter();

  uint32_t getInterstepDuration();

  uint16_t getStallGuardResult();

  uint8_t getPwmScaleSum();
  int16_t getPwmScaleAuto();
  uint8_t getPwmOffsetAuto();
  uint8_t getPwmGradientAuto();

  uint16_t getMicrostepCounter();

  HardwareSerial *hardware_serial_ptr_;
#if SOFTWARE_SERIAL_IMPLEMENTED
  SoftwareSerial *software_serial_ptr_;
#endif
  uint32_t serial_baud_rate_;
  uint8_t serial_address_;
  int16_t hardware_enable_pin_;

  void initialize(long serial_baud_rate = 115200, SerialAddress serial_address = SERIAL_ADDRESS_0);
  int serialAvailable();
  size_t serialWrite(uint8_t c);
  int serialRead();
  void serialFlush();

  GlobalConfig global_config_;
  DriverCurrent driver_current_;
  CoolConfig cool_config_;
  bool cool_step_enabled_;
  ChopperConfig chopper_config_;
  uint8_t toff_ = TOFF_DEFAULT;
  PwmConfig pwm_config_;

  void setOperationModeToSerial(SerialAddress serial_address);

  void setRegistersToDefaults();
  void readAndStoreRegisters();

  bool serialOperationMode();

  void minimizeMotorCurrent();

  uint32_t reverseData(uint32_t data);
  template <typename Datagram>
  uint8_t calculateCrc(Datagram &datagram,
                       uint8_t datagram_size);
  template <typename Datagram>
  void sendDatagramUnidirectional(Datagram &datagram,
                                  uint8_t datagram_size);
  template <typename Datagram>
  void sendDatagramBidirectional(Datagram &datagram,
                                 uint8_t datagram_size);

  void write(uint8_t register_address,
             uint32_t data);
  uint32_t read(uint8_t register_address);

  uint8_t percentToCurrentSetting(uint8_t percent);
  uint8_t currentSettingToPercent(uint8_t current_setting);
  uint8_t percentToHoldDelaySetting(uint8_t percent);
  uint8_t holdDelaySettingToPercent(uint8_t hold_delay_setting);

  uint8_t pwmAmplitudeToPwmAmpl(uint8_t pwm_amplitude);
  uint8_t pwmAmplitudeToPwmGrad(uint8_t pwm_amplitude);

  void writeStoredGlobalConfig();
  uint32_t readGlobalConfigBytes();
  void writeStoredDriverCurrent();
  void writeStoredChopperConfig();
  uint32_t readChopperConfigBytes();
  void writeStoredPwmConfig();
  uint32_t readPwmConfigBytes();
};

#endif
