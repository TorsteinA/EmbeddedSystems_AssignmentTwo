#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Wire.h> // Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "pitches.h"

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
const int TFT_CS   = 10;
const int TFT_RST  = 8;
const int TFT_DC   = 9;
const int TFT_SCLK = 13;
const int TFT_MOSI = 11;

const int analogInPin = A0;
const int buttonPin = 7;
const int buttonPin2 = 4;
const int tonePin = 2;

int sensorValue = 0;
int buttonState = 0;         // current state of the button
int buttonState2 = 0;         // current state of the button2
int lastButtonState = 0;     // previous state of the button
int lastButtonState2 = 0;     // previous state of the button2

int buttonToggleMode = 0;
const int mode_Idle = 0;
const int mode_SetAlarm_Hour = 1;
const int mode_SetAlarm_Minute = 2;
const int numberOfButtonToggleModes = 3; //Separate int/size because modes are defined using const ints

int alarmSetHour;
int alarmSetMinute;

bool alarming = false;
bool snoozing = false;
bool alarmToggle = false;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
RTC_DS1307 rtc;

DateTime now;
DateTime alarmTime;
DateTime previousTime;

int melodyAlarm[] = {
  //NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A4, NOTE_B4, NOTE_C4, NOTE_B4, NOTE_A4, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_D3
  NOTE_E3,
  NOTE_GS3,
  NOTE_B3,
  NOTE_DS3,
  NOTE_E4,
  NOTE_GS4,
  NOTE_B4,
  NOTE_DS4,
  NOTE_E5,
  NOTE_DS4,
  NOTE_B4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_B3,
  NOTE_GS3/*,
  NOTE_E3*/
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsAlarm[] = {
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, //8, 4, 1
};

int noteDurationsRandomAlarm[] = {
  16, 16, 16, 16, 8, 8, 4, 2
};

int melodyStartup[] = {
  NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B5, NOTE_C5
};

int noteDurationsStartup[] = {
  1, 2, 4, 8, 16, 32, 32 
};

void setup(void) {

  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  randomSeed(analogRead(0));

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

  alarmTime = DateTime(0, 0, 0, 10, 0, 0);  //Sets the default alarm time to 10:00 AM. 
  alarmSetHour = alarmTime.hour();
  alarmSetMinute = alarmTime.minute();

  //PlayStartupAudio();
  PlayRandomStartupAudio();

  Serial.println("Initialized");
}

void loop() {
  now = rtc.now();
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
  sensorValue = analogRead(analogInPin);

  if (now.minute() != previousTime.minute()) { updateMinute(); }

  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      pressedButton(0);
    }
    delay(100);
    lastButtonState = buttonState;
  }

  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == HIGH) {
      pressedButton(1);
    }
    delay(100);
    lastButtonState2 = buttonState2;
  }

  if (alarming) {
    if (snoozing) turnOffSnooze();
    showAlarm();
    //PlayAlarmAudio();
    PlayRandomAlarmAudio();
  } else { showCurrentMode(); }
  delay(50);
}

void pressedButton(int button){

  if (alarming) {
    if (button == 0) turnOffAlarm();
    if (button == 1) snooze();
  }
  else {
    if (snoozing) turnOffSnooze();
    if (button == 0) nextMode();
    if (button == 1) previousMode();
  }
  if ((buttonToggleMode != mode_SetAlarm_Minute && button == 0) || 
    (buttonToggleMode != mode_SetAlarm_Hour && button == 1)) hideTextLine(60);
  hideTextLine(70);
}

void updateMinute(){
  previousTime = now;
  alarmToggle = false;
  refresh();
}

void refresh() {
  tft.fillScreen(ST7735_BLACK);
  showDate();
  showTime();
  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute() && !alarmToggle && !alarming) {
    turnOnAlarm();
    alarmToggle = true;
    if (snoozing) turnOffSnooze();
  }
}

void nextMode() {
  buttonToggleMode++;
  if (buttonToggleMode >= numberOfButtonToggleModes) buttonToggleMode = 0;
  Serial.print("Mode: ");
  Serial.println(buttonToggleMode);
}

void previousMode() {
  buttonToggleMode--;
  if (buttonToggleMode < 0) buttonToggleMode = 2;
  Serial.print("Mode: ");
  Serial.println(buttonToggleMode);
}

// --- Alarm Methods --- //

void snooze() {
  snoozing = true;
  alarmTime = (alarmTime + TimeSpan(0, 0, 5, 0));
  turnOffAlarm();
  showSnooze();
}

void turnOffSnooze() {
  snoozing = false;
  hideTextLine(100);
}

void turnOffAlarm() {
  alarming = false;
  hideTextLine(100);
  hideTextLine(110);
  hideTextLine(120);
  hideTextLine(130);
}

void turnOnAlarm() {
  alarming = true;
  showAlarm();
}

// --- Display Methods --- //

void showCurrentMode(){
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
}

void showAllSetAlarm(int mode) {

  if (mode == mode_SetAlarm_Hour) {
    hideText(6 , 70, 6 * 2);
  } else if (mode == mode_SetAlarm_Minute) {
    hideText(4 * 6 , 70, 6 * 2); //In default font size, one character is 6 pixels wide
  }
  showSetAlarm();
}

void showAllIdle() {
  showAlarmTime();
}

void showSnooze() {
  showTextLine(100, "Snoozing... ", ST7735_WHITE);
  Serial.println("Snoozing...");
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

void showAlarm() {
  showTextLine(100, "      Alarm! ", ST7735_RED);
  showTextLine(120, "Snooze       Turn off", ST7735_WHITE);
  Serial.println("Alarm!");
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
  if (buttonToggleMode == mode_SetAlarm_Hour) {
    alarmSetHour = map(sensorValue, 0, 1023, 0, 30) % 24; //A bit of leverage due to inaccurate potensiometer component
    tft.setTextColor(ST7735_YELLOW);
    tft.print("[");
  }
  tft.setTextColor(ST7735_GREEN);
  if (alarmSetHour <= 9) {
    tft.print("0");
  }
  tft.print(alarmSetHour);
  if (buttonToggleMode == mode_SetAlarm_Hour) {
    tft.setTextColor(ST7735_YELLOW);
    tft.print("]");
  }
  tft.print(':');
  if (buttonToggleMode == mode_SetAlarm_Minute) {
    alarmSetMinute = map(sensorValue, 0, 1023, 0, 70) % 60; //A bit of leverage due to inaccurate potensiometer component
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

void hideText(int xStart, int yStart, int width) {
  tft.fillRect(xStart, yStart, width, 10 /*default font size*/, ST7735_BLACK);
}

void hideTextLine(int yValue) {
  tft.fillRect(0, yValue, tft.width(), 10 /*default font size*/, ST7735_BLACK);
}

// --- Audio Methods --- //

void PlayAlarmAudio() {
  for (int i = 0; i <= (sizeof(melodyAlarm) / sizeof(melodyAlarm[0])) - 1; i++) {
    int noteDuration = 1000 / noteDurationsAlarm[i % (sizeof(noteDurationsAlarm) / sizeof(noteDurationsAlarm[0]))];
    tone(tonePin, melodyAlarm[i % (sizeof(melodyAlarm) / sizeof(melodyAlarm[0]))], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(tonePin);
  }
}

void PlayStartupAudio() {
  for (int i = 0; i <= (sizeof(melodyStartup) / sizeof(melodyStartup[0])) - 1; i++) {
    int noteDuration = 1000 / noteDurationsStartup[i % (sizeof(noteDurationsStartup) / sizeof(noteDurationsStartup[0]))];
    tone(tonePin, melodyStartup[i % (sizeof(melodyStartup) / sizeof(melodyStartup[0]))], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(tonePin);
  }
}

void PlayRandomAlarmAudio(){
  double melodyLength = (sizeof(melodyStartup) / sizeof(melodyStartup[0]));
  for (int i = 0; i <= (sizeof(melodyAlarm) / sizeof(melodyAlarm[0])) - 1; i++) {
    int noteDuration = 1000 / noteDurationsRandomAlarm[random(sizeof(noteDurationsRandomAlarm) / sizeof(noteDurationsRandomAlarm[0]))];
    tone(tonePin, melodyAlarm[random(melodyLength)], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(tonePin);
  }
}

void PlayRandomStartupAudio(){
  double melodyLength = (sizeof(melodyStartup) / sizeof(melodyStartup[0]));
  for (int i = 0; i <= melodyLength - 1; i++) {
    int noteDuration = 1000 / noteDurationsStartup[i % (sizeof(noteDurationsStartup) / sizeof(noteDurationsStartup[0]))];
    tone(tonePin, melodyStartup[random(melodyLength)], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(tonePin);
  }
}
