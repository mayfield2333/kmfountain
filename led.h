#include "pwm.h"
#include "remote.h"

class Led  : public Pwm {    

    public:
      Led() : Pwm() {};

    void setNewValue(int value);

  /*
  
    Led(int pin);
    
    void init(int pin);
    
    // Update LED based on current time and fadespeed.
    void update();
    void setFadeSpeed(int);
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
    int colorIndex = 0;
        
    void init(int blue_pin, int green_pin, int red_pin);
    void setRed(int);
    void setGreen(int);
    void setBlue(int);
    void setRGB(int red, int green, int blue);
    void allOff();
    void off();
    void setWhite(int);
    void setFadeSpeed(int);
    void randomColor();
    void colorWheel();
    void update();
    void setNow();
    void test(Remote *remote);
};
