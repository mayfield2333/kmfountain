#include "pwm.h"
#include "remote.h"

class Led  : public Pwm {    

    public:
      Led() : Pwm() {};

  /*
  
    Led(int pin);
    
    void init(int pin);
    
    // Update LED based on current time and fadespeed.
    void update();
    void setFadeSpeed(int);
    void setNewValue(int value);
    void setNow(int value);
    int getNewValue();
    int getValue();
    void off();
    */
};

class RGBLed {
  
 public: 
    Led _rLed;
    Led _gLed;
    Led _bLed;
        
    void init(int blue_pin, int green_pin, int red_pin);
    void setRed(int);
    void setGreen(int);
    void setBlue(int);
    void setRGB(int red, int green, int blue);
    void allOff();
    void setWhite(int);
    void setFadeSpeed(int);
    void randomColor();
    void update();
    void test(Remote *remote);
};
