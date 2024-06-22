#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "FastLED.h"
#include "Task.h"

#define STEP_PIN 6
#define DIR_PIN 5
#define LIMIT_PIN A0
//Uart Serial2(&sercom3, SCL, SDA, SERCOM_RX_PAD_1, UART_TX_PAD_0);
//void SERCOM3_Handler()
//{
//  Serial2.IrqHandler();
//}
//HardwareSerial &serial_stream = Serial2;

//#include <AccelStepper.h>
//AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);


#define EN_PIN           11 // Enable
//#define DIR_PIN          55 // Direction
//#define STEP_PIN         54 // Step
//#define CS_PIN           42 // Chip select
//#define SW_MOSI          66 // Software Master Out Slave In (MOSI)
//#define SW_MISO          44 // Software Master In Slave Out (MISO)
//#define SW_SCK           64 // Software Slave Clock (SCK)
//#define SW_RX            63 // TMC2208/TMC2224 SoftwareSerial receive pin
//#define SW_TX            40 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define SERIAL_PORT Serial2 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

// Select your stepper driver type
//TMC2130Stepper driver(CS_PIN, R_SENSE);                           // Hardware SPI
//TMC2130Stepper driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK); // Software SPI
//TMC2660Stepper driver(CS_PIN, R_SENSE);                           // Hardware SPI
//TMC2660Stepper driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);
//TMC5160Stepper driver(CS_PIN, R_SENSE);
//TMC5160Stepper driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);

//TMC2208Stepper driver(&SERIAL_PORT, R_SENSE);                     // Hardware Serial
//TMC2208Stepper driver(SW_RX, SW_TX, R_SENSE);                     // Software serial
//TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
//TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);




class LEDTask : public ITask {
private:
  uint16_t colorWheelAngle = 0;
  CRGB leds[1];
public:
    LEDTask(uint32_t period){
      executionPeriod = period;
    }

    void OnStart() override {
      leds[0] = 0;
      FastLED.addLeds<NEOPIXEL, NEOPIXEL_BUILTIN>(leds, NEOPIXEL_BUILTIN);
    }

    void OnStop() override {
    }

    template <typename T>
    T Wrap360(T input){
      if(input>360)
        return input-360;
      if(input<0){
        return input+360;
      }
      return input;
    }

    void OnRun() override {
        const uint16_t b = 10;
        const uint16_t brightness = b/2;
        executionPeriod = 50;
        colorWheelAngle += 1;
        colorWheelAngle%=360;
        
        leds[0].r = brightness*cos(radians(Wrap360(colorWheelAngle+0)))+brightness;
        leds[0].g = brightness*cos(radians(Wrap360(colorWheelAngle+120)))+brightness;
        leds[0].b = brightness*cos(radians(Wrap360(colorWheelAngle+240)))+brightness;
        // switch(flashState){
        //   case 0:
        //     leds[0] = 0x010002;
        //     flashState++;
        //   break;
        //   case 1:
        //     leds[0] = 0x010000;
        //     flashState=0;
        //   break;
        // }
        FastLED.show();
    }
};

class PrintTask : public ITask {
private:
public:
    PrintTask(uint32_t period){
      executionPeriod = period;
    }

    void OnStart() override {
    }

    void OnStop() override {
    }

    void OnRun() override {
        Serial.printf("Time: %d\n", millis());
    }
};

TaskManager manager;
LEDTask task(1000);
PrintTask printer(5000);
void setup() {
  Serial.begin(9600);
  /*pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(LIMIT_PIN, INPUT_PULLUP);
  digitalWrite(EN_PIN, LOW);      // Enable driver in hardware

                                  // Enable one according to your setup
 //SPI.begin();                    // SPI drivers
 SERIAL_PORT.begin(9600);      // HW UART drivers
 //driver.beginSerial(115200);     // SW UART drivers

  driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.rms_current(1200);        // Set motor RMS current
  driver.microsteps(16);          // Set microsteps to 1/16th

//driver.en_pwm_mode(true);       // Toggle stealthChop on TMC2130/2160/5130/5160
driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
  driver.pwm_autoscale(true);     // Needed for stealthChop


  stepper.setMaxSpeed(1000000); 
  stepper.setAcceleration(500000); 
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();

  stepper.setSpeed(-10000);
  while(digitalRead(LIMIT_PIN)){
    stepper.runSpeed();
  }
  stepper.stop();
  stepper.setCurrentPosition(0);
  stepper.runToNewPosition(100);
  delay(2000);*/


  manager.AddTask(&task);
  manager.AddTask(&printer);
  task.Start();
  printer.Start();
}

void loop() {
  manager.RunTasks();
  // stepper.runToNewPosition(0);
  // stepper.runToNewPosition(10000);
}