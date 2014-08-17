/*
  *******************************************************
  LED fountain
  Randomly turns on different 4 LEDs and adjusts PWM
  for a percentage, representing the pump.
  *******************************************************
 */
#ifndef remote_lib
#define remote_lib

#include "Arduino.h"

class Button {
  int pin;
  int lastread;
  
public:
  
  Button(){};
  
  Button(int pin_num);  
  void init(int pin_num);
  int getState();
  
  int peekState();
  
  boolean hasChanged();
};


class Remote {
  
public:
 Button button[4];
 
  Remote(){};
  
  void init(int pin_a, int pin_b, int pin_c, int pin_d);
  void init(int firstpin);
 
  Button* getButton(int button_ndx);
  
  boolean pressed();

  int getState();

  boolean pause(int msec);
  void clear();
};


#endif
