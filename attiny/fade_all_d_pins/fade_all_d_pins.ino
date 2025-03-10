/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

#include <random_seed.h>

#define RANDOM_SEED_PIN A1

#define PIN_FIRST 0
#define PIN_LAST 4

// for generating higher-quality random number seeds
static RandomSeed<RANDOM_SEED_PIN> randomizer;

// the setup function runs once when you press reset or power the board
void setup() {
  randomizer.randomize();

  // initialize digital pin LED_BUILTIN as an output.
  for(int j = PIN_FIRST; j <= PIN_LAST; j++)
    pinMode(j, OUTPUT);
}

#define DEL 15

#define RED_PIN 0
#define GREEN_PIN 1
#define BLUE_PIN 4

#define RED_INT 100
#define GREEN_INT 20
#define BLUE_INT 50

#define MAX_SPEED 5

// the loop function runs over and over again forever
void loop() {
  int a1 = random(255);
  int a2 = random(255);
  int a3 = random(255);
  int d1 = random(1, MAX_SPEED);
  int d2 = random(1, MAX_SPEED);
  int d3 = random(1, MAX_SPEED);

  while(true){
    delay(DEL);
    
    analogWrite(RED_PIN, (a1 * RED_INT) / 100);
    analogWrite(GREEN_PIN, (a2 * GREEN_INT) / 100);
    analogWrite(BLUE_PIN, (a3 * BLUE_INT) / 100);
    
    if((a1 + d1 > 255) || (a1 + d1 < 0)){
      d1 *= -1;
    }
    a1 += d1;
    if((a2 + d2 > 255) || (a2 + d2 < 0)){
      d2 *= -1;
    }
    a2 += d2;
    if((a3 + d3 > 255) || (a3 + d3 < 0)){
      d3 *= -1;
    }
    a3 += d3;
  }
}
