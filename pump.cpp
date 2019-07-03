
#include "pump.h"
#define debug true
#include "debug.h"

void Pump::init(int pin) {
  Pwm::init(pin);
  _min = 128;
  _minFadeSpeed = 500;
}

void Pump::_writePin(int newval) 
{
  /*
  D2("_writePin = ", newval);
  if (newval == 0) {
     newval = 2;  // Try to reduce blink.
     D2("_writePin adjustvalue = ", newval);
  }

  */
  //_currentValue = newval;  // For some reason, this prevents newvalue from being set correctly?
  analogWrite(_pin, newval);
}

void Fountain::init(int firstpin)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].init(firstpin + pp);
  }
}

void Fountain::setFadeSpeed(int speed)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].setFadeSpeed(speed);
  }
}

void Fountain::test(Remote *remote)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   if (remote->pressed())
     break;
   pump[pp].test();
  }
}

void Fountain::off() {
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].off();
  }
}

void Fountain::update() {
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].update();
  }
  //displayValue();
}

char Fountain::displayValue() 
{
    D("CurrValue Pumps[0-n] = ");
    for (int p = 0; p < size(); ++p)
    {
      D(" ");
      D(pump[p].getValue());
   }
   Dln("");
   
   //delay(500);   
   D("NewValue Pumps[0-n] = ");
   for (int p = 0; p < size(); ++p)
   {
      D(" ");
      D(pump[p].getNewValue());
   }
   Dln("");
   Dln("");
   //delay(500);   

}

