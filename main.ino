#include "src/LedMatrix/LedMatrix.h"

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>

RTC_PCF8523 rtc;
Sd2Card card;
SdVolume volume;
SdFile root;
File trackfile;
const int chipSelect = 10;

CRGB leds[LED_NUM];
char daysOfTheWeek[7][12] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};

LedMatrix ledMatrix;

void setup() {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  // Initializing RTC for time
  Serial.println("Initializing RTC...");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (!rtc.initialized() || rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.start();

  // Initializing SD card
  /* Serial.println("Initializing SD card...");
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("Couldn't initialize SD card (missing or mis-configured");
    Serial.flush();
    abort();
  }

  if (!volume.init(card)) {
    Serial.println("Couldn't find FAT16/FAT32 partition");
    Serial.flush();
    abort();
  }

  root.openRoot(volume);
  */

  // Initialize Debug Track File
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    Serial.flush();
    abort();
  }
  Serial.println("card initialized.");
  char filename[] = "test/test.json";
  if (!SD.exists(filename)) {
    trackfile = SD.open(filename, FILE_WRITE);
  }
  if (!trackfile) {
    Serial.println("Couldn't created file");
    Serial.flush();
    abort();
  }

  // Initializing LED Matrix
  Serial.println("Initializing LED Matrix");
  ledMatrix.init();
  ledMatrix.applyToCRGBArray(leds);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.show();
}

void loop() {}

void DebugRTC() {
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void DebugLEDStates() {
  delay(600);
  for (int x = 0; x < LED_ARRAY_X; x++) {
    for (int y = 0; y < LED_ARRAY_Y; y++) {
      ledMatrix.setState(x, y, LED_DIMMED);
    }
  }
  ledMatrix.applyToCRGBArray(leds);
  Serial.println("DIMMED");
  FastLED.show();

  delay(600);
  for (int x = 0; x < LED_ARRAY_X; x++) {
    for (int y = 0; y < LED_ARRAY_Y; y++) {
      ledMatrix.setState(x, y, LED_ACTIVE);
    }
  }
  ledMatrix.applyToCRGBArray(leds);
  Serial.println("ACTIVE");
  FastLED.show();

  delay(600);
  for (int x = 0; x < LED_ARRAY_X; x++) {
    for (int y = 0; y < LED_ARRAY_Y; y++) {
      ledMatrix.setState(x, y, LED_PRESSED);
    }
  }
  Serial.println("PRESSED");
  ledMatrix.applyToCRGBArray(leds);
  FastLED.show();

  delay(600);
  for (int x = 0; x < LED_ARRAY_X; x++) {
    for (int y = 0; y < LED_ARRAY_Y; y++) {
      ledMatrix.setState(x, y, LED_INACTIVE);
    }
  }
  ledMatrix.applyToCRGBArray(leds);
  Serial.println("INACTIVE");
  FastLED.show();

  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);

  ledMatrix.setInputsColor(r, g, b);
}