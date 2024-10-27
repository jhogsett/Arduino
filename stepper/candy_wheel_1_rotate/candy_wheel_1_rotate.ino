#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
const double cent_step = 20.48;       // for your motor

// initialize the stepper library on pins2048 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(9);

  // perform a full revolutionon start up to allow synchronizing the clock position
  myStepper.step(stepsPerRevolution);
}

void loop() {
  myStepper.step(stepsPerRevolution);
  myStepper.step(-stepsPerRevolution);
}
