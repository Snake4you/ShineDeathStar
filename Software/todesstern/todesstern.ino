#include <Arduino.h>
#include <avr/pgmspace.h>
#include "FastLED.h"
#include <avr/sleep.h>
#include <avr/power.h>

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
bool laserDirection = true;


CRGB leds[numLeds];


void playtheme();
void shakeSensor();
void rgbRing();
void redRing();
void goToSleep();
void TinyTone(unsigned char divisor, unsigned char octave);

ISR (PCINT0_vect) {}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  pinMode(shakePin, INPUT_PULLUP);

  FastLED.addLeds<WS2812, ledRingPin, RGB>(leds, numLeds);
  FastLED.setBrightness(brigtLeds);

  //PCMSK |= bit (PCINT2);
  //GIFR  |= bit (PCIF); // clear any outstanding interrupts
  //GIMSK |= bit (PCIE); // enable pin change interrupts
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
        if (shakeTime < millis()) {
          shakeCounter++;
          shakeTime = millis() + 50;
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
  if (millis() > 3600000) { //DeepSleep after 1hour
    goToSleep();
  }
  
}

void goToSleep() {
  leds[0] = CRGB(0,0,0);
  leds[1] = CRGB(0,0,0);
  leds[2] = CRGB(0,0,0);
  leds[3] = CRGB(0,0,0);
  leds[4] = CRGB(0,0,0);
  leds[5] = CRGB(20,0,0);
  FastLED.show();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  power_all_enable();    // power everything back on
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
