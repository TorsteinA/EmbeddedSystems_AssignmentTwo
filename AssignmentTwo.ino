#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     9

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

RTC_DS1307 rtc;

DateTime now;
DateTime alarmTime;


void setup(void) {
  while (!Serial); // for Leonardo/Micro/Zero

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");

  //uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  //time = millis() - time;

  //Serial.println(time, DEC);
  delay(500);
}

void loop() {
  now = rtc.now();
  alarmTime = DateTime(0, 0, 0, 01, 9, 00);
  tft.fillScreen(ST7735_BLACK);

  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute()) {
    turnOnAlarm();
  }
  
  showAllIdle();
  delay(500);
}

void turnOnAlarm(){
  showAlarm();
}

void showAllIdle(){
  showDate();
  showTime();
  showAlarmTime();
}

void showAlarm(){
  tft.setCursor(0, 100);
  tft.setTextColor(ST7735_RED);
  tft.print("Alarm!");
}

void showDate() {
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Date:");
  tft.setCursor(0, 10);
  tft.setTextColor(ST7735_GREEN);
  tft.print(now.year(), DEC);
  tft.print('/');
  tft.print(now.month(), DEC);
  tft.print('/');
  tft.print(now.day(), DEC);
}

void showTime() {
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Time:");
  tft.setCursor(0, 40);
  tft.setTextColor(ST7735_GREEN);
  if (now.hour() <= 9){ tft.print("0"); }
  tft.print(now.hour(), DEC);
  tft.print(':');
  if (now.minute() <= 9){ tft.print("0"); }
  tft.print(now.minute(), DEC);
  tft.print(':');
  if (now.second() <= 9){ tft.print("0"); }
  tft.print(now.second(), DEC);
}

void showAlarmTime() {
  tft.setCursor(0, 60);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Alarm goes off at:");
  tft.setCursor(0, 70);
  tft.setTextColor(ST7735_GREEN);
  if (alarmTime.hour() <= 9){ tft.print("0"); }
  tft.print(alarmTime.hour(), DEC);
  tft.print(':');
  if (alarmTime.minute() <= 9){ tft.print("0"); }
  tft.print(alarmTime.minute(), DEC);
}


