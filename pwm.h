#ifndef PWM_H
#define PWM_H

#include "Arduino.h"

class Pwm {
  protected:
    int _pin;
    int _currentValue;
    int _value;
    int _newValue;
    unsigned int _defaultFadeSpeed;
    unsigned int _fadeSpeed;  // In milliseconds.
    int _min;
    unsigned long _startTime;  // time new value was set.
    int _rawValue;            // raw new value;

    unsigned int _minFadeSpeed;
    

  public:
    Pwm() {};
  
    Pwm(int pin);
    
    void init(int pin);
    
    // Update LED based on current time and fadespeed.
    void update();
    void setFadeSpeed(unsigned int);  // In Seconds.
    void setNewValue(int value);
    void setNow(int value);
    void setNow();
    int getNewValue();
    int getValue();
    int getRawValue();
    void off();
    void test();
    void writePin(int value);
    void setMin(int min);

 protected:
    virtual void _writePin(int value);
};
#endif
