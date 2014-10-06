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

int changeDelay = 5000;
int lightSensorPin = A0;    // select the input pin for the potentiometer
int lightLedPin = 13;      // select the pin for the LED
int lightSensorValue = 0;  // variable to store the value coming from the sensor

Remote remote;
RGBLed rgb;
Fountain fountain;

boolean fountainOn = false;
boolean lightsOn = false;

Timer change;

void setup() {
  // USB can be used for Debug output.
  Serial.begin(9600);
  Dln("setup");
  D2("Remote pin = ", REMOTE_PIN);
  remote.init(REMOTE_PIN);
  remote.clear();
  
  pinMode(lightLedPin, OUTPUT);
  
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
  //rgb.test(&remote);
  
  fountain.init(FIRST_PUMP_PIN);
  fountain.setFadeSpeed(10);
  //fountain.test(&remote);
  
  change.setSeconds(10);
  change.expire();
  randomSeed(analogRead(0));
  
  Dln("end setup");
}


boolean test = true;
int speed = 1000;

void loop()
{
  
#if defined(__AVR_ATmega2560__)
  if (remote.pressed())
  {
     Dln("Button pressed");
    // BUTTON A : Toggle Fountain ON/OFF?
    if (remote.getButton(0)->hasChanged())
    {
      Dln("Button 0");
      fountainOn = !fountainOn;
      if (!fountainOn)
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
    if (!fountainOn)
      fountainOn = true;

    // BUTTON D: TEST PUMP AND LIGHTS
    if (remote.getButton(3)->hasChanged())
    {
       Dln("Button 3 - DOING TEST");
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
      
       Dln("Button 1 - change speed");
       remote.getButton(1)->getState();
       speed = speed != 0 ? 0 : 3000;
       rgb.setFadeSpeed(speed);
       change.setSeconds(change.getSeconds() >10 ? 10 : 30);
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
        int sensorValue = analogRead(lightSensorPin);
        D2("FLASH sensorValue = ", sensorValue);  
        if (pause( sensorValue / 1024.0 * 4000))
        {
          Dln("Break out of flash");
          break;
        }
        rgb.allOff();
        Dln("off\n");
        if (pause(500))
          break;
      }
      Dln("End of flash");
      change.expire();
      rgb.allOff();
    }
    else
    {
        Dln("Not Button 2");
    }
  }
#else
  if (remote.getButton(0)->peekState())
  {
     Dln("Button pressed 0");
     // Wait for release.
     while (remote.getButton(0)->getState() != 0)
       delay(1);
    // BUTTON D : Toggle Fountain ON/OFF?
    remote.getButton(0)->hasChanged();
    Dln("Button 0");
    fountainOn = !fountainOn;
  }
  
  if (test) {
    rgb.test(&remote);
    Dln("Start test");
    remote.clear();
    D2("remote getstate()=",remote.getState());
    fountain.test(&remote);
    Dln("End test");
    if (remote.pressed())
       Dln("Button was pressed?");
    test = false;
  }

#endif
    
   if (!fountainOn)
   {
     fountain.off();
     rgb.allOff();
     return;
   }
  
  if (change.isNow())
  {
    D2(pause(1000), "Change time");
    rgb.randomColor();
    
    int pumpval;
    for (int p = 0; p < NUM_PUMPS; ++p)
    {
      pumpval = random(30,256);
      D("Pump ");
      D3(p," = ", pumpval);
      fountain.pump[p].setNewValue(pumpval);
    }

    pumpval = random(0,4) == 0 ? 128: 0;
    D2("Pump 5 = ", pumpval);
    fountain.pump[5].setNewValue(pumpval);
    
    Dln();
  } 
    
  rgb.update();
  fountain.update();
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
