#include <WiFi.h>
#include <Adafruit_ADS1015.h>
#include <WebSocketServer.h>

// For recognizing gestures, electrodes on pins 01 positioned on top of forearm towards elbow
// pin 23 positions top of forearm near wrist.
// Illustration shown in figure one at the following site.
// http://file.scirp.org/Html/2-70246_30987.htm 
// Electrodes positioned at set 1 and set 3
// Negative positions (pin 1, 3) are the set closest to the hand.


// Maximum packet size for ADC to send over websocket
#define BUFFSIZE 3000

// Maximum time to wait between sending packets (in milliseconds)
#define MAX_PACKET_SIZE 80
#define INTERVAL 100

// Adafruit ads1115 ADC setup
Adafruit_ADS1115 adsSensor(0x48);

// Access Point ssid + password
const char* ssid = "ESP32-Access-Point";
const char* password = "6788";

// AP + Web server setup
WiFiServer server(80);
WebSocketServer wsServer;

int16_t avg(int16_t *arr, int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += arr[i];
  return (sum / n);
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
  //adsSensor.setupContinuousADC_Differential_0_1();
  
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
 int16_t avgarr1[20];
 int16_t avgarr2[20];
 int16_t val1;
 int16_t val2;
 
  // Check if we have a client.
  WiFiClient client = server.available();
    // Attempt websocket handshake.
    if (client.connected() && wsServer.handshake(client))
    {

      // if a client connects
      if (client.connected()) 
      {
        i = 0;
        timer = millis();
        
        // pre-fill averaging arrays so we don't get very random values at the start of our sampling.
        for (int j = 0; j < 20; j++)
        {
          avgarr1[j] = adsSensor.readADC_Differential_0_1();
          avgarr2[j] = adsSensor.readADC_Differential_2_3();
        }
        
        while(client.connected())
        {
          // Do an ADC read (Differential on A0 and A1
          val1 = adsSensor.readADC_Differential_0_1();
          val2 = adsSensor.readADC_Differential_2_3();

          Serial.println(val1);
          // Add read to an array for averaging
          avgarr1[(i/2) % 20] = val1;
          avgarr2[(i/2) % 20] = val2;
          
          // Very basic decaying average version.
          datapacket[i++] = (0.2 * val1) + (0.8 * avg(avgarr1, 20));
          datapacket[i++] = (0.2 * val2) + (0.8 * avg(avgarr2, 20));

          // Sends a datapacket at every interval, or when the buffer is full. ads1115 should send every 100ms in 85 or 86 16-bit integer packets.
          if (MAX_PACKET_SIZE < i)
          {
             wsServer.sendData(datapacket, i);
             
             //Serial.println(i);
             // Reset values
             i = 0;
             //Debug: time between packets in milliseconds
             //Serial.println(millis() - timer);
             timer = millis();
             delayMicroseconds(2);
          }
        }
      }
    }
}

