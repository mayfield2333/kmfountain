#include "timer.h"

Timer::Timer()
{
    reset();
}

/**
* Reset timer to start over and clear any event;
*/
void Timer::reset()
{
  active = true;
  eventtime = millis() + delaytime;
  eventHappened = false;
}

/**
* Set the event time in seconds;
*/
void Timer::setSeconds(unsigned int seconds)
{
  delaytime = ((unsigned long) seconds) * 1000;
}

unsigned int Timer::getSeconds()
{
  return delaytime / 1000;
}

/**
* set the event time in minutes
*/
void Timer::setMinutes(unsigned int minutes)
{
    setSeconds(minutes * 60);
}

/**
* return true if the timer expired, but hasn't been checked yet
*/
boolean Timer::isPending()
{
  unsigned long time = millis();
  if (active && eventtime < time && delaytime < time)
    eventHappened = true;
  return eventHappened;
}

/**
* return true of the timer has expired,
* and reset the timer;
*/
boolean Timer::isNow()
{
   if (!isPending())
     return false;
   
   reset();
   return true;
}

void Timer::expire()
{
  eventHappened = true;
}
