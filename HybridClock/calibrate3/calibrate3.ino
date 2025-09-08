#include <Stepper.h>

const int stepsPerRevolution = 2048;
const double cent_step = 2048 / 60; 

Stepper myStepper(stepsPerRevolution, 14, 15, 16, 17);

#define SENSOR_PIN 2
#define LED_PIN 3
#define MOTOR_SPEED 11
#define SLOW_DELAY 5
#define FORE 1
#define BACK -1
#define RESTART_WAIT 3000L
#define RESET_COUNT 5
#define FOUND LOW
#define NOTFOUND HIGH
#define PERIOD 300

int centering = 9;


// remember the carry between rounds
double carry = 0.0;

void(* resetFunc)(void) = 0;

float hand_position = 0.0;

void calibrate_clock_hand(int adjustment){
  // if already on the sensor, roll forward until it's not found
  if(digitalRead(SENSOR_PIN) == FOUND){
    for(int i = 0; i < stepsPerRevolution; i++){
      if(digitalRead(SENSOR_PIN) == NOTFOUND)
        break;
      myStepper.step(FORE);
    }
  }

  // roll forward untli the sensor it found
  for(int i = 0; i < stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == FOUND)
      break;
    myStepper.step(FORE);
  }

  // roll slowly until the sensor is not found and count the steps
  int cal_step = 0;
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(FORE);
    cal_step++;
    delay(SLOW_DELAY);
  }

  // roll back slowly half the number of counted steps
  // plus the device-specific centering fudge amount
  for(int i = 0; i < (cal_step / 2) + adjustment; i++){
    myStepper.step(BACK);
    delay(SLOW_DELAY);
  }  
}

#define fabs(x) ((x)>0.0?(x):-(x))

void move_hand(float new_position){
  Serial.println("-------");
  Serial.println(hand_position);
  Serial.println(new_position);

  float dif;
  if(new_position > hand_position){
    dif = new_position - hand_position;

    if(dif <= stepsPerRevolution / 2){
      myStepper.step(dif);
      // hand_position += dif;

    } else {
      myStepper.step(-dif);
      // hand_position -= dif;
    }
  } 
  else if(new_position < hand_position){

    // could be a real less or a fake less due to the wrap around

    dif = hand_position - new_position;
    float fdif = (new_position + stepsPerRevolution) - hand_position;

    if(fdif < dif){
      Serial.println("*****");
      Serial.println(fdif);
      // move ahead past the origin
      myStepper.step(fdif);
    } else {

      // dif = hand_position - new_position;

      if(dif <= stepsPerRevolution / 2){
        myStepper.step(-dif);
        // hand_position -= dif;

      } else {
        myStepper.step(dif);
        // hand_position += dif;
      }
    }
  }

  Serial.println(dif);
  Serial.println(hand_position);

// 2013.87
// 0.00

  // if(hand_position > stepsPerRevolution){
  //   hand_position -= stepsPerRevolution;
  // }

  hand_position = new_position;

}

void step_hand(int steps){

}


int second = -1;
unsigned long next_time = 0L;

void setup() {
  Serial.begin(115200);
  myStepper.setSpeed(MOTOR_SPEED);

  // perform a full revolutionon start up to allow synchronizing the clock position
  // myStepper.step(stepsPerRevolution);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.println();

  // positive 100 steps
  // myStepper.step(500 * FORE);
  // negative 100 steps
  // myStepper.step(500 * BACK);

  // myStepper.step(FORE * random(stepsPerRevolution * 2) - stepsPerRevolution);
  calibrate_clock_hand(centering);
  // delay(500);

  next_time = millis() + PERIOD;
}


void loop() {
  unsigned long time;
  while((time = millis()) < next_time)
    ;

  second = ++second % 60;
  float hand_position = second * (stepsPerRevolution / 60.0);

  move_hand(hand_position);
  next_time = time + PERIOD;

  // for(int i = 0; i < 60; i++){
    // increase to the next floating point step
    // current += cent_step;

    // // take the integer part as the initial next integer step
    // current_i = int(current);
    
    // // compute the carry, unused part of the floating point step
    // carry += current - current_i;

    // // add the integer part of the carry, if any to the integer step          
    // int carry_i = int(carry);
    // current_i += carry_i;

    // // remove the used integer part of the carry
    // carry -= carry_i;

    // // compute the next steps needed from the previous and current integer step
    // int steps = current_i - prev_current_i;
    // prev_current_i = current_i;

    // myStepper.step(+steps);
    
    // // found after several rounds of timing multiple full minutes 
    // // started losing accuracy afteraround 13 minutes
    // // an accurate clock would need a timing source, 
    // // I think there's a built-in way to do this with interupts 
    // delay(913);
  // }
}

