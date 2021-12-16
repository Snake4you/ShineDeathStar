#include <Arduino.h>
#include <avr/pgmspace.h>
#include "FastLED.h"

#include "song.h"


#define ledRingPin 0
#define buzzerPin 1
#define shakePin 2

#define numLeds 6
#define dataLeds 3
#define brigtLeds 200


unsigned long themeMainTimer = 0;
unsigned int themeSongCounter = 0;
unsigned int themeSongActive = 0;
bool themeMainActive = false;

unsigned int shakeCounter = 0;
unsigned long shakeTime = 0;
bool shakeState = false;

unsigned long rainbowRingTimer = 0;
unsigned int rainbowRingCounter = 0;

unsigned long laserTimer = 0;
int laserCounter = 0;
bool laserDirection = true; // Zählerrichtung true => aufsteigend


CRGB leds[numLeds];

void playtheme();
void shakeSensor();
void rgbRing();
void redRing();
void TinyTone(unsigned char divisor, unsigned char octave);

void setup() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  pinMode(shakePin, INPUT_PULLUP);

  FastLED.addLeds<WS2812, ledRingPin, RGB>(leds, numLeds);
  FastLED.setBrightness(brigtLeds);
  //shakeSensor();
}


void loop() {
  if (themeMainActive) {
    playtheme();
  }
  //shakeTime
  //shakeState
  else {
    if (digitalRead(shakePin) == HIGH) {
      if (shakeState == true) {
        if (shakeTime <= millis()) {
          shakeCounter++;
          shakeTime = millis() + 100;
        }
        else {
          shakeCounter = 0;
        }
        shakeState = false;
        if (shakeCounter >= 2) {
          shakeSensor();
          shakeCounter = 0;
        }
      }
    }
    if (digitalRead(shakePin) == LOW) {
      if (shakeState == false) {
        shakeState = true;
      }
    }
    redRing();
  }
}

void playtheme() {
  int tmpTheme[3];
  if (millis() >= themeMainTimer) {
    if (themeSongActive == 0) {
      tmpTheme[0] = pgm_read_word(&(theme[themeSongCounter][0]));
      tmpTheme[1] = pgm_read_word(&(theme[themeSongCounter][1]));
      tmpTheme[2] = pgm_read_word(&(theme[themeSongCounter][2]));
      if (tmpTheme[0] > 0) {
        TinyTone(tmpTheme[0],tmpTheme[1]);
      }
      else {
        OffTone();
      }
      themeMainTimer = millis() + tmpTheme[2];
      themeSongCounter++;
      themeSongActive = 1;
      rgbRing();
    }
    else {
      OffTone();
      themeMainTimer = millis() + 50;
      themeSongActive = 0;
      if (themeSongCounter >= 73) {
        themeMainActive = false;
      }
    }
  }
}

void TinyTone(unsigned char divisor, unsigned char octave) {
  TCCR1 = 0x90 | (11-octave); // for 8MHz clock
  OCR1C = divisor-1;         // set the OCR
}
void OffTone() {
  TCNT1 = 0;
  TCCR1 = 0x80;              // stop the counter
  digitalWrite(buzzerPin, LOW);
}

void shakeSensor() {
  if (!themeMainActive) {
    themeSongCounter = 0;
    themeMainActive = true;
  }
}

void rgbRing() {
  unsigned int value = 0;
  switch (rainbowRingCounter) {
  case 0:
      value = 100; 
      leds[0] = CRGB(0,value,0);
      leds[1] = CRGB(0,value,0);
      leds[2] = CRGB(0,value,0);
      leds[3] = CRGB(0,value,0);
      leds[4] = CRGB(0,value,0);
      leds[5] = CRGB(value,0,0);
      rainbowRingCounter++;
    break;
  case 1:
      value = 100; 
      leds[0] = CRGB(0,0,value);
      leds[1] = CRGB(0,0,value);
      leds[2] = CRGB(0,0,value);
      leds[3] = CRGB(0,0,value);
      leds[4] = CRGB(0,0,value);
      leds[5] = CRGB(0,0,value);
      rainbowRingCounter++;
    break;
  case 2:
      value = 100; 
      leds[0] = CRGB(value,0,0);
      leds[1] = CRGB(value,0,0);
      leds[2] = CRGB(value,0,0);
      leds[3] = CRGB(value,0,0);
      leds[4] = CRGB(value,0,0);
      leds[5] = CRGB(0,value,0);
      rainbowRingCounter = 0;
    break;
  default:
    rainbowRingCounter = 0;
    break;
  }
    FastLED.show();
}


void redRing() {
  unsigned int value = 0;
  if (millis() >= rainbowRingTimer) {
    rainbowRingCounter++;
    switch (rainbowRingCounter) {
    case 1:
      value = 0;
      leds[0] = CRGB(value,0,0);
      value = 0;
      leds[4] = CRGB(value,0,0);
      value = 130;
      leds[3] = CRGB(value,0,0);
      value = 0;
      leds[2] = CRGB(value,0,0);
      value = 0;
      leds[1] = CRGB(value,0,0);
      break;
    case 2:
      value = 0;
      leds[1] = CRGB(value,0,0);
      value = 0;
      leds[0] = CRGB(value,0,0);
      value = 130;
      leds[4] = CRGB(value,0,0);
      value = 0;
      leds[3] = CRGB(value,0,0);
      value = 0;
      leds[2] = CRGB(value,0,0);
      break;
    case 3:
      value = 0;
      leds[2] = CRGB(value,0,0);
      value = 0;
      leds[1] = CRGB(value,0,0);
      value = 130;
      leds[0] = CRGB(value,0,0);
      value = 0;
      leds[4] = CRGB(value,0,0);
      value = 0;
      leds[3] = CRGB(value,0,0);
      break;
    case 4:
      value = 0;
      leds[3] = CRGB(value,0,0);
      value = 0;
      leds[2] = CRGB(value,0,0);
      value = 130;
      leds[1] = CRGB(value,0,0);
      value = 0;
      leds[0] = CRGB(value,0,0);
      value = 0;
      leds[4] = CRGB(value,0,0);
      break;
    case 5:
      value = 0;
      leds[4] = CRGB(value,0,0);
      value = 0;
      leds[3] = CRGB(value,0,0);
      value = 130;
      leds[2] = CRGB(value,0,0);
      value = 0;
      leds[1] = CRGB(value,0,0);
      value = 0;
      leds[0] = CRGB(value,0,0);
      rainbowRingCounter = 0;
      break;
    default:
      rainbowRingCounter = 0;
      break;
    }
    
    if (laserDirection == true) {
      if (laserCounter >= 150) {
          laserDirection = false;
      }
      else {
        laserCounter = laserCounter + 10 ;
      }
    }
    else {
      if (laserCounter <= 10) {
          laserDirection = true;
      }
      else {
        laserCounter = laserCounter - 10;
      }
    }
    value = laserCounter; 
    leds[5] = CRGB(0,value,0);
    FastLED.show();
    rainbowRingTimer = millis() + 250;
  }
}
