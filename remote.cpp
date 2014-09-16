#include "Arduino.h"
#include "remote.h"
#include "debug.h"

Button::Button(int pin_num)
{
  init(pin_num);
}

void Button::init(int pin_num)
{
  pinMode(pin = pin_num, INPUT);
  lastread = digitalRead(pin);
}

int Button::getState()
{
  return lastread = peekState();
}

int Button::peekState()
{
  return digitalRead(pin);
}

boolean Button::pressed()
{
  int val = peekState();
  return lastread != val;
}

boolean Button::hasChanged()
{
  int oldval = lastread;
  int val = getState();
  return oldval != val;
}

void Remote::init(int firstpin)
{
  for (int ii = 0; ii< 4; ++ii )
    button[ii].init(3 - ii + firstpin);
}

void Remote::init(int pin_a, int pin_b, int pin_c, int pin_d)
{
  button[0].init(pin_d); // D0 on remote receiver.
  button[1].init(pin_c); // D1 on remote receiver.
  button[2].init(pin_b); // D2 on remote receiver.
  button[3].init(pin_a); // D3 on remote receiver.
}
 
Button* Remote::getButton(int button_ndx)
{
  return &button[button_ndx];
}

boolean Remote::pressed()
{
  boolean ispressed = false;
  for (int b = 0; b < 4 && !ispressed; ++b)
  {
    ispressed |= button[b].pressed();
  }
  return ispressed;
}

int Remote::getState()
{
  int ret = 0;
  for (int b = 0; b < 4; b++)
    ret |= (button[b].getState() == HIGH ? 1 << b : 0);
  return ret;
}

void Remote::clear()
{
   for (int ii = 0; ii < 4; ++ii)
     button[ii].getState();
}

boolean Remote::pause(int msec)
{
  unsigned long now = millis();
  unsigned long start_time = now;
  unsigned long end_time = now + msec;
  
  do
  {
    if (pressed())
    {
      D("Detected button press\n");
      return true;
    }

    delay(500);
    now = millis();
  }
  while ((start_time > end_time && now > start_time) || now < end_time);
  D("No Button Pressed during remote.delay\n");
  return false;  // return true if interrupt detected.
}
