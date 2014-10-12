#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

class event {
public:
  boolean isPending();  // allows check for event without consuming/resetting.
  boolean isNow();  // true if event has happened and resets for next event.
  void reset();    // clear the event, waiting for next event.
};

class Timer : public event {
protected:
  unsigned long eventtime;
  unsigned long delaytime;
  boolean active;
  boolean eventHappened;

public:
  
  Timer();
  
  void reset();
  void setSeconds(unsigned int seconds);  
  void setMinutes(unsigned int minutes);
  unsigned int getSeconds();
  void expire();
  void off();
  void on();
  boolean isPending();
  boolean isNow();
};  

#endif
