#include "pwm.h"

#ifndef PUMP_H
#define PUMP_H

class Pump  : public Pwm {    
};

class Fountain {
  Pump  pump[5];
  Pump  ring;
  
  void init(int startpin);
};
  
#endif
