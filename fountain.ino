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
int mode = 0;
int pattern = 0;

boolean fountainOn = false;
boolean lightsOn = false;

Timer change;
Timer patternChange;

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
  patternChange.setSeconds(10);
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
  boolean pressAny = pressB || pressC || pressD;
  
#endif  
    // BUTTON A : Toggle Fountain ON/OFF?
    if (pressA)
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
#if defined(__AVR_ATmega2560__)

  if (pressAny)
  {  
    // Turn on with any button;
    if (!fountainOn)
      fountainOn = true;

    // BUTTON D: Lights on
    if (pressD)
    {
       Dln("Button D -Lights");
       lightsOn = !lightsOn;
    }
    
    // BUTTON B : Mode   
    if (pressB) {
       Dln("Button 1 - speed");
       change.setSeconds(change.getSeconds() >10 ? 10 : 30);
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
 #else
  if (remote.getButton(0)->peekState())
  {
     Dln("Button pressed 0");
     // Wait for release.
     if (++mode > 2)
         mode = 0;
      pattern = mode;
  }
 
#endif
    
   if (!fountainOn)
   {
     fountain.off();
     rgb.allOff();
     return;
   }
   
   int sensorValue = analogRead(lightSensorPin);
   if (lightsOn || sensorValue < 300)
     lightsOn = true;
   

 
  if (pattern == 0 || (patternChange.isNow() && mode == 0))
  {
    pattern = random(0,2);
    change.expire();
  }
    
  if (change.isNow())
  {
    D2(pause(1000), "Change time");
    rgb.randomColor();
    
    if (pattern == 1) // random height
    {
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
    }
    else if (pattern == 2) // random on
    {
      int pumpnum = random(0,6);
      int maxvalue = pumpnum == 5 ? 128 : 255;
      
      fountain.off();
      fountain.pump[pumpnum].setNewValue(maxvalue);
    }
    
    Dln();
  } 
    
  if (lightsOn)
    rgb.update();
  fountain.update();
}

boolean pause(unsigned mills)
{
  return remote.pause(mills);
}
