#include <EEPROM.h>

#define MODE_SLOW   0
#define MODE_MEDIUM 1
#define MODE_FAST   2
#define MODE_FIRST 0
#define MODE_LAST 2

#define SLOW_DELAY 1000
#define MEDIUM_DELAY 250
#define FAST_DELAY 50

int mode = MODE_FIRST;
int delay_ = SLOW_DELAY;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(0, OUTPUT);

  mode = EEPROM.read(0); //place this inside a function
  mode++;

  if(mode < MODE_FIRST || mode > MODE_LAST){
    mode = MODE_FIRST;
  }
  EEPROM.write(0, mode);

  switch(mode){
    case MODE_SLOW:
      delay_ = SLOW_DELAY;
      break;
    case MODE_MEDIUM:
      delay_ = MEDIUM_DELAY;
      break;
    case MODE_FAST:
      delay_ = FAST_DELAY;
      break;
  }
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(0, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(delay_);                      // wait for a second
  digitalWrite(0, LOW);   // turn the LED off by making the voltage LOW
  delay(delay_);                      // wait for a second
}
