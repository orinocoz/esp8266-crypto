/*
 * ESP8266 Crypto
 * 
 * A program to display various cryptocurrency prices on an
 * OLED display using an ESP8266
 * 
 * Conor Walsh 2021-02-20
 * conor.engineer
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

/**** Setup ****/

// WIFI Network name and password
#define WIFI_SSID "YOUR_NETWORK"
#define WIFI_PASS "YOUR_PASSWORD"

// Setup time in seconds
#define SETUP_TIME 4

// Update screen every __ seconds
#define UPDATE_TIME 10

// Fingerprint for api.cryptonator.com - expires 6 Feb 2022
const uint8_t fingerprint[20] = {0x10, 0x76, 0x19, 0x6B, 0xE9, 0xE5, 0x87, 0x5A, 0x26, 0x12, 0x15, 0xDE, 0x9F, 0x7D, 0x3B, 0x92, 0x9A, 0x7F, 0x30, 0x13};

/** End Setup **/

ESP8266WiFiMulti WiFiMulti;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// All crypto logos must be 48x48
#define LOGO_WIDTH 48
#define LOGO_HEIGHT 48

// Struct to store a crypto
struct crypto { 
   String coin;
   String url;
   const unsigned char logo [288];
};

// Declare all cryptos used
// To add a new coin
//  1: Add a new setup similar to one shown in the examples
//  2: Find the api.cryptonator.com api url for the coin
//  3: Create a 48x48 bitmap logo for the coin using http://javl.github.io/image2cpp/
crypto cryptos[] = {
    // Doge setup
    {"Doge",
     "https://api.cryptonator.com/api/ticker/doge-eur",
      {0x03, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x07, 0xc0, 0x00, 0x03, 0xf8, 0x00, 0x0f, 0xe0, 0x00, 0x07, 
      0xf8, 0x00, 0x0f, 0xe0, 0x00, 0x0f, 0xfc, 0x00, 0x1f, 0xf0, 0x00, 0x3f, 0x3c, 0x00, 0x1e, 0xf8, 
      0x00, 0x7e, 0x38, 0x00, 0x1e, 0x78, 0x00, 0xfc, 0x38, 0x00, 0x1c, 0x3c, 0x03, 0xff, 0x38, 0x00, 
      0x3c, 0x3f, 0xff, 0xff, 0xf8, 0x00, 0x3c, 0x7f, 0xff, 0xff, 0xf8, 0x00, 0x3c, 0xff, 0xff, 0x83, 
      0xf8, 0x00, 0x38, 0x78, 0x04, 0x01, 0xfc, 0x00, 0x78, 0x20, 0x00, 0x00, 0xfe, 0x00, 0x78, 0x00, 
      0x00, 0x00, 0x3f, 0x00, 0x78, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x78, 0x00, 0x00, 0x00, 0x07, 0xc0, 
      0x78, 0x00, 0x00, 0x01, 0xe3, 0xe0, 0x78, 0x00, 0x00, 0x03, 0xe1, 0xe0, 0x78, 0x00, 0x78, 0x03, 
      0xf1, 0xf0, 0x78, 0x00, 0x7c, 0x03, 0xe0, 0xf0, 0x78, 0x00, 0xfc, 0x01, 0xe0, 0x78, 0xf0, 0x00, 
      0x7c, 0x00, 0x00, 0x78, 0xf0, 0x00, 0x38, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3c, 
      0xe0, 0x00, 0x00, 0x00, 0x00, 0x1c, 0xe0, 0x00, 0x00, 0x00, 0xf8, 0x1e, 0xe0, 0x00, 0x00, 0x03, 
      0xf8, 0x1e, 0xe0, 0x00, 0x00, 0x03, 0xfc, 0x1e, 0xe0, 0x00, 0x00, 0x01, 0xf8, 0x1e, 0xe0, 0x00, 
      0x00, 0x00, 0xf8, 0x1e, 0xf0, 0x00, 0x00, 0x00, 0x70, 0x1e, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1e, 
      0x78, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x00, 0x00, 0x00, 0x30, 0x1e, 0x7c, 0x00, 0x00, 0x38, 
      0xf8, 0x1e, 0x3e, 0x00, 0x00, 0x3f, 0xf8, 0x1e, 0x1e, 0x00, 0x00, 0x7f, 0xf0, 0x3c, 0x1f, 0x00, 
      0x00, 0x3f, 0xc0, 0x3c, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x78, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x78, 
      0x03, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xfc, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x7f, 0x00, 0x00, 
      0x07, 0xe0, 0x00, 0x3f, 0xf0, 0x00, 0x1f, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x03, 
      0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00}
    },
    // BTC setup
    {"BTC",
     "https://api.cryptonator.com/api/ticker/btc-eur",
      {0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
      0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 
      0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 
      0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xfe, 0x3f, 0xff, 0xe0, 0x0f, 0xff, 0xfe, 0x73, 
      0xff, 0xf0, 0x0f, 0xff, 0xfc, 0x63, 0xff, 0xf0, 0x1f, 0xff, 0x0c, 0x63, 0xff, 0xf8, 0x1f, 0xff, 
      0x00, 0x63, 0xff, 0xfc, 0x3f, 0xff, 0x80, 0x07, 0xff, 0xfc, 0x3f, 0xff, 0xe0, 0x01, 0xff, 0xfc, 
      0x7f, 0xff, 0xe0, 0x80, 0x7f, 0xfe, 0x7f, 0xff, 0xe0, 0xf0, 0x3f, 0xfe, 0x7f, 0xff, 0xe1, 0xf8, 
      0x3f, 0xfe, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0xff, 0xff, 0xff, 
      0xc1, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 
      0xff, 0xff, 0x82, 0x00, 0xff, 0xff, 0xff, 0xff, 0x83, 0xc0, 0x7f, 0xff, 0xff, 0xff, 0x87, 0xf0, 
      0x3f, 0xff, 0xff, 0xff, 0x07, 0xf8, 0x3f, 0xff, 0xff, 0xfb, 0x07, 0xf8, 0x3f, 0xff, 0x7f, 0xf8, 
      0x07, 0xf0, 0x3f, 0xfe, 0x7f, 0xf0, 0x03, 0xf0, 0x3f, 0xfe, 0x7f, 0xf8, 0x00, 0x00, 0x7f, 0xfe, 
      0x3f, 0xff, 0x80, 0x00, 0x7f, 0xfc, 0x3f, 0xff, 0x88, 0x00, 0xff, 0xfc, 0x3f, 0xff, 0x9c, 0x43, 
      0xff, 0xfc, 0x1f, 0xff, 0x18, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0x18, 0xff, 0xff, 0xf8, 0x0f, 0xff, 
      0xf8, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xfd, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
      0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0xff, 
      0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 
      0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00}
    },
    // ETH setup
    {"ETH",
     "https://api.cryptonator.com/api/ticker/eth-eur",
      {0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
      0xc0, 0x00, 0x00, 0x0f, 0xfe, 0xff, 0xf0, 0x00, 0x00, 0x1f, 0xfc, 0x7f, 0xfc, 0x00, 0x00, 0x7f, 
      0xfc, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x01, 0xff, 0xf0, 0x1f, 0xff, 0x80, 
      0x03, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 0x07, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0xe0, 0x07, 
      0xff, 0xf0, 0x0f, 0xff, 0xc0, 0x07, 0xff, 0xf0, 0x1f, 0xff, 0x80, 0x03, 0xff, 0xf8, 0x1f, 0xff, 
      0x80, 0x03, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x01, 0xff, 0xfc, 0x3f, 0xff, 0x00, 0x00, 0xff, 0xfc, 
      0x7f, 0xfe, 0x00, 0x00, 0xff, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 
      0x7f, 0xfe, 0x7f, 0xf8, 0x00, 0x00, 0x3f, 0xfe, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0, 
      0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 
      0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x00, 
      0x7f, 0xff, 0x7f, 0xe7, 0x00, 0x01, 0xcf, 0xff, 0x7f, 0xf3, 0xc0, 0x07, 0x9f, 0xff, 0x7f, 0xf8, 
      0xf0, 0x0e, 0x3f, 0xfe, 0x7f, 0xf8, 0x38, 0x3c, 0x3f, 0xfe, 0x7f, 0xfc, 0x1e, 0xf0, 0x7f, 0xfe, 
      0x3f, 0xfe, 0x07, 0xc0, 0xff, 0xfe, 0x3f, 0xff, 0x03, 0x80, 0xff, 0xfc, 0x1f, 0xff, 0x00, 0x01, 
      0xff, 0xfc, 0x1f, 0xff, 0x80, 0x03, 0xff, 0xf8, 0x0f, 0xff, 0xc0, 0x07, 0xff, 0xf0, 0x0f, 0xff, 
      0xc0, 0x07, 0xff, 0xf0, 0x07, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x03, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 
      0x01, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 0x00, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xfc, 0x7f, 
      0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x07, 
      0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00}
    },
    // XLM setup
    {"Stellar",
     "https://api.cryptonator.com/api/ticker/xlm-eur",
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
      0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0x00, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 
      0x00, 0x1f, 0xc0, 0x03, 0xc0, 0x3f, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x7c, 0x00, 0x00, 
      0x03, 0xfc, 0x00, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x01, 0xf0, 0x00, 0x00, 0x3f, 0xe0, 0x01, 0xe0, 
      0x00, 0x00, 0xff, 0x80, 0x03, 0xc0, 0x00, 0x03, 0xfe, 0x01, 0x03, 0xc0, 0x00, 0x07, 0xf8, 0x07, 
      0x07, 0x80, 0x00, 0x3f, 0xe0, 0x1f, 0x07, 0x80, 0x00, 0x7f, 0x80, 0x7f, 0x0f, 0x00, 0x01, 0xfe, 
      0x01, 0xfe, 0x0f, 0x00, 0x07, 0xf8, 0x07, 0xf8, 0x0f, 0x00, 0x1f, 0xe0, 0x1f, 0xf8, 0x0f, 0x00, 
      0x7f, 0x80, 0x7f, 0xf0, 0x0e, 0x01, 0xfe, 0x01, 0xfe, 0x70, 0x0e, 0x07, 0xf8, 0x07, 0xf8, 0x70, 
      0x0e, 0x1f, 0xe0, 0x1f, 0xe0, 0x70, 0x0e, 0x7f, 0x80, 0x7f, 0x80, 0x70, 0x0f, 0xfe, 0x01, 0xfe, 
      0x00, 0xf0, 0x1f, 0xf8, 0x07, 0xf8, 0x00, 0xf0, 0x1f, 0xe0, 0x1f, 0xe0, 0x00, 0xf0, 0x7f, 0x80, 
      0x7f, 0x80, 0x00, 0xf0, 0xfe, 0x01, 0xfe, 0x00, 0x01, 0xe0, 0xf8, 0x07, 0xfc, 0x00, 0x01, 0xe0, 
      0xe0, 0x1f, 0xe0, 0x00, 0x03, 0xc0, 0x80, 0x7f, 0xc0, 0x00, 0x03, 0xc0, 0x01, 0xff, 0x00, 0x00, 
      0x07, 0x80, 0x07, 0xfc, 0x00, 0x00, 0x0f, 0x80, 0x0f, 0xf0, 0x00, 0x00, 0x1f, 0x00, 0x3f, 0xc0, 
      0x00, 0x00, 0x3e, 0x00, 0xff, 0x00, 0x00, 0x00, 0xfc, 0x00, 0xfc, 0x03, 0xc0, 0x03, 0xf8, 0x00, 
      0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x00, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xff, 
      0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    }
};

// Stonks and not stonks logos are both 70x30
#define STONKS_WIDTH 70
#define STONKS_HEIGHT 30

// Stonks symbol as bitmap
const unsigned char stonks [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 
  0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x07, 0x80, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0xff, 0xf8, 0x00, 
  0x00, 0x00, 0x3f, 0xc0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x07, 0xff, 
  0xf0, 0x00, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 
  0x3f, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0xff, 
  0xf8, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xe7, 0xc0, 0x00, 0x03, 
  0xff, 0xff, 0xfe, 0x1f, 0xff, 0xc7, 0xc0, 0x00, 0x07, 0xff, 0xf3, 0xfe, 0x7f, 0xff, 0x03, 0x80, 
  0x00, 0x1f, 0xff, 0xc1, 0xff, 0xff, 0xfe, 0x03, 0x80, 0x00, 0x7f, 0xff, 0x01, 0xff, 0xff, 0xf8, 
  0x01, 0x80, 0x00, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xf8, 0x00, 0xff, 
  0xff, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xc0, 
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0xff, 
  0xfc, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x3f, 0xe0, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x04, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Not stonks symbol as bitmap
const unsigned char notstonks [] PROGMEM = {
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x0c, 
  0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x00, 
  0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x7f, 
  0xff, 0x80, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x7f, 0xfe, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0xff, 0xff, 
  0xc0, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xf0, 0x01, 0x80, 0x00, 0x3f, 0xff, 0x81, 
  0xff, 0xff, 0xf8, 0x01, 0x80, 0x00, 0x1f, 0xff, 0xe3, 0xff, 0xff, 0xfe, 0x03, 0x80, 0x00, 0x07, 
  0xff, 0xf3, 0xfe, 0x3f, 0xff, 0x83, 0xc0, 0x00, 0x01, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xc7, 0xc0, 
  0x00, 0x00, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x3f, 0xff, 0xf8, 0x01, 0xff, 
  0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xf0, 
  0x00, 0x3f, 0xff, 0xe0, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 
  0x7f, 0xe0, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x01, 0xff, 0xf0, 0x00, 
  0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0xff, 
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00
};

// Varible to store what coin should be displayed
int currentCoin = 0;
// Const to store how many coins are configured
const int maxCoins = sizeof(cryptos)/sizeof(cryptos[0]);

void setup() {

  Serial.begin(115200);

  // Try to start display if it doesnt work halt program and inform user
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("HALT: OLED allocation failed"));
    while(1);
  }

  // Immediately clear display to remove Adafruit logo
  display.clearDisplay();
  // Set the text color to white this will turn on pixels when set
  display.setTextColor(SSD1306_WHITE);
  // Set starting text size to 1 for connecting message
  display.setTextSize(1);

  // Allow esp wifi time to setup before attempting connection
  for (uint8_t t = SETUP_TIME; t > 0; t--) {
    // Print info to serial
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    // and print info to OLED
    display.clearDisplay();
    display.setCursor(0, display.height()/4);
    display.print(F("Setup please wait . . . "));
    display.print(t);
    display.display();
    delay(1000);
  }

  // Use font size 2 for the rest of the program
  display.setTextSize(2);

  // Add Wifi network to ESP
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
}

void loop() {
  // Wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // Setup a https client
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    HTTPClient https;

    // Connect to API
    Serial.print("HTTPS begin...\n");
    if (https.begin(*client, cryptos[currentCoin].url)) {  // HTTPS

      // start connection and send HTTP header
      Serial.print("HTTPS GET...\n");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("HTTPS GET... code: %d\n", httpCode);

        // If the HTTP code is valid process and display the data
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          // Store the API payload in a string
          String payload = https.getString();
          // Calculate the price, change and percentage change
          float price = payload.substring(payload.indexOf("price")+8,payload.indexOf("volume")-3).toFloat();
          float change = payload.substring(payload.indexOf("change")+9,payload.indexOf("timestamp")-4).toFloat();
          float changePercent = change/price*100;
          // Print payload to serial
          Serial.println(cryptos[currentCoin].coin);
          Serial.println(payload);
          // Print info to display
          display.clearDisplay();
          drawLogo(cryptos[currentCoin].logo);
          drawStonks(change);
          printPrice(price);
          printChange(changePercent);

          // Move to the next coin
          currentCoin++;
          if(currentCoin>=maxCoins){
            currentCoin = 0;
          }
        }
      } else {
        // Warn user using serial and OLED that call failed
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, display.height()/4);
        display.println(F("HTTPS GET failed"));
        display.print(F("Will retry in a few seconds"));
        display.setTextSize(2);
      }

      // Must close the https connection
      https.end();
    }
    else {
      // Warn user using serial and OLED that call failed
      Serial.println("Unable to connect to API");
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, display.height()/4);
      display.println(F("Unable to connect"));
      display.print(F("Will retry in a few seconds"));
      display.setTextSize(2);
    }
  }

  Serial.print("Wait a few seconds before next round...\n\n");
  delay(UPDATE_TIME*1000);
}

// Draw the crypto logo on the OLED
void drawLogo(const unsigned char logo []) {
  display.drawBitmap(0,
    (display.height() + (display.height()/4) - LOGO_HEIGHT) / 2,
    logo, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}

// Draw the stonks or not stonks logo on the OLED
void drawStonks(float change) {
  if(change>=0){
    display.drawBitmap(display.width()-STONKS_WIDTH, display.height()-STONKS_HEIGHT, stonks, STONKS_WIDTH, STONKS_HEIGHT, 1);
  }
  else{
    display.drawBitmap(display.width()-STONKS_WIDTH, display.height()-STONKS_HEIGHT, notstonks, STONKS_WIDTH, STONKS_HEIGHT, 1);
  }  
  display.display();
}

// Print the price on the OLED
void printPrice(float price){
    char priceBuf[20];
    // Construct the price string
    if(price<10){
      sprintf(priceBuf, "E%.5f", price);
    }
    else {
      sprintf(priceBuf, "E%.3f", price);
    }
    // To center the price we need to get the size it will be on the OLED
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(priceBuf, 0, 0, &x1, &y1, &w, &h);
    // Set the cursor to the starting location that will center the text
    display.setCursor((display.width() - w) / 2, 0);
    // Print the string to the OLED
    display.print(priceBuf);
    display.display();
}

// Print the price change percentage to the OLED
void printChange(float change) {
  display.setCursor(display.width()-STONKS_WIDTH, display.height()/4);
  // Manually print a + for postive changes
  if(change>=0){
    display.print(F("+"));
  }
  // If the change is less than 10 use a decimal point if not dont
  if(abs(change)<10){
    display.print(change,1);
  }
  else{
    display.print(change,0);
  }
  display.print(F("%"));
  display.display();
}
