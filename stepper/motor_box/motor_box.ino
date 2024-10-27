#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
const double cent_step = 20.48;       // for your motor

// initialize the stepper library on pins2048 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(11);

  // perform a full revolutionon start up to allow synchronizing the clock position
  myStepper.step(-stepsPerRevolution);
}

// remember the carry between rounds
double carry = 0.0;

void loop() {
  double current = 0.0;
  int prev_current_i = 0;
  int current_i = 0;
    
  for(int i = 0; i < 100; i++){
    // increase to the next floating point step
    current += cent_step;

    // take the integer part as the initial next integer step
    current_i = int(current);
    
    // compute the carry, unused part of the floating point step
    carry += current - current_i;

    // add the integer part of the carry, if any to the integer step          
    int carry_i = int(carry);
    current_i += carry_i;

    // remove the used integer part of the carry
    carry -= carry_i;

    // compute the next steps needed from the previous and current integer step
    int steps = current_i - prev_current_i;
    prev_current_i = current_i;

    myStepper.step(-steps);
    
    // found after several rounds of timing multiple full minutes 
    // started losing accuracy afteraround 13 minutes
    // an accurate clock would need a timing source, 
    // I think there's a built-in way to do this with interupts 
    delay(548);
  }
}
