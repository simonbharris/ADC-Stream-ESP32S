#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_ADS1015.h>

// Delay between the swap in conversions between channels.
#define READ_DELAY 720

// Number of ADSs to use (1-4)
#define NUM_ADS 4

// Debug Opts: Use up to one
#define DEBUG_PRINT // prints values from channels
//#define DEBUG_SPS // prints sps

//| **All below obervations on Serial only, no WiFi
// Intestingly, ADDR->SDA first seems to reduce the static of this ADS1115 chip.
// Otherwise it's quite noticable different. All signals seem to float between -4 and +15, with some noise reaching a bit further.
Adafruit_ADS1115 ads[4] = {            //| n: sps (channels = n*2) @1000 delay
  Adafruit_ADS1115(0x4A), // ADDR to SDA | 1: 400 | delay @950 is too fast
  Adafruit_ADS1115(0x49), // ADDR to VDD | 2: 335
  Adafruit_ADS1115(0x4B), // ADDR to SCL | 3: 287
  Adafruit_ADS1115(0x48), // ADDR to GND | 4: 252 | delay @720 works for 293sps
};

// Sets all ads1115 devices to read
// Select:
//    0: Differential_0_1
//    1: Differential_2_3
void assignAll_ADS_diff(int select)
{
  if (select == 0)
    for (int i = 0; i < NUM_ADS; i++)
      ads[i].setupContinuousADC_Differential_0_1();
  else if (select == 1)
    for (int i = 0; i < NUM_ADS; i++)
      ads[i].setupContinuousADC_Differential_2_3();

  // Allow time for device to update with new values
  delayMicroseconds(READ_DELAY);
}

// Applies desired ads setup to all ads1115s in global
void setupAllAds()
{
  for (int i = 0; i < NUM_ADS; i++)
  {
    ads[i].begin();
    ads[i].setGain(GAIN_SIXTEEN);
    ads[i].setSampleRate(ADS1115_SPS860);
  }
}

// Gets and prints all conversion values stored in ads1115 chips.
void print_values()
{
  int val[4];

  // Read from chips and store values
  for (int i = 0; i < NUM_ADS; i++)
    val[i] = ads[i].getLastConversionResults();

  #ifdef DEBUG_PRINT
  // Print comma-seperated values to the screen.
  for (int i = 0; i < NUM_ADS; i++)
  {
    Serial.print(val[i]);
    Serial.print(",");
  }
  #endif
}

void setup() {
  Serial.begin(115200);
  setupAllAds();
  Wire.setClock(400000);

long timer = millis();
void loop() {
  static int i;
  int val[4];

  // Read and record all values from diff_0_1
  assignAll_ADS_diff(0);
  print_values();

  // read and record all values from diff_2_3
  assignAll_ADS_diff(1);
  print_values();

  // DEBUG
  #ifdef DEBUG_SPS
  i++;
  if (timer + 1000 < millis())
  {
    Serial.println(i);
    timer = millis();
    i = 0;
  }
  #endif
  #ifdef DEBUG_PRINT
  Serial.println();
  #endif
}
