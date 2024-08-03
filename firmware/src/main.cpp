#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "Task.h"
#include "LedController/LedController.h"
#include "SerialTextInterface/SerialTextInterface.h"
#include "ExternalInterface.h"
#include "Messages.h"
#include <cstdint>

/*
TaskManager
  Handles tasking
  Calls all registered tasks

IControlInterface
  Inhereted by classes that manage an external interface (Serial, Wifi, I2c, Uart)
  Passes bytes down to MessageProcessor
  Receives Bytes from MessageProcessor to send to send out of interface
  Each interface will have a buffer whose pointer gets passed down

MessageProcessor 
  Maintains a list of Controller classes 
  Maintains a list of pointers to a control interfaces
  Calls various control functions based on what functionality they control 
  Allows functionControllers to send data back to interface

*/
struct InterfaceLimits{
  ISenderInterface* interface;
  uint16_t lowLimit;
  uint16_t highLimit;
};

class MessageProcessor : public ITask, public ISenderInterface{
private:
  std::vector<ISenderInterface*> externalInterfaces;   // Vector to hold pointers to interfaces
  std::vector<InterfaceLimits> controllerInterfaces;   // Vector to hold pointers to interfaces
public:
    MessageProcessor(uint32_t period){
      executionPeriod = period;
    }

    void AddExternalInterface(ISenderInterface* new_interface){
      externalInterfaces.push_back(new_interface);
      new_interface->SetProcessorInterface(this);
    }

    void AddControllerInterface(ISenderInterface* new_interface, MessageTypes low, MessageTypes high){
      InterfaceLimits interface_to_add;
      interface_to_add.highLimit = (uint16_t)high;
      interface_to_add.lowLimit = (uint16_t)low;
      interface_to_add.interface = new_interface;
      controllerInterfaces.push_back(interface_to_add);
      new_interface->SetProcessorInterface(this);
    }

    void OnStart() override {
      
    }

    void OnStop() override {
    }

    void OnRun() override {
        
    }

    void HandleIncomingMsg(uint8_t* recv_bytes, uint32_t recv_bytes_size = 0){
      Serial.println("message manager handling message: ");
      for(int i = 0; i<recv_bytes_size; i++){
        Serial.printf("%2X, ", recv_bytes[i]);
      }
      Serial.println();

      Header* hdr = (Header*)&recv_bytes[0];

      for(InterfaceLimits interface : controllerInterfaces){
        if((uint16_t)hdr->message_type > interface.lowLimit && (uint16_t)hdr->message_type < interface.highLimit){
          Serial.println("Sending to led");
          interface.interface->HandleIncomingMsg(recv_bytes, recv_bytes_size);
        }else{
          Serial.println("Invalid message limits");
        }
      }
      
    }

    void SendMsg(uint8_t* send_bytes, uint32_t send_bytes_size){

    }
};

void pinMode(EPortType port, uint32_t pin, uint32_t pinMask, uint32_t ulMode )
{
  // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN);
      PORT->Group[port].DIRCLR.reg = pinMask;
    break;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
      PORT->Group[port].DIRCLR.reg = pinMask;

      // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')
      PORT->Group[port].OUTSET.reg = pinMask;
    break;

    case INPUT_PULLDOWN:
      // Set pin to input mode with pull-down resistor enabled
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
      PORT->Group[port].DIRCLR.reg = pinMask;

      // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
      PORT->Group[port].OUTCLR.reg = pinMask;
    break;

    case OUTPUT:
      // enable input, to support reading back values, with pullups disabled
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);

      // Set pin to output mode
      PORT->Group[port].DIRSET.reg = pinMask;
    break;

    default:
      // do nothing
    break;
  }
}

void digitalWrite(EPortType port, uint32_t pin, uint32_t pinMask, uint32_t ulVal )
{
  if ( (PORT->Group[port].DIRSET.reg & pinMask) == 0 ) {
    // the pin is not an output, disable pull-up if val is LOW, otherwise enable pull-up
    PORT->Group[port].PINCFG[pin].bit.PULLEN = ((ulVal == LOW) ? 0 : 1) ;
  }

  switch ( ulVal )
  {
    case LOW:
      PORT->Group[port].OUTCLR.reg = pinMask;
    break ;

    default:
      PORT->Group[port].OUTSET.reg = pinMask;
    break ;
  }

  return ;
}


class StatusLedDriver : public ITask{
private:
  uint32_t led_timer = 0;
  bool led_state = 0;
public:
    StatusLedDriver(uint32_t period){
      executionPeriod = period;
    }

    void OnStart() override {
       pinMode(PORTB, 13, PORT_PB13, OUTPUT);
    }

    void OnStop() override {
    }

    void OnRun() override {
        digitalWrite(PORTB, 13, PORT_PB13, led_state = !led_state);
    }
};



TaskManager manager;
SerialTextInterface serialTextInterface(0);
MessageProcessor messageProcessor(0);
LedController ledController(40);
StatusLedDriver statusLight(1000);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Axis");
//connect the SerialTextInterface to the Message Processor
  messageProcessor.AddControllerInterface(&ledController, MessageTypes::LedControlMessageTypeLowerBounds, MessageTypes::LedControlMessageTypeUpperBounds);
  messageProcessor.AddExternalInterface(&serialTextInterface);
  manager.AddTask(&serialTextInterface);
  manager.AddTask(&messageProcessor);
  manager.AddTask(&statusLight);
  manager.AddTask(&ledController);
//start the interfaces
  serialTextInterface.Start();

//start the message processor
  messageProcessor.Start();
  ledController.Start();
  statusLight.Start();
}

void loop() {
  manager.RunTasks();
}