#include "esp8266semg.h"

/*
* ESP8266 cannot connect to Ad Hoc networks 
* (IE: A Mac creating a network to connect directly with another)
* 
* This limits us to having the ESP8266 host as an Access Point(AP)
* Or using router or other external point to connect for us.
* 
* The Robotics lab does not get a very good signal o the campus wireless network
* (about -78db signal strength, which is poor for streaming)
*/

// Initializees the access point and starts the websocket service and event.
void init_AP_websocket(char *ssid, char *password){
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);
  Serial.print("ssid: ");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

// Different behaviors depending on status or received packet.
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
        // send message to client
        //webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
             //webSocket.sendTXT(num, "message here");

            // send data to all connected clients
             //webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }
}

// Inserts val into BUFF, starting with the ith element.
// Function returns the updated index and buffer
void fillBuff(uint8_t BUFF[BUFFSIZE], int16_t val[4], int *i)
{
  for (int j = 0; j < NUM_ADS; j++)
  {
    buff[*i] = (val[j] >> 0) & 0xff;
    *i += 1;
    buff[*i] = (val[j] >> 8) & 0xff;
    *i += 1;
  }
}

