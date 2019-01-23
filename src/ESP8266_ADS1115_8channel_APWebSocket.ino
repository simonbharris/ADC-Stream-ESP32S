#include "esp8266semg.h"

/*
 * Hardware:
 * Wemos D1 mini v2.1.0 (Uses an ESP8266 WiFi module)
 * 4x ADS1115 ADC
 * 
 * Purpose of this code is to obtain as high Samples per Second(sps) as possible
 * With as many channels as possible.
 * 
 * My particular case uses electtrodes on ADS1115 Analog pins to perform a
 * surface electromyography of the lower arm and record the data to detect gesture recognition.
 */

// NUM_ADS will use the top most declared first, then go down the list.
// So if you use only 1 ADS, be sure the first in the list is set to the ADDR you want to use.
Adafruit_ADS1115 ads[4] = {
  Adafruit_ADS1115(0x4A), // ADDR to SDA
  Adafruit_ADS1115(0x49), // ADDR to VDD
  Adafruit_ADS1115(0x4B), // ADDR to SCL
  Adafruit_ADS1115(0x48), // ADDR to GND // Approximately 285sps when broadcasting WiFi
};

WebSocketsServer webSocket = WebSocketsServer(80);

void setup() {
  Serial.begin(115200);

  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for(uint8_t t = 4; t > 0; t--) {
      USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
      USE_SERIAL.flush();
      delay(1000);
  }

  setupAllAds();
  Wire.setClock(400000);
  init_AP_websocket(SSID_HOSTNAME, SSID_PASS);
}

long timer = millis();
uint8_t buff[BUFFSIZE];
void loop() {
  static int i;
  int16_t val[4];

  webSocket.loop();
  
  // Read and record all values from diff_0_1
  assignAll_ADS_diff(0);
  get_values(val);
  fillBuff(buff, val, &i);
  
  // read and record all values from diff_2_3
  assignAll_ADS_diff(1);
  get_values(val);
  fillBuff(buff, val, &i);

  // Send data every INTERVAL milliseconds
  if (timer + INTERVAL < millis())
  {
    #ifdef DEBUG_SPS
    Serial.println(i*(1000/INTERVAL)/16);
    #endif

    // Send packet and reset counters
    webSocket.broadcastBIN(buff, i);
    timer = millis();
    i = 0;
  }

  #ifdef DEBUG_PRINT
  Serial.println();
  #endif
}

