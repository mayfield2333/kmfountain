#include "Arduino.h"
#define debug false
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"

#define REMOTE_PIN 22
#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

#define NUM_PUMPS 6
#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

Remote remote;
RGBLed rgb;
Pump   pump[NUM_PUMPS];

unsigned long change;
boolean on = false;

void setup() {
  // USB can be used for Debug output.
  Serial.begin(9600);
  delay(100);
  Dln("setup");
  remote.init(REMOTE_PIN);
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
  for (int pp = 0; pp < NUM_PUMPS; ++pp)
  {
   pump[pp].init(FIRST_PUMP_PIN + pp);
  }
  change = millis();
  randomSeed(analogRead(0));
}


boolean test = true;
int speed = 1000;

void loop()
{
  
  if (remote.pressed())
  {
     D("button pressed");

    if (remote.getButton(0)->hasChanged())
    {
      on = !on;
      if (!on)
        rgb.allOff();
      else
        remote.clear();
      D("Button 0");
      remote.getButton(0)->getState();
    }

    if (on && remote.getButton(3)->hasChanged())
    {
       D("Button 3 - DOING TEST");
       remote.getButton(3)->getState();
       test = true;
    }
       
    if (on && remote.getButton(1)->hasChanged()) {
       D("Button 1 - change speed");
       remote.getButton(1)->getState();
       speed = speed != 0 ? 0 : 3000;
       rgb.setFadeSpeed(speed);
       change = millis() + 10000;
    }
    
    if (on && remote.getButton(2)->hasChanged())
    {
      D("Button 2 - flash");
      remote.getButton(2)->getState();
      remote.clear();
      for (int ii = 0; ii < 10; ii++)
      {
          rgb.allOff();
          D("off");
          if (pause(500))
          {
            D("Break out of flash");
            break;
          }
          D("on");
          rgb.setRGB(255,255,255);
          rgb.setFadeSpeed(0);
          rgb.update();
          if (pause(500))
          {
            D("Break out of flash");
            break;
          }
      }
      D("End of flash");
      change = 0;  // force change
    }
  }
    
   if (!on)
     return;
     
   if (test) {
    rgb.test(&remote);
    test = false;
  }
    
  if (remote.pressed())
  {
    D("Pressed\n");
    return;
  }
  
  D2("\n" + "millis()=",millis());
  D2("change=",change);
  if (millis() > change)
  {
    D2(pause(1000), "Change time");
    
    int r;
    change = millis() + 5000;
    rgb.randomColor();
    Dln();
  } 
    
  rgb.update();
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
