#ifndef ESP8266SEMG
# define ESP8266SEMG

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_ADS1015.h>

// !!! DEBUG OPTIONS: Use up to one
//#define DEBUG_PRINT // prints values from channels to serial (Intestingly, only ~1sps drop enabled.)
//#define DEBUG_SPS // prints sps to Serial

#define USE_SERIAL Serial
#define BUFFSIZE 5200 // In bytes
#define INTERVAL 200 // Interval between packets, in (ms)

// Wireless settings
#define SSID_HOSTNAME "ESP8266-sEMG"
#define SSID_PASS     "67886788"

// Delay between the swap in conversions between channels.
#define READ_DELAY 725

// Number of ADSs to use (1-4)
#define NUM_ADS 4


// Functs found in (ads_util)
void assignAll_ADS_diff(int select);
void setupAllAds();
void print_values();

// Functs found in (websocket_util)
void init_AP_websocket(char *ssid, char *password);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void fillBuff(uint8_t BUFF[BUFFSIZE], int16_t *val, int *i);

#endif
