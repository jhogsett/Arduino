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
#define DELAY 15

// the setup function runs once when you press reset or power the board
void setup() {

  for(int i = MIN_PIN; i <= MAX_PIN; i++){
    pinMode(i, OUTPUT);  
    digitalWrite(i, HIGH);
  }
  
  // initialize digital pin 13 as an output.
  
}


//
//speed VAR Byte
//r VAR Word
//i VAR Byte
//
//speed = 119
//
//main:
//
//FOR i = 0 TO 63
//RANDOM r
//OUTL = i ^ (r.LOWBYTE & %11111101)
//PAUSE speed + (r.HIGHBYTE & %00000110)
//NEXT
//
//GOTO main

#define MIN_WAIT 100

int order[8] = { 1, 5, 3, 7, 4, 8, 2, 6 };

// the loop function runs over and over again forever
void loop() {

  for(int i = 0; i < 256; i++){

    int r = random(256);
    int pins = i ^ (r & 0b10111101);

    r = random(256);
    int wait = r & 0b00001110;

    for(int j = 0; j < NUM_PINS; j++){
      int level = (pins & 0x01) == 1 ? HIGH : LOW; 
      pins = pins >> 1;
      digitalWrite(MIN_PIN + j, level);
    }

    delay(MIN_WAIT + wait);
}

//
//  int i = random(NUM_PINS) + MIN_PIN;
//  int l = random(2) == 1 ? HIGH : LOW;   
//     
}
