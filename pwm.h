#ifndef PWM_H
#define PWM_H

#include "Arduino.h"

class Pwm {
  protected:
    int _pin;
    int _value;
    int _newValue;
    int _fadeSpeed;
    int _min;
    unsigned long _startTime;  // time new value was set.
    

  public:
    Pwm() {};
  
    Pwm(int pin);
    
    void init(int pin);
    
    // Update LED based on current time and fadespeed.
    void update();
    void setFadeSpeed(int);
    void setNewValue(int value);
    void setNow(int value);
    void setNow();
    int getNewValue();
    int getValue();
    void off();
    void test();
};
#endif
