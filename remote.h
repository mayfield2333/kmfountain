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
#if !defined(__AVR_ATmega2560__)
  // This isn't specific to Uno but rather my test board doesn't have a latching switch
  int fakelatch;
#endif
  
public:
  
  Button(){};
  
  Button(int pin_num);  
  void init(int pin_num);
  int getState();  // Consumes the state change.
  
  int peekState();  // simply reads state without clearing change state.s
  
  boolean pressed();  // doesn't consume press.
  boolean hasChanged();  // consumes the button press.
};

#if defined(__AVR_ATmega2560__)
#define NUM_BUTTONS 4
#else
#define NUM_BUTTONS 1
#endif

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
