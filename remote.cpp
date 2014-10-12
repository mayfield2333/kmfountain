#include "Arduino.h"
#include "remote.h"

#define debug false
#include "debug.h"


Button::Button(int pin_num)
{
  init(pin_num);
}

void Button::init(int pin_num)
{
  pinMode(pin = pin_num, INPUT);
  lastread = digitalRead(pin);
  D("INIT : digital read pin/val is ");
  D3(pin, "=", lastread);
}

int Button::getState()
{
  lastread = peekState();
  D2("getState Lastread = ",lastread);
  return lastread;
}

int Button::peekState()
{
#if defined(__AVR_ATmega2560__)
  return digitalRead(pin);
#else
  D2("peekState pin ",pin);
  int val = digitalRead(pin);
  D2("real digitalRead = ", val);
  int last = -1;
  if (val == LOW)
  {
    D2("return fakelatch because val =",val);
    return fakelatch;
  }
    
  D2("Initial read = ", val);
  while ((last = digitalRead(pin)) == HIGH)
      ;
      
  D2("Last read = ", last);
    
  fakelatch = fakelatch != LOW ? LOW : HIGH;
  D2("peekState : fakelatch =",fakelatch);
  return fakelatch;
#endif
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
  D2("Remote Init : first pin =",firstpin);
  for (int ii = 0; ii< NUM_BUTTONS; ++ii )
  {
    int pin;
    button[ii].init(pin = (NUM_BUTTONS - 1) - ii + firstpin);
    D2("Init button pin ", pin);
  }
}

void Remote::init(int pin_a, int pin_b, int pin_c, int pin_d)
{
  Dln("Remote init");
  if (NUM_BUTTONS == 1)
  {
  Dln("Init remote one button");
  button[0].init(pin_a); // D0 on remote receiver.
  }
  else
  {
  button[0].init(pin_d); // D0 on remote receiver.
  button[1].init(pin_c); // D1 on remote receiver.
  button[2].init(pin_b); // D2 on remote receiver.
  button[3].init(pin_a); // D3 on remote receiver.
  }
}
 
Button* Remote::getButton(int button_ndx)
{
  if (button_ndx > NUM_BUTTONS - 1)
    return &button[0];
    
  return &button[button_ndx];
}

boolean Remote::pressed()
{
  boolean ispressed = false;
  for (int b = 0; b < NUM_BUTTONS && !ispressed; ++b)
  {
    ispressed |= button[b].pressed();
  }
  return ispressed;
}

int Remote::getState()
{
  int ret = 0;
  for (int b = 0; b < NUM_BUTTONS; b++)
    ret |= (button[b].getState() == HIGH ? 1 << b : 0);
  return ret;
}

void Remote::clear()
{
   for (int ii = 0; ii < NUM_BUTTONS; ++ii)
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
