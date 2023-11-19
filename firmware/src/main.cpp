#include <Arduino.h>
#include <TMC2209.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>

#define STEP 6
#define DIR 5

Uart Serial2(&sercom3, SCL, SDA, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler()
{
  Serial2.IrqHandler();
}
HardwareSerial &serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 100;

// Instantiate TMC2209
TMC2209 stepper_driver;
AccelStepper a_stepper(AccelStepper::DRIVER, STEP, DIR);
#define SET
void trySettings(){
   

  TMC2209::GlobalConfig cfg;
  cfg.i_scale_analog = 1;
  cfg.internal_rsense = 0;
  cfg.enable_spread_cycle = 0;
  cfg.shaft = 0;
  cfg.index_otpw = 1;
  cfg.index_step = 0;
  cfg.pdn_disable = 1; // always 1
  cfg.mstep_reg_select = 1;
  cfg.multistep_filt = 0;
  cfg.test_mode = 0;

  stepper_driver.global_config_ = cfg;
  stepper_driver.writeStoredGlobalConfig();

  TMC2209::DriverCurrent cfg_current;
  cfg_current.ihold = 20;
  cfg_current.irun = 20;
  cfg_current.iholddelay = 14;

  stepper_driver.driver_current_ = cfg_current;
  stepper_driver.writeStoredDriverCurrent();

  stepper_driver.write(TMC2209::ADDRESS_TPOWERDOWN, 20); 
  stepper_driver.write(TMC2209::ADDRESS_TPWMTHRS, 0xFFFFFF);
  stepper_driver.write(TMC2209::ADDRESS_VACTUAL, 0);
  stepper_driver.write(TMC2209::ADDRESS_TCOOLTHRS, 0xFFFF);
  stepper_driver.write(TMC2209::ADDRESS_SGTHRS, 60);
  
  TMC2209::CoolConfig cool_cfg;
  cool_cfg.seimin = 0;
  cool_cfg.sedn = 0;
  cool_cfg.semax = 0xF;
  cool_cfg.seup = 1;
  cool_cfg.semin = 0000;

  stepper_driver.cool_config_ = cool_cfg;
  stepper_driver.write(TMC2209::ADDRESS_COOLCONF, stepper_driver.cool_config_.bytes);

  TMC2209::ChopperConfig chop_cfg;
  chop_cfg.diss2vs = 0;
  chop_cfg.diss2g = 0;
  chop_cfg.double_edge = 0;
  chop_cfg.interpolation = 1;
  chop_cfg.mres = 8;
  chop_cfg.vsense = 0;
  chop_cfg.tbl = 0;
  chop_cfg.hstart = 0;
  chop_cfg.toff = 0b0011;
  
  stepper_driver.chopper_config_ = chop_cfg;
  stepper_driver.write(TMC2209::ADDRESS_CHOPCONF, stepper_driver.chopper_config_.bytes);

  TMC2209::PwmConfig pwm_cfg;
  pwm_cfg.pwm_lim = 12;
  pwm_cfg.pwm_reg = 8;
  pwm_cfg.freewheel = 0;
  pwm_cfg.pwm_autograd = 0;
  pwm_cfg.pwm_autoscale = 1;
  pwm_cfg.pwm_freq = 1;
  pwm_cfg.pwm_grad = 0x14;
  pwm_cfg.pwm_offset = 36;
  
  stepper_driver.pwm_config_ = pwm_cfg;
  stepper_driver.writeStoredPwmConfig();
  
}

void readSettings(){
   Serial.println("*************************");
  Serial.println("getSettings()");
  TMC2209::Settings settings = stepper_driver.getSettings();
  Serial.print("settings.is_communicating = ");
  Serial.println(settings.is_communicating);
  Serial.print("settings.is_setup = ");
  Serial.println(settings.is_setup);
  Serial.print("settings.software_enabled = ");
  Serial.println(settings.software_enabled);
  Serial.print("settings.microsteps_per_step = ");
  Serial.println(settings.microsteps_per_step);
  Serial.print("settings.inverse_motor_direction_enabled = ");
  Serial.println(settings.inverse_motor_direction_enabled);
  Serial.print("settings.stealth_chop_enabled = ");
  Serial.println(settings.stealth_chop_enabled);
  Serial.print("settings.standstill_mode = ");
  switch (settings.standstill_mode)
  {
    case TMC2209::NORMAL:
      Serial.println("normal");
      break;
    case TMC2209::FREEWHEELING:
      Serial.println("freewheeling");
      break;
    case TMC2209::STRONG_BRAKING:
      Serial.println("strong_braking");
      break;
    case TMC2209::BRAKING:
      Serial.println("braking");
      break;
  }
  Serial.print("settings.irun_percent = ");
  Serial.println(settings.irun_percent);
  Serial.print("settings.irun_register_value = ");
  Serial.println(settings.irun_register_value);
  Serial.print("settings.ihold_percent = ");
  Serial.println(settings.ihold_percent);
  Serial.print("settings.ihold_register_value = ");
  Serial.println(settings.ihold_register_value);
  Serial.print("settings.iholddelay_percent = ");
  Serial.println(settings.iholddelay_percent);
  Serial.print("settings.iholddelay_register_value = ");
  Serial.println(settings.iholddelay_register_value);
  Serial.print("settings.automatic_current_scaling_enabled = ");
  Serial.println(settings.automatic_current_scaling_enabled);
  Serial.print("settings.automatic_gradient_adaptation_enabled = ");
  Serial.println(settings.automatic_gradient_adaptation_enabled);
  Serial.print("settings.pwm_offset = ");
  Serial.println(settings.pwm_offset);
  Serial.print("settings.pwm_gradient = ");
  Serial.println(settings.pwm_gradient);
  Serial.print("settings.cool_step_enabled = ");
  Serial.println(settings.cool_step_enabled);
  Serial.print("settings.analog_current_scaling_enabled = ");
  Serial.println(settings.analog_current_scaling_enabled);
  Serial.print("settings.internal_sense_resistors_enabled = ");
  Serial.println(settings.internal_sense_resistors_enabled);
  Serial.println("*************************");
  Serial.println();

  Serial.println("*************************");
  Serial.println("hardwareDisabled()");
  bool hardware_disabled = stepper_driver.hardwareDisabled();
  Serial.print("hardware_disabled = ");
  Serial.println(hardware_disabled);
  Serial.println("*************************");
  Serial.println();

  Serial.println("*************************");
  Serial.println("getStatus()");
  TMC2209::Status status = stepper_driver.getStatus();
  Serial.print("status.over_temperature_warning = ");
  Serial.println(status.over_temperature_warning);
  Serial.print("status.over_temperature_shutdown = ");
  Serial.println(status.over_temperature_shutdown);
  Serial.print("status.short_to_ground_a = ");
  Serial.println(status.short_to_ground_a);
  Serial.print("status.short_to_ground_b = ");
  Serial.println(status.short_to_ground_b);
  Serial.print("status.low_side_short_a = ");
  Serial.println(status.low_side_short_a);
  Serial.print("status.low_side_short_b = ");
  Serial.println(status.low_side_short_b);
  Serial.print("status.open_load_a = ");
  Serial.println(status.open_load_a);
  Serial.print("status.open_load_b = ");
  Serial.println(status.open_load_b);
  Serial.print("status.over_temperature_120c = ");
  Serial.println(status.over_temperature_120c);
  Serial.print("status.over_temperature_143c = ");
  Serial.println(status.over_temperature_143c);
  Serial.print("status.over_temperature_150c = ");
  Serial.println(status.over_temperature_150c);
  Serial.print("status.over_temperature_157c = ");
  Serial.println(status.over_temperature_157c);
  Serial.print("status.current_scaling = ");
  Serial.println(status.current_scaling);
  Serial.print("status.stealth_chop_mode = ");
  Serial.println(status.stealth_chop_mode);
  Serial.print("status.standstill = ");
  Serial.println(status.standstill);
  Serial.println("*************************");
  Serial.println();
}

void setup()
{
  pinPeripheral(SCL, PIO_SERCOM);
  pinPeripheral(SDA, PIO_SERCOM);




  Serial.begin(SERIAL_BAUD_RATE);
  //while (!Serial)
  //  ;
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(13, OUTPUT);

  a_stepper.setMaxSpeed(20000.0);
  a_stepper.setAcceleration(10000.0);


  stepper_driver.setup(serial_stream, 9600);

  while (!stepper_driver.isCommunicating())
  {
    Serial.println("waiting for comms");
    delay(10);
  }



 

  #ifdef SET
  stepper_driver.setRunCurrent(10);
  stepper_driver.setMicrostepsPerStep(256);
  stepper_driver.useInternalSenseResistors();
  //stepper_driver.disableCoolStep();
  //stepper_driver.enable();
  #else
  trySettings();
  #endif
}

void loop()
{


  readSettings();
  Serial.println("running");
  digitalWrite(13, HIGH);
  a_stepper.runToNewPosition(5000);
  digitalWrite(13, LOW);
  a_stepper.runToNewPosition(0);
  delay(1000);
}
