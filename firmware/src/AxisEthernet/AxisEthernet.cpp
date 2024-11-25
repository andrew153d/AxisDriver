#include "AxisEthernet.h"
#include "utility/w5100.h"
#include "Site.h"
#include "../EncoderController/EncoderController.h"
#include "../LedController/LedController.h"
void HandleInturrupts();


void AxisEthernet::OnStart()
{
    pinMode(PMODE0, OUTPUT);
    pinMode(PMODE1, OUTPUT);
    pinMode(PMODE2, OUTPUT);
    digitalWrite(PMODE0, HIGH);
    digitalWrite(PMODE1, HIGH);
    digitalWrite(PMODE2, HIGH);

    pinMode(nRST, OUTPUT);
    digitalWrite(nRST, HIGH);

    pinMode(nINT, INPUT);

    pinMode(nCS, OUTPUT);
    digitalWrite(nCS, LOW);

    Ethernet.init();
    Ethernet.begin(mac, ip);

    //Serial.println(W5100.readSIR());
    //W5100.writeSIR(0x00); // clear inturrupts
    ///W5100.writeSIMR(0xFF);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        // while (true)
        // {
        //     delay(1); // do nothing, no point running without Ethernet hardware
        // }
    }
    else{
    }
    return;
    if (Ethernet.linkStatus() == LinkOFF)
    {
        Serial.println("Ethernet cable is not connected.");
    }

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    //interrupts();
    attachInterrupt(AUX4, HandleInturrupts, RISING);
}

void AxisEthernet::HandleInturrupt()
{

    Serial.println("int");
    return;
}

void HandleInturrupts()
{
    AEthernet.HandleInturrupt();
}
void AxisEthernet::OnRun()
{
    EthernetClient client = server.available();
    if (client) {
    String request = "";
    
    // Read the HTTP request from the client
    while (client.available()) {
      char c = client.read();
      request += c;
    }
    
    // Debugging: Print the request to Serial Monitor
    //Serial.println(request);
    
    // If the client requests the root URL ("/"), send the HTML page
    if (request.indexOf("GET / ") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println(html_page);
    }
    
    // If the client requests the "/get-number" endpoint, return a random number
    else if (request.indexOf("GET /get-number") >= 0) {
      int randomNumber = random(1, 101);  // Random number between 1 and 100
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println(encoderController.GetPositionDegrees());
    }

    // Close the client connection after handling the request
    delay(1);
    client.stop();
  }
}


void AxisEthernet::HandleIncomingMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // if (recv_bytes_size == 0)
    //     return;

    // if ((char)recv_bytes[0] == '{')
    //     HandleJsonMsg(recv_bytes, recv_bytes_size);
    // else
    //     HandleByteMsg(recv_bytes, recv_bytes_size);
}

void AxisEthernet::HandleJsonMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // DeserializationError error = deserializeJson(recvd_json, recv_bytes, 256);
    // if (error)
    // {
    //     Serial.println("Error parsing json in motor controller");
    //     return;
    // }

    // String command = "";

    // if (!TryParseJson<String>("mode", &command, recvd_json))
    // {
    //     Serial.println("Failed to parse encoder mode");
    //     return;
    // }

    // EncoderCommands encoder_command = ToEncoderCommandsEnum(command);

    // switch (encoder_command)
    // {
    // case EncoderCommands::SetPosition:
    // {
    //     float offset = 0;
    //     if (!TryParseJson<float>("position", &offset, recvd_json))
    //     {
    //         Serial.println("Failed to parse position");
    //         return;
    //     }
    //     SetPosition(offset);
    //     break;
    // }
    // case EncoderCommands::GetPosition:
    // {
    //     recvd_json.clear();
    //     recvd_json["type"] = ENCODERTYPESTRING;
    //     recvd_json["mode"] = GETPOSITIONCOMMAND;
    //     recvd_json["position"] = GetShaftAngle();
    //     String output;
    //     serializeJson(recvd_json, output);
    //     Serial.println(output);
    //     break;
    // }
    // case EncoderCommands::GetUpdateRate:
    // {

    //     recvd_json.clear();
    //     recvd_json["type"] = ENCODERTYPESTRING;
    //     recvd_json["mode"] = GETUPDATERATECOMMAND;
    //     recvd_json["rate"] = GetUpdateRate();
    //     String output;
    //     serializeJson(recvd_json, output);
    //     Serial.println(output);
    //     break;
    // }
    // }
}

void AxisEthernet::HandleByteMsg(uint8_t *recv_bytes, uint32_t recv_bytes_size)
{
    // if (recv_bytes_size < HEADER_SIZE + FOOTER_SIZE)
    // {
    //     Serial.println("Size too small");
    // }

    // Header *header = (Header *)recv_bytes;
    // if (recv_bytes_size < HEADER_SIZE + header->body_size + FOOTER_SIZE)
    // {
    //     Serial.printf("Invalid body size: recv_bytes:%d, header_size:%d, body_size:%d, footer_size:%d\n", recv_bytes_size, HEADER_SIZE, header->body_size, FOOTER_SIZE);
    //     return;
    // }

    // //   switch (header->message_type)
    // //   {
    // //   break;
    // //   default:
    // //     break;
    // //   }
}

void AxisEthernet::OnStop()
{

}

AxisEthernet AEthernet(10);
