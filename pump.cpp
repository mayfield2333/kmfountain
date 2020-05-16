
#include "pump.h"
#define debug true
#include "debug.h"

void Pump::init(int pin) {
  Pwm::init(pin);
  _min = 128;
}

void Fountain::init(int firstpin)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].init(firstpin + pp);
  }
}

void Fountain::setNewValue(int val)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].setNewValue(val);
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
}

char Fountain::displayValue() 
{
    D("Pumps[0-n] = ");
    for (int p = 0; p < size(); ++p)
    {
      D(" ");
      D(pump[p].getValue());
   }
   Dln("");
   delay(500);   
}

