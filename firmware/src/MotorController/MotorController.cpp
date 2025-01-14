#include "MotorController.h"
#include "LedController/LedController.h"
float readVUSB()
{
  return (float)analogRead(PIN_VUSB) * (3.3f / 1024.0f) * 2;
}

float readVBUS()
{
  return analogRead(PIN_VBUS) * (24.0f / 553.0f);
}

bool UserButtonPressed()
{
  return !digitalRead(USR_INPUT);
}

/*
Future implementations should totally implement this
https://forum.arduino.cc/t/samd51-dac-using-dma-seems-too-fast/678418/6

#define HWORDS 4000
uint16_t data[HWORDS];
float phase = 3.14159 * 2. / HWORDS;
int i;
static DmacDescriptor descriptor1 __attribute__((aligned(16)));
void dma_init() {

  static DmacDescriptor descriptor __attribute__((aligned(16)));
  static DmacDescriptor descriptor1 __attribute__((aligned(16)));
  static DmacDescriptor wrb __attribute__((aligned(16)));
  static uint32_t chnl0 = 0;  // DMA channel
  DMAC->BASEADDR.reg = (uint32_t)&descriptor1;
  DMAC->WRBADDR.reg = (uint32_t)&wrb;
  DMAC->CTRL.bit.LVLEN0 = 1 ;
  DMAC->CTRL.bit.LVLEN1 = 1 ;
  DMAC->CTRL.bit.LVLEN2 = 1 ;
  DMAC->CTRL.bit.LVLEN3 = 1 ;
  DMAC->CTRL.bit.DMAENABLE = 1;
  DMAC->Channel[0].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(0x49) |   // DAC DATA trigger
                                 DMAC_CHCTRLA_TRIGACT_BURST;    // Burst transfers
  descriptor1.DESCADDR.reg = (uint32_t) &descriptor1;
  descriptor1.BTCTRL.bit.VALID    = 0x1; //Its a valid channel
  descriptor1.BTCTRL.bit.BEATSIZE = 0x1;  // HWORD.
  descriptor1.BTCTRL.bit.SRCINC   = 0x1;   //Source increment is enabled
  descriptor1.BTCTRL.bit.DSTINC   = 0x0;   //Destination increment disabled
  descriptor1.BTCTRL.bit.BLOCKACT = 0x2;   //Suspend after block complete.
  // ("Burst" size will be 1 "beat" = 1 HWORD, by default)
  descriptor1.BTCNT.reg           = HWORDS;   //HWORDS points to send
  descriptor1.SRCADDR.reg         = (uint32_t)(&data[HWORDS]); //send from the data vevtor
  descriptor1.DSTADDR.reg         = (uint32_t)&DAC->DATA[1].reg;   //to the DAC output
  // start channel
  DMAC ->Channel[0].CHCTRLA.bit.ENABLE = 0x1;     //OK
  DMAC->CTRL.bit.DMAENABLE = 1;
}

void dac_init() {
 
 GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(4) |       // Divide the 
48MHz clock source by divisor 4: 48MHz/4 = 12MHz (max for DAC)
                         GCLK_GENCTRL_GENEN |        // Enable GCLK7
                         GCLK_GENCTRL_SRC_DFLL;      // Select 48MHz DFLL clock source

  while (GCLK->SYNCBUSY.bit.GENCTRL7);
  GCLK->PCHCTRL[42].reg = GCLK_PCHCTRL_CHEN |        // Enable the DAC peripheral channel
                          GCLK_PCHCTRL_GEN_GCLK7;    // Connect generic clock 7 to DAC
  MCLK->APBDMASK.bit.DAC_ = 1;
  DAC->CTRLA.bit.SWRST = 1;
  while (DAC->CTRLA.bit.SWRST);
  DAC->DACCTRL[1].reg = DAC_DACCTRL_REFRESH(2) |
                        DAC_DACCTRL_CCTRL_CC12M |
                        DAC_DACCTRL_ENABLE
                        //                        | DAC_DACCTRL_FEXT
                        ;
  DAC_DACCTRL_LEFTADJ;
  DAC->CTRLA.reg = DAC_CTRLA_ENABLE;
  while (DAC->SYNCBUSY.bit.ENABLE);
  while (!DAC->STATUS.bit.READY1);
  PORT->Group[0].DIRSET.reg = (1 << 2);
  PORT->Group[0].PINCFG[5].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[1].bit.PMUXE = 1;
}

void setup() {
  for (i = 0; i < HWORDS; i++) {
    data[i] = 4 * (sinf(i * phase) * 510.0f + 512.0f); //Make a single-period sine wave.
  }
#ifdef ANALOGWRITESQUARE
  //  for (i = 0; i < 100000; i++) {
  while (0) {
    analogWrite(A0, 999);
    delayMicroseconds(100);
    analogWrite(A0, 0);
    delayMicroseconds(100);
  }
#endif


#ifdef ANALOGWRITERAMP
  while (1) {
    i++;
    analogWrite(A0, i & 0x3FF);
  }
#endif
  dac_init();
  dma_init();
}

//#define ANALOGWRITESINE 1
void loop() {
#ifdef ANALOGWRITESINE
  for (i = 0; i < HWORDS; i++) {
    analogWrite(A0, data[i]);
  }
#endif
  DMAC ->Channel[0].CHCTRLB.reg = 0x2;     // resume
  delayMicroseconds(8000);
}

*/

void MotorController::CheckForErrors()
{
  motorErrors.bits.TMC_lost_comms = (driver.getVersion() == 0);
  motorErrors.bits.lostPower = (readVBUS() < 5.0f);

  if (previousErrors.errors > 0 && motorErrors.errors == 0)
  {
    stepper.enableOutputs();
  }
  if (previousErrors.errors == 0 && motorErrors.errors > 0)
  {
    // stepper.disableOutputs();
    // DEBUG_PRINTF("Motor Error Present: %4x\n", motorErrors.errors);
    // addrLedController.SetLedState(FLASH_ERROR);
    // controlMode = ControlMode::MOTOR_OFF;
  }

  previousErrors = motorErrors;
}

void MotorController::OnStart()
{

  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_STEP, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_M0, OUTPUT);
  pinMode(MOTOR_M1, OUTPUT);
  pinMode(MOTOR_SPREAD, OUTPUT);
  pinMode(MOTOR_DIAG, INPUT);

  digitalWrite(MOTOR_M0, LOW);
  digitalWrite(MOTOR_M1, LOW);
  digitalWrite(MOTOR_EN, HIGH);
  digitalWrite(MOTOR_SPREAD, LOW);

  stepper.setEnablePin(MOTOR_EN);
  stepper.setPinsInverted(true, false, true);
  stepper.disableOutputs();
  stepper.setAcceleration(400 * 64);
  stepper.setMaxSpeed(100 * 64);

  driver.setup(serial_stream, 115200, TMC2209base::SerialAddress::SERIAL_ADDRESS_0);
  //driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  //driver.setStandstillMode(TMC2209base::StandstillMode::BRAKING);
  SetMotorState(MotorStates::OFF);
  start_millis = millis();
  target_position = 0;

  DEBUG_PRINTF("TMC2209 Version: %d\n", driver.getVersion());
}

void MotorController::OnStop()
{
  stepper.disableOutputs();
}

void MotorController::OnRun()
{
  if (millis() - error_check_timer > error_check_period)
  {
    error_check_timer = millis();
    CheckForErrors();
  }

  switch (controlMode)
  {
  case MotorStates::OFF:
    break;
  case MotorStates::POSITION:
    stepper.run();
    //if (stepper.distanceToGo() == 0)
    //  stepper.disableOutputs();
    break;
  case MotorStates::VELOCITY:
    stepper.runSpeed();
    if(target_velocity_duration!=0 && (millis()-target_velocity_timer)>target_velocity_duration)
      controlMode = MotorStates::OFF;
    break;
  case MotorStates::IDLE:
    break;
  }
}

void MotorController::SetMotorState(MotorStates state)
{
  controlMode = state;
  switch(controlMode){
    case MotorStates::OFF:
      stepper.disableOutputs();
      break;
    case MotorStates::POSITION:
      stepper.enableOutputs();
      break;
    case MotorStates::VELOCITY:
      stepper.enableOutputs();
      break;
    case MotorStates::IDLE:
      stepper.disableOutputs();
      break;
  }
}

MotorStates MotorController::GetMotorState()
{
  return controlMode;
}

void MotorController::SetMotorBraking(MotorBrake brake)
{
  motorBrake = brake;
  driver.setStandstillMode((TMC2209base::StandstillMode)brake);
  DEBUG_PRINTF("Setting Motor Brake: %d\n", brake);
}

MotorBrake MotorController::GetMotorBraking()
{
  return (MotorBrake)driver.pwm_config_.freewheel;
}

void MotorController::SetMaxSpeed(uint32_t spd)
{
  stepper.setMaxSpeed(spd);
}

uint32_t MotorController::GetMaxSpeed()
{
  return stepper.maxSpeed();
}

void MotorController::SetAcceleration(uint32_t acl)
{
  stepper.setAcceleration(acl);
}

uint32_t MotorController::GetAcceleration()
{
  return stepper.acceleration();
}

void MotorController::SetPositionTargetRelative(double position)
{
  SetMotorState(MotorStates::POSITION);
  target_position += position;
  stepper.moveTo(target_position);
}

void MotorController::SetPositionTarget(double position)
{
  SetMotorState(MotorStates::POSITION);
  target_position = position;
  stepper.moveTo(position);
}

double MotorController::GetPositionTarget()
{
  return target_position;
}

void MotorController::SetVelocityTarget(double velocity)
{
  stepper.enableOutputs();
  target_velocity = velocity;
  stepper.setSpeed(velocity);
}

double MotorController::GetVelocityTarget()
{
  return target_velocity;
}

void MotorController::setEncoderValueSource(IEncoderInterface *encoder_value)
{
  encoder_ptr = encoder_value;
}

uint32_t MotorController::GetErrors()
{
  return error_flag;
}

void MotorController::PrintErrorsToSerial()
{
  auto stat = driver.getStatus();
  auto gstat = driver.getGlobalStatus();
  DEBUG_PRINTF("reset: %u\n", gstat.reset);
  DEBUG_PRINTF("drv_error: %u\n", gstat.drv_err);
  DEBUG_PRINTF("uv_cp: %u\n", gstat.uv_cp);
  DEBUG_PRINTF("over_temperature_warning: %u\n", stat.over_temperature_warning);
  DEBUG_PRINTF("over_temperature_shutdown: %u\n", stat.over_temperature_shutdown);
  DEBUG_PRINTF("short_to_ground_a: %u\n", stat.short_to_ground_a);
  DEBUG_PRINTF("short_to_ground_b: %u\n", stat.short_to_ground_b);
  DEBUG_PRINTF("low_side_short_a: %u\n", stat.low_side_short_a);
  DEBUG_PRINTF("low_side_short_b: %u\n", stat.low_side_short_b);
  DEBUG_PRINTF("open_load_a: %u\n", stat.open_load_a);
  DEBUG_PRINTF("open_load_b: %u\n", stat.open_load_b);
  DEBUG_PRINTF("over_temperature_120c: %u\n", stat.over_temperature_120c);
  DEBUG_PRINTF("over_temperature_143c: %u\n", stat.over_temperature_143c);
  DEBUG_PRINTF("over_temperature_150c: %u\n", stat.over_temperature_150c);
  DEBUG_PRINTF("over_temperature_157c: %u\n", stat.over_temperature_157c);
  DEBUG_PRINTF("reserved0: %u\n", stat.reserved0);
  DEBUG_PRINTF("current_scaling: %u\n", stat.current_scaling);
  DEBUG_PRINTF("reserved1: %u\n", stat.reserved1);
  DEBUG_PRINTF("stealth_chop_mode: %u\n", stat.stealth_chop_mode);
  DEBUG_PRINTF("standstill: %u\n", stat.standstill);
}

MotorController motorController(0);