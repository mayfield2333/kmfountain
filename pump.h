#include "remote.h"
#include "pwm.h"

#ifndef PUMP_H
#define PUMP_H

class Pump  : public Pwm { 
public:  
   void init(int pin); 
};

#define NUM_FOUNTAIN_PUMPS 6
class Fountain {
public:
  Pump  pump[NUM_FOUNTAIN_PUMPS];
  Pump  ring;
  
public:
  void init(int startpin);
  void test(Remote *);
  void off();
  void update();
  void setFadeSpeed(int speed);
  int size() { return NUM_FOUNTAIN_PUMPS; };
  char displayValue();
  void setNewValue(int val);
};
  
#endif
