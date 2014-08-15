#include "led.h"
#define debug true
#include "debug.h"
#include "Arduino.h"

int color() {
    int r = random((random(0,4) == 0 ? 225: 175),255);
    return r;
}

int tint() {
  if (random(0,2) == 0)
    return 0;
  return random(0,(random(0,4) == 0 ? 127 : 70));
}

void RGBLed::test() {
  setFadeSpeed(4000);
  setRed(255);
  unsigned long offtime = millis() + 6000;
  while(millis() < offtime)
    update();
  allOff();
  setGreen(255);
  
  offtime = millis() + 6000;
  while(millis() < offtime)
    update();

  allOff();
  setBlue(255);
  
  offtime = millis() + 6000;
  while(millis() < offtime)
    update();

  allOff();
  setWhite(255);
  offtime = millis() + 5000;
  while (millis() < offtime)
    update();
  delay(2000);
  
  setWhite(0);
  offtime = millis() + 5000;
  while (millis() < offtime)
    update();
  
}

void RGBLed::randomColor()
{
    int primary = random(0,2);
    int secondary = random(0,5);  // Use primary only half the time.
    
    int r;
    setRed(r = ( (primary == 0 || secondary == 0) ? color() : tint()) );
    D2("Red=",r);
    setGreen(r = ( (primary == 1 || secondary == 1) ? color() : tint()) );
    D2("Green=", r);
    setBlue(r = ( (primary == 2 || secondary == 2) ? color() : tint()) );
    D2("Blue=",r);
    int speed;
    setFadeSpeed(speed = random(4,10)* 1000);
    D2("Speed=",speed);
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
void RGBLed::allOff(){
  _rLed.off();
  _gLed.off();
  _bLed.off();
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

void RGBLed::update() {
  _rLed.update();
  _gLed.update();
  _bLed.update();
}

