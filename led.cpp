#include "led.h"
#define debug false
#include "debug.h"
#include "Arduino.h"

int color() {
    int r = random(0,5) < 3 ? 255 : random(200,240);
    return r;
}

int tint() {
  if (random(0,5) != 0) {
    Dln("Tint() return 0");
    return 0;
  }
  int ret = random(5,15)*10;
  D2("Tint return ", ret);
  return ret;
}

void Led::setNewValue(int value)
{
  D4("Led[pin=",_pin,"]::setNewValue = ", value);
  Pwm::setNewValue(value);
}

void RGBLed::test(Remote *remote) {
  int elapse = 1000;
  Dln("RGBLED:test");
  setFadeSpeed(elapse/2);
  setRGB(255,0,0);
  unsigned long offtime = millis() + elapse;
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

void RGBLed::colorWheel() {
  const int maxColorIndex = 5;
  if (colorIndex++ > maxColorIndex)
    colorIndex = 0;

  int primary = colorIndex / 2;
  int secondary = (primary + colorIndex % 2) % 3;

  if (primary > 2)
    { Serial.print("**** ERROR ****: primary == "); Serial.println(primary); }
  if (secondary != primary && secondary > 2)
    { Serial.print("**** ERROR ****: secondary == "); Serial.println(secondary);}

  if (primary == 0 || secondary == 0)
    setRed(255);
  if (primary == 1 || secondary == 1)
    setGreen(255);
  if (primary == 2 || secondary == 2)
    setBlue(255);
}

void RGBLed::randomColor()
{
    int primary = random(0,3);  // Choose the primary color 0=red, 1=Green, 2=blue
    int secondary = random(0,2) == 0 ? -1 : random(0,3);  // Use primary only half the time.
    
    int r;
    int totalbrightness;

    do {
    totalbrightness = 0;
    setRed(r = ( primary == 0 ? 255 : (secondary == 0 ? color() : tint() ) ) );
    totalbrightness += r;
    D2("Red=",r);
    
    setGreen(r = ( primary == 1 ? 255 : (secondary == 1 ? color() : tint() ) ) );
    totalbrightness += r;
    D2("Green=", r);

    setBlue(r = ( primary == 2 ? 255 : (secondary == 2 ? color() : tint() ) ) );
    totalbrightness += r;
    D2("Blue=",r);
    D("\n");
#if debug
    if (totalbrightness < 160) {
      Dln("=======================================================");
      D2("Oops too dim? totalbrightness =", totalbrightness);
    }
#endif
    }
    while (totalbrightness < 160);
    
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
  Dln("setRGB");
  D2("red = ", _rLed.getValue());
  D2("green=", _gLed.getValue());
  D2("blue= ", _bLed.getValue());
  Dln();
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
  Dln("LED setnow");
}

void RGBLed::update() {
  _rLed.update();
  _gLed.update();
  _bLed.update();
//  D2("red = ", _rLed.getValue());
//  D2("green=", _gLed.getValue());
//  D2("blue= ", _bLed.getValue());
#if true
  if (_rLed.getNewValue() + _gLed.getNewValue() + _bLed.getNewValue() < 128) {
     Dln("---------DIM DIM DIM-------------------");
     if (Serial.available() > 0) {
      int inbyte = Serial.read();
      while (Serial.available() == 0);
      inbyte = Serial.read();

      while (_rLed.getNewValue() + _gLed.getNewValue() + _bLed.getNewValue() < 128) {
        _rLed.setNewValue(_rLed.getNewValue() * 2);
        _gLed.setNewValue(_gLed.getNewValue() * 2);
        _bLed.setNewValue(_bLed.getNewValue() * 2);
      }
     }
  }
#endif
//  Dln();
}

void RGBLed::off() {
  allOff();
  Dln("allOff");
}

