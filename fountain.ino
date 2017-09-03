#include "Arduino.h"
#define debug true
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"
#include "timer.h"

//#define TESTING

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

//#if defined(__AVR_ATmega2560__)
#if !defined(TESTING)
static const int patternChangeSeconds = 120;
static const int colorChangeSeconds = 10;
static const int internalPatternChangeSeconds = 15;
static const int pumpfade = 4;
static const int lightfade = 3;
static const long autoOffMinutes = 120;
static const int pumpmin = 150;
static const bool enablepumps = true;
boolean forceLightsOn = false;

#else
static const int patternChangeSeconds = 35;
static const int colorChangeSeconds = 15;
static const int internalPatternChangeSeconds = 7;
static const int pumpfade = 5;
static const int lightfade = 5;
static const long autoOffMinutes = 5;
static const int pumpmin = 0;
static const bool enablepumps = false;
boolean forceLightsOn = true;

#endif

int mode = 0;
int colormode = 0;
int pattern = 0;
int pattern_one_state = 0;
const int p5reset = 11;
int p5state = 0;
int p5previous = 0;

boolean fountainOn = true;  //Always on
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
Timer buttonTimer;

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
  fountain.pump[5].setMin(pumpmin);
  //fountain.test(&remote);
  
  colorChange.setSeconds(colorChangeSeconds);
  colorChange.expire();
  patternChange.setSeconds(patternChangeSeconds);
  
  internalPatternChange.setSeconds(internalPatternChangeSeconds);
  randomSeed(analogRead(A0));
  
  autoOff.setMinutes(autoOffMinutes);
  
  buttonTimer.setSeconds(5);
  Dln("end setup");
}


boolean test = false;
int speed = 1000;

void loop()
{
  boolean pressAny = false;
  boolean pressA = remote.getButton(0)->hasChanged();
  boolean pressB = false;
  boolean pressC = false;
  boolean pressD = false;

#ifndef HAVE_REMOTE_CONTROL
  pressAny = pressA = false;
#else
#if defined(__AVR_ATmega2560__)
  pressB = remote.getButton(1)->hasChanged();
  pressC = remote.getButton(2)->hasChanged();
  pressD = remote.getButton(3)->hasChanged();
  pressAny = pressA /*|| pressB*/ || pressC || pressD;  // PressB is turn off.
#else 
  // Only 1 button on testbed.
  pressAny = pressA;
#endif
#endif

#if defined(TESTING)
    fountain.off();
    rgb.setWhite(255);
    rgb.setNow();
    //rgb.setRed(0);
    //rgb.setNow();
    //rgb.setBlue(0);
    //rgb.setNow();
    return;
#endif
    if (fountainOn == true && pressAny)
    {
      boolean pressInvalid = buttonTimer.isPending();
      buttonTimer.reset();
      Dln("Checking button timer");
      if (pressInvalid)
      {
        Dln("Too long since press, resetting");
        return;
      }
      
      Dln("allowing button press");
    }

    // BUTTON D: Lights on
    if (pressD)
    {
       Dln("Button D -Lights");
       forceLightsOn = !forceLightsOn;
    }
    
    // BUTTON C : Color mode/set
    if (pressC && lightsOn)
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
        Dln("Halloween color change mode");
      }
      
      if(colormode == 2)
      {
        Dln("Forth of July colors");
      }
      
      Dln("Prepare blink");
      for (int blink = 0; blink <= colormode; ++blink)
      {
        D2("top loop. blink = ", blink);
        Dln("Blink off");
        rgb.off();
        delay(1000);
        Dln("Blink on");
        rgb.setWhite(255);
        rgb.setNow();
        delay(1000); 
        D2("End loop. blink = ", blink);
      }
      Dln("Done blinking");
      rgb.off();
      delay(1000); 
      colorChange.expire();
    }

    // BUTTON B : Toggle Fountain ON/OFF?
    if (pressB)
    {
      Dln("Button B");
      fountainOn = false;
    }

   if (autoOff.isNow())
   {
     D2("AutoOff Minutes expired: ", autoOff.getMinutes());
    // fountainOn = false;
   }
     
   int sensorValue = analogRead(lightSensorPin);
   //D2("sensorValue=",sensorValue);
   if (forceLightsOn || sensorValue < 300)
   {
  //   D4("Sensor=",sensorValue,"forceLightsOn=",forceLightsOn);
     if (lightsOn == false)
        colorChange.expire();
     lightsOn = true;
   }
   /*
   else
   {
     D4("Sensor=",sensorValue,"forceLightsOn=",forceLightsOn);
     lightsOn = false;
   }
   */
 
#define NUM_PATTERNS 5

  if (pressA && fountainOn) 
  {
    if (mode == 0)
    {
      pattern = pattern == 1 ? 2 : 1;
      mode = pattern;
      D2("mode and pattern = ", mode);
    }
    else if ((mode = ++pattern) > NUM_PATTERNS)
    {
      mode = pattern = 0;
    }

    D2("New pattern = ",pattern);
    D2("current mode = ", mode);
    fountain.off();
    if (mode == 0)
        fountain.pump[5].setNow(255);
     
    else if (pattern < fountain.size())
      fountain.pump[pattern - 1].setNow(255);
      
    if (remote.pause(500))
       return;
    fountain.off();
    pattern_one_state = 10;
    p5state = p5reset;
    internalPatternChange.setSeconds(internalPatternChangeSeconds);
    internalPatternChange.expire();
  }
  
  if (pressAny)
  { 
    // Turn on with any button;
    if (!fountainOn)
    {
      Dln("Fountain turning on");
      patternChange.expire();
      mode = 0;  // Random patterns
      autoOff.reset();
      colorChange.expire();
      internalPatternChange.setSeconds(internalPatternChangeSeconds);
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
    Dln("Mode 0 and patternChange is now()");
    int oldpattern = pattern;
    while (oldpattern == pattern)
      pattern = random(1,NUM_PATTERNS + 1);
    D2("Random pattern = ",pattern);
    internalPatternChange.setSeconds(internalPatternChangeSeconds);
    internalPatternChange.expire();
    p5state = p5reset;
    
    if (pattern >= 4)
      fountain.off();
  }
     
  if (colorChange.isNow() && lightsOn)
  {
    D2("colorchange now colormode=",colormode);
    switch (colormode)
    {
      case 0:
        rgb.randomColor();
        break;
      
      case 1: // Halloween;

        if (random(0,4) == 0)
        {
          Dln("Lightning");
          do {
            rgb.setWhite(255);
            rgb.setNow();
            delay(random(250,500));
            rgb.off();
            delay(random(500,2000));
          } while (random(0,3) != 0);
          Dln("End Lightning");
        }
        switch (random(0,3))
        {
          case 0: // red
            rgb.setRed(random(200,255));      
            rgb.setGreen(0);
            rgb.setBlue(random(0, 50));
            break;
          case 1: //orange
            rgb.setRed(random(170,255));      
            rgb.setGreen(random(60,100));
            rgb.setBlue(0);
            break;
          case 2:  
            // Purple    
            rgb.setRed(random(200,255));      
            rgb.setGreen(0);
            rgb.setBlue(random(125,200));
            break;
          case 3:  
            // Yellowp Green    
            rgb.setRed(random(200,255));      
            rgb.setGreen(random(200,255));
            rgb.setBlue(0);
            break;
        }
        break;
        
      case 2:
        switch (random(0,3))
        {
          case 0: // red
            rgb.setRGB(255,0,0);
            break;
          case 1: //white
            rgb.setRGB(255,255,255);
            break;
          case 2:
            rgb.setRGB(40,40,255);
            break;
        }
        break;
    }
  }
  
  if (internalPatternChange.isNow())
  {
    if (pattern == 1) // all on at half, 3qtr or full;
    {
      Dln("Pattern 1");
      int p1value = random(2,5) * 64;
      if (p1value == 256) p1value = 255;
      for (int p = 0; p < fountain.size() - 1; ++p)
      {
        fountain.pump[p].setNewValue(p1value);
      }
    }
    
    else if (pattern == 2) // random on
    {
      internalPatternChange.setSeconds(3); 
      Dln("Pattern 2");
      int pumpnum = random(0,fountain.size());
      D2("Turn on Pump#",pumpnum);
      int maxvalue = pumpnum == 5 ? 200 : 255;
      
      fountain.off();
      fountain.pump[pumpnum].setNow(maxvalue);
    }
    else if (pattern == 3)  // rolling
    {
      internalPatternChange.setSeconds(1); 
      Dln("Pattern 3");
      p5previous = p5state;
      ++p5state;
      if (p5state > 11)
        p5state = 0;
      D4("p5state=",p5state," p5previous=",p5previous);
      if (p5state == 0)
         fountain.off();
      if (p5state <= 4)
      {
        fountain.pump[p5state].setNow(255);
        if (p5previous < 4)
          fountain.pump[p5previous].setNow(0);
      }
      else if (p5state <= 8)
      {
        fountain.pump[8-p5state].setNow(255);
        fountain.pump[8-p5previous].setNow(0);
      }
      else if (p5state == 9)
      {
        fountain.pump[0].setNow(0);
        fountain.pump[5].setNow(255);
      }
      else if (p5state == 10)
      {
        fountain.pump[2].setNow(255);
        fountain.pump[5].setNow(0);
      }
      else
      {
        fountain.off();
        if (random(0,3) == 0)
          patternChange.expire();
      }
      
    }
    /*
    else if (pattern == 6)
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
    }
    */

    else if (pattern == 4)
    {  
      internalPatternChange.setSeconds(3);
      Dln("Pattern 4");
      int pumpval;
      int pumpon = random(0,fountain.size());
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
    else if (pattern == 5) // Symetrical
    {
      Dln("Pattern 5");
      for (int p = 5; p >= 0; --p)
      {
        switch (p)
        {
            // shift to front
            case 5:
              fountain.pump[p].setNewValue(fountain.pump[p-1].getRawValue());
              //D4("F",p,"=",fountain.pump[p].getRawValue());
              //D2("F(currentvalue)=",fountain.pump[p].getValue());
              break;

            case 4:
            case 3:
              fountain.pump[p].setNow(fountain.pump[p-1].getRawValue());
              //D4("R",p,"=",fountain.pump[p].getRawValue());
              break;

            case 2:  // center;
            
              int value;
              do {
                value = random(0,8) * 32 - 1;
                if (value < 0)
                  value = 0;
              }
              while
                (value == 0 && fountain.pump[1].getRawValue() == 0);
              fountain.pump[p].setNewValue(value);
              //D4("C",p,"=",fountain.pump[p].getRawValue());
              break;
  
            // mirror
            case 1:
            case 0:
              fountain.pump[p].setNow(fountain.pump[4 - p].getRawValue());
              //D4("L",p,"=",fountain.pump[p].getRawValue());
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
  //Dln(fountain.displayValue());
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
