#include "pwm.h"
#define debug false
#include "debug.h"
#include "Arduino.h"

Pwm::Pwm(int pin){
  init(pin);
}

void Pwm::init(int pin) {
  _pin = pin;
  _min = _rawValue = _currentValue = _value = _newValue = _fadeSpeed = 0;
  pinMode(pin, OUTPUT); 
  setNow(0);
}

void Pwm::setMin(int min)
{
  _min = min;
}

// Update PWM pin based on current time and fadeSpeed.
void Pwm::update() {
  int newval = _newValue;
  // Need to fade?
   if (_value != _newValue)
   {
     // Compute elapsed time
     unsigned long current = millis();
     D("Pin ");D(_pin);D(" Times: "); D("_startTime=");D(_startTime);D(" current=");Dln(current);

     long elapsed = (current - _startTime);
     D("Pin ");D(_pin);D2(" elapsed time=",elapsed);
     D("Pin ");D(_pin);D2(" fade time=",_fadeSpeed);
     // Past fade speed? then done fading
     if (current < _startTime || elapsed > _fadeSpeed) {
       _startTime = current;
       _value = _newValue;
        D("Pin ");D(_pin);D2(" elapsed time\n_value=",_value);
     }
     else {
       // compute percentage into fade time.
       long percent = elapsed * 100L / _fadeSpeed;
       D("Pin ");D(_pin); D2(" percent = ",percent);
       D("Pin ");D(_pin); D2(" value=",_value);
       // compute porpotional adjustment to value;

       newval = _value + (_newValue - _value) * percent / 100;
       if (newval > 255)
         newval = 255;
         
       if (newval < 0)
         newval = 0;
         
       D("Pin ");D(_pin); D2(" newval=",newval);       
       Dln();
     }
   }
   // write output value;
   writePin(newval);
}

void Pwm::writePin(int newval)
{
  if (newval  // on
      && _min > newval)  // new value is less than min then off
  {
    D3("newval=",newval," is less than min.  forcing off");
    newval = 0;
    _value = _newValue = 0;
  }

  _currentValue = newval;
    
#if defined(__AVR_ATmega2560__)
  analogWrite(_pin, newval);
#else
  switch(_pin)
  {
    // anlogwrite capable pins:
    case 9:
    case 10:
    case 11:
      analogWrite(_pin, newval);
      break;
    
      // Pwm Pins but treat as digital for consistent diplay.
    case 3:
    case 5:
    case 6:
      // digital only pins
    case 4:
    case 7:
    case 8:  
    default:
       // simulate pwm for non-pwm pins
       int digitalval;
       digitalWrite(_pin, digitalval = newval && (millis() % 256 <= newval) ? HIGH : LOW);
       //D4("Simulated analog pin ", _pin, " value=", digitalval);
       break;
  }
 
#endif
}


void Pwm::setFadeSpeed(unsigned int speed){
  _fadeSpeed = speed * 1000U;
}

void Pwm::setNewValue(int value){
  D("pwm::setNewValue[_pin=");
  D4( _pin,"](",value,")");
  int rawvalue = _rawValue = value;
  // Need to adjust for minimum value?
  if (_min != 0)
  {
    if (value == 0 && _currentValue != 0)  // Turning off?
      value = _min -1;      // Set to allow fade speed.
    else if (value != 255) // Scale
      value = _min + value * (255L - _min + 1) / 256;
      
    if (rawvalue && _currentValue == 0)  // Turning On?
      _currentValue = _min;           // Set to allow for fade speed.
      
    D4("Adjusting value ", rawvalue, " to ", value);
  }

  if (value == _newValue)
  {
    Dln("No change in value");
    return;
  }
  
  _newValue = value;
  _value = _currentValue;
  _startTime = millis();
}

int Pwm::getNewValue(){
  return _newValue;
}

int Pwm::getValue(){
  return _currentValue;
}

int Pwm::getRawValue(){
  return _rawValue;
}

void Pwm::off() {
  setNow(0);
}

void Pwm::setNow(int val)
{
  _value = _currentValue = val; // Force no fade processing.
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
