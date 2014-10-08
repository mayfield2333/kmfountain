#include "Arduino.h"
#define debug true
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"
#include "timer.h"

#if defined(__AVR_ATmega2560__)
#define REMOTE_PIN 22
#else
#define REMOTE_PIN 13
#endif

#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

#define NUM_PUMPS 6
#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

int patternChangeSeconds = 120;
int colorChangeSeconds = 10;
int internalPatternChangeSeconds = 6;
int pumpfade = 4;
int lightfade = 5;

int lightSensorPin = A0;    // select the input pin for the potentiometer
int lightLedPin = 13;      // select the pin for the LED
int lightSensorValue = 0;  // variable to store the value coming from the sensor

Remote remote;
RGBLed rgb;
Fountain fountain;
int mode = 0;
int pattern = 0;
int pattern_one_state = 0;

boolean fountainOn = false;
boolean lightsOn = false;

Timer change;
Timer patternChange;
Timer internalPatternChange;

void setup() {
  // USB can be used for Debug output.
  Serial.begin(9600);
  Dln("setup");
  D2("Remote pin = ", REMOTE_PIN);
  remote.init(REMOTE_PIN);
  
  pinMode(lightLedPin, OUTPUT);
  
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
  rgb.setFadeSpeed(lightfade);  // In seconds.
  //rgb.test(&remote);
  
  fountain.init(FIRST_PUMP_PIN);
  fountain.setFadeSpeed(pumpfade);  // in Seconds.
  //fountain.test(&remote);
  
  change.setSeconds(colorChangeSeconds);
  change.expire();
  patternChange.setSeconds(patternChangeSeconds);
  
  internalPatternChange.setSeconds(internalPatternChangeSeconds);
  randomSeed(analogRead(0));
  
  Dln("end setup");
}


boolean test = false;
int speed = 1000;

void loop()
{
  
  boolean pressA = remote.getButton(0)->hasChanged();
#if defined(__AVR_ATmega2560__)
  boolean pressB = remote.getButton(1)->hasChanged();
  boolean pressC = remote.getButton(2)->hasChanged();
  boolean pressD = remote.getButton(3)->hasChanged();
  boolean pressAny = pressA || pressB || pressC || pressD;
  
  if (pressAny)
  {  
    // Turn on with any button;
    if (!fountainOn)
    {
      internalPatternChange.expire();
      patternChange.expire();
      mode = 0;
    }
      
    fountainOn = true;

    // BUTTON D: Lights on
    if (pressD)
    {
       Dln("Button D -Lights");
       lightsOn = !lightsOn;
    }
    
    // BUTTON C : mode
    if (pressC)
    {
      Dln("Button 2 - mode");
      if (++mode > 2)
         mode = 0;
         
      pattern = mode;
    }
  }

    // BUTTON B : Toggle Fountain ON/OFF?
    if (pressB)
    {
      Dln("Button B");
      fountainOn = false;
    }


#endif

   if (!fountainOn)
   {
      rgb.off();
      fountain.off();
      return;
   }
   
   int sensorValue = analogRead(lightSensorPin);
   if (lightsOn || sensorValue < 300)
     lightsOn = true;
 
   if (pressA) 
  {
    fountain.off();
    if (remote.pause(500))
      return;
    fountain.pump[2].setNow(255);
    if (remote.pause(500))
      return;
    fountain.off();
    Dln("Press A, pattern change");
    D2("Old Pattern = ", pattern);
    if (mode == 0)
    {
      mode = pattern = 1;
      D2("mode and pattern = ", mode);
    }
    else if ((mode = ++pattern) > 3)
    {
      mode = pattern = 0;
    }

    D2("New pattern = ",pattern);
    D2("current mode = ", mode);
    internalPatternChange.expire();
    pattern_one_state = 10;
  }
 
  if (pattern == 0 || (mode == 0 && patternChange.isNow()))
  {
    D("Mode 0 and patternChange is now()");
    pattern = random(1,4);
    D2("Random pattern = ",pattern);
    change.expire();
  }
  
   
  if (change.isNow())
  {
    D2(pause(1000), "Change Color");
    rgb.randomColor();
  }
  
  if (internalPatternChange.isNow())
  {
    if (pattern == 1) // random height
    {
      Dln("Pattern 1");
      if (++pattern_one_state >= 10)
      {
        fountain.off();
        pattern_one_state = 0;
      }
      
      D2("Pattern state = ", pattern_one_state);
      switch (pattern_one_state)
      {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
            fountain.pump[pattern_one_state].setNewValue(255);
            fountain.pump[5].setNewValue(0);
            break;

          case 5:
          case 6:
          case 7:
          case 8:
            fountain.pump[pattern_one_state - 5].setNow(0);
            break;
          case 9:
            fountain.pump[pattern_one_state - 5].setNewValue(0);
            fountain.pump[5].setNewValue(255);
            break;
      }
    }
    
    else if (pattern == 2) // random on
    {
      Dln("Pattern 2");
      int pumpnum = random(0,6);
      int maxvalue = pumpnum == 5 ? 128 : 255;
      
      fountain.off();
      fountain.pump[pumpnum].setNow(maxvalue);
    }
    else if (pattern == 3)
    {  
      Dln("Pattern 3");
      int pumpval;
      for (int p = 0; p < NUM_PUMPS - 1; ++p)
      {
        pumpval = random(0,256);
        D("Pump ");
        D3(p," = ", pumpval);
        fountain.pump[p].setNewValue(pumpval);
      }

      pumpval = random(0,4) == 0 ? 128: 0;
      D2("Pump 5 = ", pumpval);
      fountain.pump[5].setNewValue(pumpval);
    }
  } 
    
  if (lightsOn)
    rgb.update();
  else
    rgb.off();
  fountain.update();
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
