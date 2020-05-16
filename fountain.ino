#include "Arduino.h"

// Turn on /off debug here
#define debug false
#include "debug.h"

#include "remote.h"   // Not used.
#include "led.h"
#include "pump.h"
#include "timer.h"

//#define TESTING


#define USE_RANDOM_PATTERNS false
static const boolean useRandomColors = false;

/*
  Pin Assignments
*/ 
#if defined(__AVR_ATmega2560__)
#define REMOTE_PIN 22
#else
#define REMOTE_PIN 13
#endif

#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

static const int lightSensorPin = A0;    // select the input pin for the potentiometer
static const int lightLedPin = 13;      // select the pin for the LED

/*
   Constants
*/

static const int lightSensorValue = 0;  // variable to store the value coming from the sensor

#define NUM_PUMPS 6
#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

//#if defined(__AVR_ATmega2560__)
#if !defined(TESTING)
static const int patternChangeSeconds = 20;
static const int colorChangeSeconds = 10;
static const int internalPatternChangeSeconds = 4;
static const int pumpfade = 4;
static const int lightfade = 2;
static const long autoOffMinutes = 120;
static const int pumpmin = 150;
static const bool enablepumps = true;
boolean forceLightsOn = false;

#else
static const int patternChangeSeconds = 35;
static const int colorChangeSeconds = 3;
static const int internalPatternChangeSeconds = 3;
static const int pumpfade = 5;
static const int lightfade = 1;
static const long autoOffMinutes = 5;
static const int pumpmin = 0;
static const bool enablepumps = false;
boolean forceLightsOn = true;

#endif

enum pump_patterns {
  POPCORN, 
  RANDOM_PAIR,
  INSTANT_FULL_5,
  FULL_5,
  INSTANT_FULL_6,
  FULL_6,
  INSTANT_SIX_ONLY,
  SIX_ONLY,
  MOUNTAIN,
  VALLEY,
  SPLASH,

  /* Last Pattern.  Enum would be number of patterns */
  NUM_PATTERNS,
  
  /* Disabled */
  DECAY,
  SYMETRICAL, 
  FULL_OR_3QTR
};

// If not using randomPatterns, then use program list
int programList[] = { 
                      POPCORN, POPCORN, POPCORN, 
                      SPLASH,
                      RANDOM_PAIR,
                      INSTANT_FULL_5, INSTANT_FULL_6, INSTANT_SIX_ONLY, 
                      MOUNTAIN, VALLEY, FULL_5,
                      -1  // END PROGRAM restart.
                    };

int maxProgramIndex = sizeof(programList) / sizeof(int) - 2;  // Less End of program marker
int mode = 0;
int colormode = 0;
int pattern = NUM_PATTERNS; // force change
int nextPattern = -1;
int programIndex = 1000;    // force Change
int pattern_one_state = 0;
const int popcorn_reset = 11;
int popcorn_state = 0;
int popcorn_previous = 0;

boolean fountainOn = true;  //Always on
boolean lightsOn = false;

/*
  Fountain sensors and actuators
*/

Remote remote;
RGBLed rgb;
Fountain fountain;

/*
   Timers
*/

Timer colorChange;    // Color change
Timer patternChange;
Timer internalPatternChange;
Timer autoOff;
Timer buttonTimer;

void setup() {
  // USB can be used for Debug output.
  Serial.begin(9600);
  Dln("setup");
  D2("Remote pin = ", REMOTE_PIN);
  remote.init(REMOTE_PIN);
  
  pinMode(lightLedPin, OUTPUT);
  
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
  rgb.setFadeSpeed(lightfade);  // In seconds.
  //rgb.test(&remote);
  
  fountain.init(FIRST_PUMP_PIN);
  fountain.setFadeSpeed(pumpfade);  // in Seconds.
  fountain.pump[5].setMin(pumpmin);
  //fountain.test(&remote);
  
  colorChange.setSeconds(colorChangeSeconds);
  colorChange.expire();
  patternChange.setSeconds(patternChangeSeconds);
  
  internalPatternChange.setSeconds(internalPatternChangeSeconds);
  internalPatternChange.expire();
  patternChange.expire();
  randomSeed(analogRead(A0));
  
  autoOff.setMinutes(autoOffMinutes);
  
  buttonTimer.setSeconds(5);
  Dln("end setup");
}


boolean test = false;
int speed = 1000;

void loop()
{

#if defined(TESTING2)
    fountain.off();
    rgb.setWhite(255);
    rgb.setNow();
    //rgb.setRed(0);
    //rgb.setNow();
    //rgb.setBlue(0);
    //rgb.setNow();
    return;
#endif

  
   if (autoOff.isNow())
   {
     D2("AutoOff Minutes expired: ", autoOff.getMinutes());
    // fountainOn = false;
   }
     
   int sensorValue = analogRead(lightSensorPin);
   //D2("sensorValue=",sensorValue);
   if (forceLightsOn || sensorValue < 300)
   {
    //   D4("Sensor=",sensorValue,"forceLightsOn=",forceLightsOn);
     if (lightsOn == false)
        colorChange.expire();
     lightsOn = true;
   }

  int oldpattern = pattern;
  
  if (mode == 0 && patternChange.isNow())
  {
    Dln("-----------------");
    if (USE_RANDOM_PATTERNS) {
      Dln("Mode 0 and patternChange is now()");
      D2("Time = ", millis());
      while (oldpattern == pattern)
        pattern = random(0,NUM_PATTERNS);
      D2("Random pattern = ",pattern);
      nextPattern = -1;
    }
    else {
      if (++programIndex >= sizeof(programList)/sizeof(int) || programList[programIndex] == -1)
        programIndex = 0;
      pattern = programList[programIndex];
      D2("programIndex = ", programIndex);
      D2("Pattern = ", pattern);
      //if (pattern == RANDOM_PAIR)
      //  Serial.println("changed to random pairs");
      nextPattern = programList[(programIndex + 1) % (maxProgramIndex + 1)];
      D2("NextPattern = ", nextPattern);
    }
    
    // Reset Common state  
    fountain.setFadeSpeed(pumpfade);  // in Seconds.
    internalPatternChange.setSeconds(internalPatternChangeSeconds);
    patternChange.setSeconds(patternChangeSeconds);
    internalPatternChange.expire();
    popcorn_state = popcorn_reset;
  }
     
  if (colorChange.isNow() && lightsOn)
  {
    if (useRandomColors){
      Dln("Choose Random color");
      rgb.randomColor();
    }
    else {
      Dln("Choose from ColorWheel");
      rgb.colorWheel();
      D2("red = ", rgb._rLed.getNewValue());
      D2("green = ", rgb._gLed.getNewValue());
      D2("blue = ", rgb._bLed.getNewValue());
    }
    //changecolor(); // Supported buttons for different color modes.
  }
  
  if (internalPatternChange.isNow())
  {
    switch (pattern) {
      case FULL_OR_3QTR:
        {
          // all on at half, 3qtr or full;
          Dln("Pattern - FULL_OR_3QTR");
          int p1value = random(7,9) * 32 - 1;
          if (p1value >= 256) p1value = 255;
          for (int p = 0; p < fountain.size() - 1; ++p)
          {
            fountain.pump[p].setNewValue(p1value);
          }
        }
        break;

      case RANDOM_PAIR:
        {
          Dln("Pattern RANDOM_PAIR");

          if (pattern != oldpattern) {
            //Serial.println("RANDOM_PAIR: forcing fountain off");
            fountain.setNewValue(0);
          }
            
          int max_random_pump = fountain.size() - 1;
          // Pick an off pump
          int pumpnum;
          do {
            pumpnum = random(0,max_random_pump);
            //Serial.print("pumpnum=");Serial.print(pumpnum); Serial.print(" value = ");Serial.println(fountain.pump[pumpnum].getRawValue());
          }
          while (fountain.pump[pumpnum].getRawValue() != 0);
          //Serial.print("RANDOM_PAIRS: off pumpnum = "); Serial.println(pumpnum);
          
          // Pick another off pump
          int pumpnum2;
          do {
            pumpnum2 = random(0,max_random_pump);
            //Serial.print("pumpnum2=");Serial.print(pumpnum2); Serial.print(" value = ");Serial.println(fountain.pump[pumpnum2].getRawValue());
          }
          while (pumpnum == pumpnum2 || (fountain.pump[pumpnum2].getRawValue() != 0 ));
          //Serial.print("RANDOM_PAIRS: off pumpnum2 = "); Serial.println(pumpnum2);
          internalPatternChange.setSeconds(2); 
          
          int maxvalue = 255;
          
          fountain.off();
          fountain.pump[pumpnum].setNow(maxvalue);
          D2("Turn on Pump#",pumpnum);
          fountain.pump[pumpnum2].setNow(maxvalue);
          D2("Turn on Pump#",pumpnum2);
        }
        break;

      case POPCORN:
        // rolling
        Dln("Pattern POPCORN");
        internalPatternChange.setSeconds(1); 
        popcorn_previous = popcorn_state;
        if (++popcorn_state > 9)  // Need to reach 'reset'
          popcorn_state = 0;

        D4("popcorn_state=",popcorn_state," popcorn_previous=",popcorn_previous);
        switch (popcorn_state) {
          case 0:
            fountain.off();
            // flow through
          case 1:
          case 2:
          case 3:
          case 4:
            fountain.pump[popcorn_state].setNow(255);
            if (popcorn_previous < 4)
              fountain.pump[popcorn_previous].setNow(0);
            break;
          case 5:
          case 6:
          case 7:
          case 8:
            // If repeating program, then skip 8
            if (popcorn_state != 8 || USE_RANDOM_PATTERNS || nextPattern != pattern){
              fountain.pump[8-popcorn_state].setNow(255);
              fountain.pump[8-popcorn_previous].setNow(0);
              break;
            }
            // else fall through 
          default:  // random repeat?
            fountain.off();
            if (!USE_RANDOM_PATTERNS||random(0,3) == 0)
              patternChange.expire();
            break;
        }
        break;
        
      case SPLASH:
        // rolling
        Dln("Pattern SPLASH");

        internalPatternChange.setSeconds(1); 
        Dln("Pattern 3 POPCORN");
        popcorn_previous = popcorn_state;
        if (++popcorn_state > 11)
          popcorn_state = 9;

        D4("popcorn_state=",popcorn_state," popcorn_previous=",popcorn_previous);
        switch (popcorn_state) {

          case 9:
            fountain.pump[0].setNow(0);
            fountain.pump[5].setNow(255);
            break;
            
          case 10:
            fountain.pump[2].setNow(255);
            fountain.pump[5].setNow(0);
            break;
          
          default:  // random repeat?
            fountain.off();
            if (random(0,3) == 0 || !USE_RANDOM_PATTERNS)
              patternChange.expire();
            break;
        }
        break;

      case DECAY:
        {
          Dln("Pattern DECAY");
          internalPatternChange.setSeconds(3);          
          int pumpon;
          do {
            pumpon = random(0,fountain.size());
          }
          // Possible no pumps are set to off, so only wait for internal pattern change.
          while (fountain.pump[pumpon].getNewValue() != 0 && !internalPatternChange.isNow());
          
          for (int p = 0; p < fountain.size(); ++p)
          {
            D("Pump "); D(p);
            D2(" value = ", fountain.pump[p].getValue());
            if (p == pumpon)
              fountain.pump[p].setNow(255);
            else
              fountain.pump[p].setNewValue(0);      
          }
          D2("Pump on: ",pumpon);
        }
        break;

      case SYMETRICAL: // Symetrical
        {
          Dln("Pattern SYMETRICAL");
          int totalOn = 0;
          for (int p = 5; p >= 0; --p)
          {
            switch (p)
            {
                // shift to front
                case 5:
                  fountain.pump[p].setNewValue(fountain.pump[p-1].getRawValue());
                  //D4("F",p,"=",fountain.pump[p].getRawValue());
                  //D2("F(currentvalue)=",fountain.pump[p].getValue());
                  break;
    
                case 4:
                case 3:
                  fountain.pump[p].setNow(fountain.pump[p-1].getRawValue());
                  //D4("R",p,"=",fountain.pump[p].getRawValue());
                  break;
    
                case 2:  // center;
                
                  int value;
                  do {
                    value = random(0,5) * 64 - 1;
                    if (value < 128)
                      value = 0;
                  }
                  while
                    (value == 0 && fountain.pump[1].getNewValue() == 0);
                  fountain.pump[p].setNewValue(value);
                  //D4("C",p,"=",fountain.pump[p].getRawValue());
                  break;
      
                // mirror
                case 1:
                case 0:
                  fountain.pump[p].setNow(fountain.pump[4 - p].getRawValue());
                  //D4("L",p,"=",fountain.pump[p].getRawValue());
                  break;
            }
  
            if (fountain.pump[p].getNewValue() != 0)
              ++totalOn;
          }
  
          // Make sure more than 1 pump is on;
          if (totalOn <=2) {
            Dln("SYMETRIC Forcing on extra pumps");
            fountain.pump[0].setNewValue(128);
            fountain.pump[4].setNewValue(128);
          }
      }
      break;

    case INSTANT_FULL_5:
     Dln("Pattern INSTANT_FULL_5");
      //fountain.setFadeSpeed(0);  // in Seconds.
      for (int p = 0; p <= 4; ++p)
        fountain.pump[p].setNow(255);
      fountain.pump[5].setNow(0);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
      
    case FULL_5:
      Dln("Pattern FULL_5");
      for (int p = 0; p <= 4; ++p)
        fountain.pump[p].setNewValue(255);
      fountain.pump[5].setNewValue(0);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;

    case INSTANT_FULL_6:
      Dln("Pattern INSTANT_FULL_6");
      //fountain.setFadeSpeed(0);  // in Seconds.
      for (int p = 0; p <= 5; ++p)
        fountain.pump[p].setNow(255);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
      
    case FULL_6:
      Dln("Pattern FULL_6");
      fountain.setNewValue(255);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;

    case INSTANT_SIX_ONLY:
      Dln("Pattern INSTANT_SIX_ONLY");    
      D2("patternChange.isActive() = ", patternChange.isActive());
      D2("patternChange.isPending() = ", patternChange.isPending());
      D2("eventtime = ", patternChange.eventtime);
      D2("millis() = ", millis());
      //fountain.setFadeSpeed(0);  // in Seconds.  screws up the setNow code????
      fountain.off();
      fountain.pump[5].setNow(255);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
      
    case SIX_ONLY:
      Dln("Pattern SIX_ONLY");
      fountain.setNewValue(0);
      fountain.pump[5].setNewValue(255);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
      
    case MOUNTAIN:
      Dln("Pattern MOUNTAIN");
      fountain.pump[0].setNewValue(128);
      fountain.pump[1].setNewValue(172);
      fountain.pump[2].setNewValue(255);
      fountain.pump[3].setNewValue(172);
      fountain.pump[4].setNewValue(128);
      fountain.pump[5].setNewValue(0);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
      
    case VALLEY:
      Dln("Pattern VALLEY");
      fountain.pump[0].setNewValue(255);
      fountain.pump[1].setNewValue(172);
      fountain.pump[2].setNewValue(128);
      fountain.pump[3].setNewValue(172);
      fountain.pump[4].setNewValue(255);
      fountain.pump[5].setNewValue(0);
      patternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.off();
      break;
            
    default:  // programmimg error?
      Dln("Pattern 'default', time to change patterns");
      patternChange.expire();
      break;
    }
  } 

   /* Watch Dog */
#ifdef TESTING3
   boolean oops = true;
   for (int pumpnum = 0; pumpnum < fountain.size(); ++pumpnum)
      if (fountain.pump[pumpnum].getNewValue() > 0) {
         oops = false;
         break;
      }

   if (oops) {
      D2("OOPS\npattern = ",pattern);
      internalPatternChange.setSeconds(internalPatternChangeSeconds);
      internalPatternChange.expire();
      patternChange.expire();
      fountain.pump[fountain.size()-1].setNow(255);
   }
#endif
      
  if (lightsOn)
    rgb.update();
  else
    rgb.off();
    
  fountain.update();
}


boolean pause(unsigned mills)
{
  return remote.pause(mills);
}

void changecolor() {
    D2("colorchange now colormode=",colormode);
    switch (colormode)
    {
      case 0:
        rgb.randomColor();
        break;
      
      case 1: // Halloween;



        if (random(0,4) == 0)
        {
          Dln("Lightning");
          do {
            rgb.setWhite(255);
            rgb.setNow();
            delay(random(250,500));
            rgb.off();
            delay(random(500,2000));
          } while (random(0,3) != 0);
          Dln("End Lightning");
        }
        switch (random(0,3))
        {
          case 0: // red
            rgb.setRed(random(200,255));      
            rgb.setGreen(0);
            rgb.setBlue(random(0, 50));
            break;
          case 1: //orange
            rgb.setRed(random(170,255));      
            rgb.setGreen(random(60,100));
            rgb.setBlue(0);
            break;
          case 2:  
            // Purple    
            rgb.setRed(random(200,255));      
            rgb.setGreen(0);
            rgb.setBlue(random(125,200));
            break;
          case 3:  
            // Yellowp Green    
            rgb.setRed(random(200,255));      
            rgb.setGreen(random(200,255));
            rgb.setBlue(0);
            break;
        }
        break;
        
      case 2:
        switch (random(0,3))
        {
          case 0: // red
            rgb.setRGB(255,0,0);
            break;
          case 1: //white
            rgb.setRGB(255,255,255);
            break;
          case 2:
            rgb.setRGB(40,40,255);
            break;
        }
        break;
    }
}


