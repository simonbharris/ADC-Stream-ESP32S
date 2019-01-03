#include <WiFi.h>
#include <Adafruit_ADS1015.h>
#include <WebSocketServer.h>

// Maximum packet size for ADC to send over websocket
#define BUFFSIZE 3000

// Maximum time to wait between sending packets (in milliseconds)
#define INTERVAL 100

// Adafruit ads1115 ADC setup
Adafruit_ADS1115 adsSensor(0x48);

// Access Point ssid + password
const char* ssid = "ESP32-Access-Point";
const char* password = "6788";

// AP + Web server setup
WiFiServer server(80);
WebSocketServer wsServer;

float avg(int16_t *arr, int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += arr[i];
  return ((float)sum / n);
}

void setup() {
  // Begin Serial output for debugging.
  Serial.begin(115200);
  
  // Starting the ADC (ADS1115 board)
  adsSensor.begin();
  
  // Setting up ADS 1115 config
  
  // Highest gain (FSR = +-0.256v) with 860 sps
  adsSensor.setGain(GAIN_SIXTEEN);
  
  // Setups ADS1115 in continuous reading mode on a differential between pins 0 and 1.
  // Continuous mode allows for faster reading.
  adsSensor.setupContinuousADC_Differential_0_1();
  
  //adsSensor.setGain(GAIN_SIXTEEN);

  // Setting i2c in full mode.
  Wire.setClock(400000);

  // Initializing Access point
  Serial.print("Setting AP");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);
  server.begin();
}

void loop() {
 long timer;
 int i;
 int16_t datapacket[BUFFSIZE];
 int16_t avgarr[20];
 int16_t val;
 
  // Check if we have a client.
  WiFiClient client = server.available();
    // Attempt websocket handshake.
    if (client.connected() && wsServer.handshake(client))
    {

      // While client is still connected.
      while (client.connected()) 
      {
        i = 0;
        timer = millis();
        
        while(client.connected())
        {
          // Do an ADC read (Differential on A0 and A1
          val = adsSensor.getLastConversionResults();
          
          // Add read to an array for averaging
          avgarr[i % 20] = val;
          
          //Record the current average.
          datapacket[i++] = avg(avgarr, 20);

          // Debug, print to serial
          Serial.println(datapacket[i-1]);
          
          // A more decaying average result for testing.
          //datapacket[i++] = (0.2 * val) + (0.8 * avg(avgarr, 20));
          
          // Sends a datapacket at every interval, or when the buffer is full. ads1115 should send every 100ms in 85 or 86 16-bit integer packets.
          if (timer + INTERVAL < millis() || BUFFSIZE <= i)
          {
             wsServer.sendData(datapacket, i);
             //Serial.println(i);
             // Reset values
             i = 0;
             timer = millis();
          }
        }
      }
    }
}

