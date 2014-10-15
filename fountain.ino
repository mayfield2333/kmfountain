#include "Arduino.h"
#define debug false
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"
#include "timer.h"

/*
  Pin Assignments
*/ 
#if defined(__AVR_ATmega2560__)
#define REMOTE_PIN 22
#else
#define REMOTE_PIN 13
#endif

#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

static const int lightSensorPin = A0;    // select the input pin for the potentiometer
static const int lightLedPin = 13;      // select the pin for the LED

/*
   Constants
*/

static const int lightSensorValue = 0;  // variable to store the value coming from the sensor

#define NUM_PUMPS 6
#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

#if defined(__AVR_ATmega2560__)
static const int patternChangeSeconds = 120;
static const int colorChangeSeconds = 60;
static const int internalPatternChangeSeconds = 6;
static const int pumpfade = 4;
static const int lightfade = 5;
static const long autoOffMinutes = 90;
#else
static const int patternChangeSeconds = 60;
static const int colorChangeSeconds = 60;
static const int internalPatternChangeSeconds = 6;
static const int pumpfade = 4;
static const int lightfade = 5;
static const long autoOffMinutes = 5;
#endif
int mode = 0;
int colormode = 0;
int pattern = 0;
int pattern_one_state = 0;

boolean fountainOn = false;
boolean lightsOn = false;

/*
  Fountain sensors and actuators
*/

Remote remote;
RGBLed rgb;
Fountain fountain;

/*
   Timers
*/

Timer colorChange;    // Color change
Timer patternChange;
Timer internalPatternChange;
Timer autoOff;

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
  
  colorChange.setSeconds(colorChangeSeconds);
  colorChange.expire();
  patternChange.setSeconds(patternChangeSeconds);
  
  internalPatternChange.setSeconds(internalPatternChangeSeconds);
  randomSeed(analogRead(A0));
  
  autoOff.setMinutes(autoOffMinutes);
  Dln("end setup");
}


boolean test = false;
int speed = 1000;

void loop()
{
  boolean pressAny = false;
  boolean pressA = remote.getButton(0)->hasChanged();
 
#if defined(__AVR_ATmega2560__)
  boolean pressB = remote.getButton(1)->hasChanged();
  boolean pressC = remote.getButton(2)->hasChanged();
  boolean pressD = remote.getButton(3)->hasChanged();
  pressAny = pressA /*|| pressB*/ || pressC || pressD;  // PressB is turn off.
#else 
  pressAny = pressA;
#endif
   
#if defined(__AVR_ATmega2560__)

    // BUTTON D: Lights on
    if (pressD)
    {
       Dln("Button D -Lights");
       lightsOn = !lightsOn;
    }
    
    // BUTTON C : Color mode/set
    if (pressC)
    {
      Dln("press c");
      if (++colormode > 2)
      {
        Dln("Random color mode");
        colormode = 0;  // random
        colorChange.setSeconds(colorChangeSeconds);
        rgb.setFadeSpeed(lightfade);
      }
       
      Dln("Button C - COLOR mode");
      if (colormode == 1)
      {
        // Rapid change
        Dln("Rapid color change mode");
        colorChange.setSeconds(4);
        rgb.setFadeSpeed(1);
      }
      
      if(colormode == 2)
      {
        Dln("Hold color");
        colorChange.off();
      }
    }

    // BUTTON B : Toggle Fountain ON/OFF?
    if (pressB)
    {
      Dln("Button B");
      fountainOn = false;
    }

#endif

   if (autoOff.isNow())
   {
     D2("AutoOff Minutes expired: ", autoOff.getMinutes());
     fountainOn = false;
   }
     
   int sensorValue = analogRead(lightSensorPin);
   if (lightsOn || sensorValue < 300)
     lightsOn = true;
 
#define NUM_PATTERNS 5
  if (pressA && fountainOn) 
  {
    Dln("Press A"); 
    fountain.off();
    if (remote.pause(500))
      return;
    fountain.pump[5].setNow(255);
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
    else if ((mode = ++pattern) > NUM_PATTERNS)
    {
      mode = pattern = 0;
    }

    D2("New pattern = ",pattern);
    D2("current mode = ", mode);
    fountain.off();
    fountain.pump[pattern].setNow(255);
    if (remote.pause(500))
       return;
    fountain.off();

    pattern_one_state = 10;
  }
  
  if (pressAny)
  { 
    // Turn on with any button;
    if (!fountainOn)
    {
      Dln("Fountain turning on");
      internalPatternChange.expire();
      patternChange.expire();
      mode = 0;  // Random patterns
      colormode = 0; // Normal Random colors.
      autoOff.reset();
      internalPatternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.expire();
    }
      
    fountainOn = true;
  }
 
  if (!fountainOn)
  {
     Dln("Fountain off");
     rgb.off();
     fountain.off();
     return;
  }
   

  if (pattern == 0 || (mode == 0 && patternChange.isNow()))
  {
    D("Mode 0 and patternChange is now()");
    pattern = random(1,NUM_PATTERNS + 1);
    D2("Random pattern = ",pattern);
    colorChange.expire();
    if (pattern >= 4)
      fountain.off();
  }
  
   
  if (colorChange.isNow())
  {
    D2(pause(1000), "Change Color");
    rgb.randomColor();
  }
  
  if (internalPatternChange.isNow())
  {
    if (pattern == 1) // left to right
    {
      Dln("Pattern 1");
      for (int p = 0; p < fountain.size(); ++p)
      {
        fountain.pump[p].setNewValue(255);
      }

 /*
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
*/
    }
    
    else if (pattern == 2) // random on
    {
      Dln("Pattern 2");
      int pumpnum = random(0,fountain.size());
      int maxvalue = pumpnum == 5 ? 128 : 255;
      
      fountain.off();
      fountain.pump[pumpnum].setNow(maxvalue);
    }
    else if (pattern == 3)
    {  
      Dln("Pattern 3");
      int pumpval;
      for (int p = 0; p < fountain.size(); ++p)
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
    else if (pattern == 4)
    {  
      internalPatternChange.setSeconds(3); // 3
      Dln("Pattern 4");
      int pumpval;
      int pumpon = random(0,fountain.size());  // Skip a couple turns.
      for (int p = 0; p < fountain.size(); ++p)
      {
        D("Pump "); D(p);
        D2(" value = ", fountain.pump[p].getValue());
        if (p == pumpon)
          fountain.pump[p].setNow(255);
        else
          fountain.pump[p].setNewValue(0);      
      }
      D2("Pump on: ",pumpon);
    }
    else if (pattern == 5) // Mirror
    {
      Dln("Pattern 5");
      for (int p = 5; p >= 0; --p)
      {
        switch (p)
        {
            // shift to front
            case 5:
              fountain.pump[p].setNewValue(fountain.pump[p-1].getValue());
              D2(p == 5 ? "F=":"R=",fountain.pump[p].getNewValue());
              //D2("F(currentvalue)=",fountain.pump[p].getValue());
              break;

            case 4:
            case 3:
              fountain.pump[p].setNow(fountain.pump[p-1].getValue());
              D2(p == 5 ? "F=":"R=",fountain.pump[p].getNewValue());
              break;

            case 2:  // center;
              fountain.pump[p].setNewValue(random(0,256));
              D2("Center=",fountain.pump[p].getNewValue());
              break;
  
            // mirror
            case 1:
            case 0:
              fountain.pump[p].setNow(fountain.pump[4 - p].getNewValue());
              D2("Left=",fountain.pump[p].getNewValue());
              break;
        }
      }
    }
  } 
    
  if (lightsOn)
    rgb.update();
  else
    rgb.off();
  fountain.update();
  Dln(fountain.displayValue());
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
