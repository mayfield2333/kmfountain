#include "Arduino.h"
#define debug true
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"

#define REMOTE_PIN 22
#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

int changeDelay = 5000;
int lightSensorPin = A0;    // select the input pin for the potentiometer
int lightLedPin = 13;      // select the pin for the LED
int lightSensorValue = 0;  // variable to store the value coming from the sensor

Remote remote;
RGBLed rgb;
Fountain fountain;

unsigned long change;
boolean on = false;

void setup() {
  // USB can be used for Debug output.
  Serial.begin(9600);
  Dln("setup");
  remote.init(REMOTE_PIN);
  
  pinMode(lightLedPin, OUTPUT);
  
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
  //rgb.test(&remote);
  
  fountain.init(FIRST_PUMP_PIN);
  //fountain.test(&remote);
  
  change = millis();
  randomSeed(analogRead(0));
  Dln("end setup");
}


boolean test = true;
int speed = 1000;

void loop()
{
  
  if (remote.pressed())
  {
     Dln("Button pressed");
    // BUTTON A : Toggle Fountain ON/OFF?
    if (remote.getButton(0)->hasChanged())
    {
      Dln("Button 0");
      on = !on;
      if (!on)
      {
        rgb.off();
        fountain.off();
        remote.clear();
        return;
      }
    }
    else
    {
      Dln("NOT Button 0");
    }
    
    // Turn on with any button;
    if (!on)
      on = true;

    // BUTTON D: TEST PUMP AND LIGHTS
    if (remote.getButton(3)->hasChanged())
    {
       D("Button 3 - DOING TEST");
       remote.getButton(3)->getState();
       test = true;       
       remote.clear();
    }
    else
    {
      Dln("Not Button 3");
    }
       
    // BUTTON B : Change Speed    
    if (remote.getButton(1)->hasChanged()) {
      
       D("Button 1 - change speed");
       remote.getButton(1)->getState();
       speed = speed != 0 ? 0 : 3000;
       rgb.setFadeSpeed(speed);
       change = millis() + 10000;
    }
    else
    {
      Dln("Not BUTTON 1");
    }
    
    // BUTTON C : flash
    if (remote.getButton(2)->hasChanged())
    {
      Dln("Button 2 - flash");
      while (!remote.pressed())
      {
        rgb.randomColor();
        rgb.setNow();
        int sensorValue = digitalRead(lightSensorPin);
        D2("FLASH sensorValue = ", sensorValue);  
        if (pause( sensorValue / 1024.0 * 4000))
        {
          D("Break out of flash");
          break;
        }
        rgb.allOff();
        D("off");
        if (pause(500))
          break;
      }
      Dln("End of flash");
      change = 0;  // force change
      rgb.allOff();
    }
    else
    {
        Dln("Not Button 2");
    }
  }
    
   if (!on)
   {
     fountain.off();
     rgb.allOff();
     return;
   }  
   
   if (test) {
    rgb.test(&remote);
    if (!remote.pressed())
      fountain.test(&remote);
    test = false;
  }
    
  if (remote.pressed())
  {
    D("Pressed\n");
    return;
  }
  
  if (millis() > change)
  {
    D2(pause(1000), "Change time");
    
    int r;
    change = millis() + changeDelay;
    rgb.randomColor();
    Dln();
  } 
    
  rgb.update();
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
