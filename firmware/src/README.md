# Software Architecture #

I know this code looks complex, but its visual complexity enables the long list of control interfaces, peripherals, and functionality. Heres a diagram of roughly how ot works.

```mermaid
graph TD;
    IExternalInterface["`
    **<<**Interface**>>**
    ---------------------
    IExternalInterface
    `"]

    IProcessorInterface["`
    **<<**Interface**>>**
    ---------------------
    IProcessorInterface
    `"]

    SPIInterface["`
    SPI Interface
    SendMsg();
    `"]

    EthernetInterface["`
    Ethernet Interface
    SendMsg();
    `"]

    SerialInterface["`
    Serial Interface
    SendMsg();
    `"]

    MessageProcessor["`
    MessageProcessor
    SendMsg();
    `"]

    MotorPeripheral["`
    MotorPeripheral
    `"]


    IExternalInterface-.->SerialInterface
    IExternalInterface-.->EthernetInterface
    IExternalInterface-.->SPIInterface
    MessageProcessor-.->IProcessorInterface

    SerialInterface~~~IProcessorInterface

    IProcessorInterface-.->MesssageProcessor

    SerialInterface-->MessageProcessor
    
```

```mermaid
classDiagram
  class IExternalInterface{
          + IProcessorInterface *processor_interface_
          + SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size)
          + SetProcessorInterface(IProcessorInterface *proc_interface)
      }
  class IProcessorInterface{
      + SendMsg(uint8_t *send_bytes, uint32_t send_bytes_size) = 0;
      + HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size) = 0;
  }
  class IInternalInterface{
    + IProcessorInterface *processor_interface_
    HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
  }
```
  