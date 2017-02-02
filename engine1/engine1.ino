/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

#define NUM_PINS 8
#define MIN_PIN 2
#define MAX_PIN (MIN_PIN + NUM_PINS - 1)
#define DELAY 27

// the setup function runs once when you press reset or power the board
void setup() {

  for(int i = MIN_PIN; i <= MAX_PIN; i++){
    pinMode(i, OUTPUT);  
    digitalWrite(i, HIGH);
  }
  
  // initialize digital pin 13 as an output.
  
}

int order[8] = { 1, 5, 3, 7, 4, 8, 2, 6 };

// the loop function runs over and over again forever
void loop() {

  for(int i = 0; i < NUM_PINS; i++){
    int pin = MIN_PIN + order[i] - 1;
    digitalWrite(pin, LOW);  
    delay(DELAY);
    digitalWrite(pin, HIGH);  
    delay(DELAY);
 }

//
//  int i = random(NUM_PINS) + MIN_PIN;
//  int l = random(2) == 1 ? HIGH : LOW;   
//     
}
