# ADC-Stream-ESP32S
Using a NodeMCU ESP32S board to stream data received by an ADS1115. Intent is to detect sEMG readings to identify hand gestures.

# Dependencies
The included dependencies contain library(ies) that have been modified to suit my needs. These are not the same as the original repositories. An overview of the changes are listed here

* Adafruit_ADS1015
  - Modified so that sample rate can be changed by a public function
  - Now delays by microseconds, controlled by a macro in the header file
  - Contains enums for SPS rates of both 1115 and 1015 devices
  - Added functionality to write to the ADS chips without without a delay or a read.
  --  Used properly, this can help promote asynchronous functionality.
