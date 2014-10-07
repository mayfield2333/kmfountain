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
   writePin(newval);
}

void Pwm::writePin(int newval)
{
#if defined(__AVR_ATmega2560__)
  analogWrite(_pin, newval);
#else
  switch(_pin)
  {
    // anlogwrite capable pins:
    case 3:
    case 5:
    case 6:
    case 9:
    case 10:
    case 11:
      analogWrite(_pin, newval);
      break;
    
      // digital only pins
    case 4:
    case 7:
    case 8:  
    default:
       // simulate pwm for non-pwm pins
       int digitalval;
       digitalWrite(_pin, digitalval = (millis() % 256 < newval) ? HIGH : LOW);
       if (digitalval < newval)
       D2("Simulated analog", digitalval);
       break;
  }
 
#endif
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
  _value = val; // Force no fade processing.
  setNewValue(val);
  update();
}

void Pwm::setNow()
{
  setNow(_newValue);
}

void Pwm::test()
{
  setNow(80);
  delay(1000);
  setNow(200);
  delay(1000);
  setNow(0);
}
