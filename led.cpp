#include "led.h"
#define debug false
#include "debug.h"
#include "Arduino.h"

int color() {
    int r = random(0,1) == 0 ? 255 : random(64,255);
    return r;
}

int tint() {
  if (random(0,4) != 0)
    return 0;
  return random(0,20);
}

void RGBLed::test(Remote *remote) {
  int elapse = 500;
  Dln("RGBLED:test");
  setFadeSpeed(elapse/2);
  setRGB(255,0,0);
  unsigned long offtime = millis() + 6000;
  while(millis() < offtime)
  {
    update();
    if (remote->pressed())
      return;
  }
  //allOff();
  setRGB(255,64,0);
  offtime = millis() + elapse;
  while(millis() < offtime)
  {
    if (remote->pressed())
      return;
    update();
  }
  setRGB(0,255,0);
  offtime = millis() + elapse;
  while(millis() < offtime)
  {
    if (remote->pressed())
      return;

    update();
  }
  //allOff();
  setRGB(0,127,127);  
  offtime = millis() + elapse;
  while(millis() < offtime)
  {
    if (remote->pressed())
      return;
    update();
  }

  setRGB(0,0,255);  
  offtime = millis() + elapse;
  while(millis() < offtime)
  {
    if (remote->pressed())
      return;
    update();
  }

  setRGB(127,0,127);  
  offtime = millis() + elapse;
  while(millis() < offtime)
  {
    update();
    if (remote->pressed())
      return;
  }


  setWhite(255);
  offtime = millis() + elapse;
  while (millis() < offtime)
  {
    if (remote->pressed())
      return;
    update();
  }
  
  setWhite(0);
  offtime = millis() + elapse;
  while (millis() < offtime)
  {
    if (remote->pressed())
      return;
    update();
  }
  
}

void RGBLed::randomColor()
{
    int primary = random(0,2);
    int secondary = random(0,1) == 0 ? -1 : random(0,2);  // Use primary only half the time.
    
    int r;
    setRed(r = ( (primary == 0 || secondary == 0) ? color() : tint()) );
    D2("Red=",r);
    setGreen(r = ( (primary == 1 || secondary == 1) ? color() : tint()) );
    D2("Green=", r);
    setBlue(r = ( (primary == 2 || secondary == 2) ? color() : tint()) );
    D2("Blue=",r);
    int speed;
    //setFadeSpeed(speed = random(4,10)* 1000);
    //D2("Speed=",speed);
}

void RGBLed::init(int blue_pin, int red_pin, int green_pin) {
 _rLed.init(red_pin);
 _gLed.init(green_pin);
 _bLed.init(blue_pin);
}

void RGBLed::setRed(int val) {
  _rLed.setNewValue(val);
}

void RGBLed::setGreen(int val) {
  _gLed.setNewValue(val);
}
void RGBLed::setBlue(int val) {
  _bLed.setNewValue(val);
}
void RGBLed::allOff() {
  _rLed.off();
  _gLed.off();
  _bLed.off();
}

void RGBLed::setRGB(int red, int green, int blue)
{
  setRed(red);
  setGreen(green);
  setBlue(blue);
}

void RGBLed::setWhite(int val) {
  _rLed.setNewValue(val);
  _gLed.setNewValue(val);
  _bLed.setNewValue(val);
}

void RGBLed::setFadeSpeed(int speed) {
  _rLed.setFadeSpeed(speed);
  _gLed.setFadeSpeed(speed);
  _bLed.setFadeSpeed(speed);
}

void RGBLed::setNow() {
  _rLed.setNow();
  _gLed.setNow();
  _bLed.setNow();
}

void RGBLed::update() {
  _rLed.update();
  _gLed.update();
  _bLed.update();
}

void RGBLed::off() {
  allOff();
}

