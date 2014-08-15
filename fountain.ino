#include "Arduino.h"
#define debug true
#include "debug.h"
#include "remote.h"
#include "led.h"
#include "pump.h"

#define REMOTE_PIN 22
#define BLUE_PIN   9
#define RED_PIN   10
#define GREEN_PIN 11

#define NUM_PUMPS 6
#define FIRST_PUMP_PIN  3
#define LAST_PUMP_PIN   (FIRST_PUM_PIN + NUM_PUMPS - 1)

Remote remote;
RGBLed rgb;
Pump   pump[NUM_PUMPS];

unsigned long change;

void setup() {
  remote.init(REMOTE_PIN);
  rgb.init(BLUE_PIN, RED_PIN, GREEN_PIN);
//  for (int pp = 0; pp < NUM_PUMPS; ++pp)
//  {
//    pump[pp].init(FIRST_PUMP_PIN + pp);
//  }
  change = millis();
//  randomSeed(analogRead(0));
  // USB can be used for Debug output.
  Serial.begin(9600);
}


boolean test = true;
void loop() {
  
  if (test) {
    rgb.test();
    test = false;
  }
    
//  if (remote.pressed())
//     Serial.print("Pressed\n");
  
  D2((delay(500), "millis()="),millis());
  D2("change=",change);
  if (millis() > change)
  {
    D2("Change time",(delay(5000), ""));
    
    int r;
    change = millis() + 10000;
    int speed;
    rgb.setFadeSpeed(speed = random(4,10)* 1000);
    D2("Speed=",speed);
    rgb.randomColor();
    Dln();
  } 
    
  rgb.update();
}
