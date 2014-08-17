#include "pwm.h"
#define debug false
#include "debug.h"
#include "Arduino.h"

Pwm::Pwm(int pin){
  init(pin);
}

void Pwm::init(int pin) {
  _pin = pin;
  _min = _value = _newValue = _fadeSpeed = 0;
  pinMode(pin, OUTPUT); 
  setNow(0);
}

// Update PWM pin based on current time and fadeSpeed.
void Pwm::update() {
  int newval = _newValue;
  // Need to fade?
   if (_value != _newValue)
   {
     // Compute elapsed time
     unsigned long current = millis();
     long elapsed = (current - _startTime);
     
     // Past fade speed? then done fading
     if (elapsed > _fadeSpeed || _fadeSpeed == 0) {
       _value = _newValue;
       D2("elapsed time\n_value=",_value);
     }
     else {
       // compute percentage into fade time.
       long percent = elapsed * 100L / _fadeSpeed;
       D2("percent = ",percent);
       D2("value=",_value);
       // compute porpotional adjustment to value;
       int val = getValue();
       newval = val + (_newValue - val) * percent / 100;
       D2("newval=",newval);
       if (newval > 255)
         newval = 255;
         
       if (newval < 0)
         newval = 0;
         
       Dln();
     }
   }
   // write output value;
   analogWrite(_pin, newval);
}

void Pwm::setFadeSpeed(int speed){
  _fadeSpeed = speed;
}

void Pwm::setNewValue(int value){
  _newValue = value;
  _startTime = millis();
}

int Pwm::getNewValue(){
  return _newValue;
}

int Pwm::getValue(){
  return _value;
}

void Pwm::off() {
  setNow(0);
}

void Pwm::setNow(int val)
{
  _value = val;
  setNewValue(val);
  analogWrite(_pin, val);
}


