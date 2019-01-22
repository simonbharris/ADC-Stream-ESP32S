#include "esp8266semg.h"

// Check very bottom for notes on observed performance

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
void get_values(int16_t val[4])
{
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

/*
* All obervations here are on Serial only, no WiFi
*
* Intestingly, ADDR->SDA first seems to reduce the static of this ADS1115 chip.
* Otherwise it's quite a noticable difference. All signals seem to float between -4 and +15, with some noise reaching a bit further.
*            
*     Using i2c Fast Mode (400kbit)
*                        | n: sps (channels = n*2) @1000 delay
*            ADDR to SDA | 1: 400 | delay @950 is too fast
*            ADDR to VDD | 2: 335
*            ADDR to SCL | 3: 287
*            ADDR to GND | 4: 252 | delay @720 works for 293sps
*
*    WiFi enabled, I am able to acquire about 285sps on 4 devices, 8 total channels.
*/