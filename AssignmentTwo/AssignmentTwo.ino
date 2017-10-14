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

const int analogInPin = A0; // for Potensiometer
const int buttonPin = 7;

int sensorValue = 0;
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int buttonToggleMode = 0;
const int mode_Idle = 0;
const int mode_SetAlarm_Hour = 1;
const int mode_SetAlarm_Minute = 2;

int alarmSetHour = 0;
int alarmSetMinute = 0;

bool alarming = false;
bool alarmToggle = false;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
RTC_DS1307 rtc;

DateTime now;
DateTime alarmTime;
DateTime previousTime;


void setup(void) {

  pinMode(buttonPin, INPUT);

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
  tft.setRotation(2); // Flips the output upside down. This is due to how I placed the hardware
  tft.fillScreen(ST7735_BLACK);

  alarmTime = DateTime(0, 0, 0, 10, 0, 0);
  Serial.println("Initialized");

  now = rtc.now();
  showDate();
  showTime();
  showAllIdle();
  delay(250);
}

void loop() {
  now = rtc.now();
  buttonState = digitalRead(buttonPin);
  sensorValue = analogRead(analogInPin);

  if (now.minute() != previousTime.minute()){
    previousTime = now;
    alarmToggle = false;
    refresh();
  }

  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      Serial.println("on");
      showButtonPressed();
      
      if (!alarming) nextMode();
      else turnOffAlarm();
      refresh();
    } else {
      Serial.println("off");

    }
    delay(50);
  }
  lastButtonState = buttonState;

  if (!alarming){
    switch (buttonToggleMode) {
      case mode_Idle:
        showAllIdle();
        break;
      case mode_SetAlarm_Hour:
        showAllSetAlarm(mode_SetAlarm_Hour);
        break;
      case mode_SetAlarm_Minute:
        showAllSetAlarm(mode_SetAlarm_Minute);
        break;
    }
  } else showAlarm();
  delay(50);
}

void turnOffAlarm(){
  alarming = false;
  hideTextLine(100);
}

void turnOnAlarm(){
  alarming = true;
  showAlarm();
}

void refresh(){
  tft.fillScreen(ST7735_BLACK);
  showDate();
  showTime();
  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute() && !alarmToggle && !alarming) {
    turnOnAlarm();
    alarmToggle = true;
  }
}

void nextMode() {
  buttonToggleMode++;
  if (buttonToggleMode >= 3) buttonToggleMode = 0;
  Serial.print("Mode: ");
  Serial.println(buttonToggleMode);
}

void showButtonPressed() {
  tft.setCursor(0, 130);
  tft.setTextColor(ST7735_RED);
  tft.print("Button is Pressed!");
}

void showAllSetAlarm(int mode) {
  
  if(mode == mode_SetAlarm_Hour){
    hideText(6 ,70, 6*2);
  } else if(mode == mode_SetAlarm_Minute){
    hideText(4*6 ,70, 6*2);
  }
  showSetAlarm();
}

void showAllIdle() {
  showAlarmTime();
}

void showAlarm() {
  showTextLine(100, "Alarm! ", ST7735_RED);
  showTextLine(110, "Press Button to turn off.", ST7735_WHITE);
}

void showDate() {
  showTextLine(0, "Date: ", ST7735_WHITE);
  String text = "";
  text += now.year();
  text += '/';
  text += now.month() ;
  text += '/';
  text += now.day();
  showTextLine(10, text, ST7735_GREEN);
}

void showTime() {
  showTextLine(30, "Time: ", ST7735_WHITE);
  String text = "";
  if (now.hour() <= 9) {
    text += "0";
  }
  text += now.hour();
  text += ':';
  if (now.minute() <= 9) {
    text += "0";
  }
  text += now.minute();
  showTextLine(40, text, ST7735_GREEN);
}

void showAlarmTime() {
  showTextLine(60, "Alarm is set to:", ST7735_WHITE);

  String text = "";
  if (alarmTime.hour() <= 9) {
    text += "0";
  }
  text += alarmTime.hour();
  text += ':';
  if (alarmTime.minute() <= 9) {
    text += "0";
  }
  text += alarmTime.minute();
  showTextLine(70, text, ST7735_GREEN);
}

void showSetAlarm() {
  showTextLine(60, "Set Alarm To:", ST7735_WHITE);

  tft.setCursor(0, 70);
  if (buttonToggleMode == mode_SetAlarm_Hour){  
    alarmSetHour = map(sensorValue, 0, 1023, 0, 30)%24; //A bit of leverage due to inaccurate potensiometer component
    tft.setTextColor(ST7735_YELLOW);
    tft.print("[");
  }
  tft.setTextColor(ST7735_GREEN);
  if (alarmSetHour <= 9) {
    tft.print("0");
  }
  tft.print(alarmSetHour);
  if (buttonToggleMode == mode_SetAlarm_Hour){  
    tft.setTextColor(ST7735_YELLOW);
    tft.print("]");    
  }
  tft.print(':');
  if (buttonToggleMode == mode_SetAlarm_Minute) {
    alarmSetMinute = map(sensorValue, 0, 1023, 0, 70)%60; //A bit of leverage due to inaccurate potensiometer component
    tft.setTextColor(ST7735_YELLOW);
    tft.print("[");
  }
  tft.setTextColor(ST7735_GREEN);
  if (alarmSetMinute <= 9) {
    tft.print("0");
  }
  tft.print(alarmSetMinute);
  if (buttonToggleMode == mode_SetAlarm_Minute) {
    tft.setTextColor(ST7735_YELLOW);
    tft.print("]");
  }
  alarmTime = DateTime(2017, 10, 14, alarmSetHour, alarmSetMinute, 0);
}

void showTextLine(int yValue, String text, uint16_t color) {
  tft.setCursor(0, yValue);
  tft.setTextColor(color);
  tft.println(text);
}

void hideText(int xStart, int yStart, int width){
  tft.fillRect(xStart, yStart, width, 10 /*default font size*/, ST7735_BLACK);
}

void hideTextLine(int yValue){
  tft.fillRect(0, yValue, tft.width(), 10 /*default font size*/, ST7735_BLACK);
}
