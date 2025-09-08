#include <Stepper.h>

const int stepsPerRevolution = 2048;
const double cent_step = 20.48; 

Stepper myStepper(stepsPerRevolution, 14, 15, 16, 17);

#define SENSOR_PIN 2
#define LED_PIN 3
#define MOTOR_SPEED 11
#define SLOW_DELAY 5
#define FORE 1
#define BACK -1
#define RESTART_WAIT 3000L
#define RESET_COUNT 5

int centering = 9;

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

  myStepper.step(FORE * random(stepsPerRevolution * 2) - stepsPerRevolution);
  delay(1000);
}

// remember the carry between rounds
double carry = 0.0;

void(* resetFunc)(void) = 0;

int reset_count = 0;

void loop() {

  digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));

  // if not already on the sensor, roll forward until it's found
  if(digitalRead(SENSOR_PIN) == HIGH){
    // Serial.println("NOT FOUND");
    for(int i = 0; i < stepsPerRevolution; i++){
      // Serial.print("!");
      digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));
      if(digitalRead(SENSOR_PIN) == LOW)
        break;
      // delay(SLOW_DELAY);
      myStepper.step(FORE);
    }
  } else {
    // Serial.println("FOUND");
    // roll backward until it's not found
    for(int i = 0; i < stepsPerRevolution; i++){
      // Serial.print("@");
      digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));
      if(digitalRead(SENSOR_PIN) == HIGH)
        break;
      myStepper.step(BACK);
      delay(SLOW_DELAY);
    }

  // roll forward until it's found
    for(int i = 0; i < stepsPerRevolution; i++){
      // Serial.print("#");
      digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));
      if(digitalRead(SENSOR_PIN) == LOW)
        break;
      myStepper.step(FORE);
      delay(SLOW_DELAY);
    }
  }

  int cal_step = 0;
  // roll slowly until the sensor goes high NOT FOUND and count the steps
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    // Serial.print("*");
    digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));
    if(digitalRead(SENSOR_PIN) == HIGH)
      break;
    myStepper.step(FORE);
    cal_step++;
    delay(SLOW_DELAY);
  }

  // digitalWrite(LED_PIN, LOW);

  // roll back slowly half the number of counted steps
  for(int i = 0; i < (cal_step / 2) + centering; i++){
    // Serial.print("$");
    digitalWrite(LED_PIN, !digitalRead(SENSOR_PIN));
    myStepper.step(BACK);
    delay(SLOW_DELAY);
  }  

  digitalWrite(LED_PIN, HIGH);

  unsigned long now = millis();
  while(millis() < now + RESTART_WAIT)
    ;

  if(reset_count++ >= RESET_COUNT){
    resetFunc();
  }

  // myStepper.step(FORE * random(stepsPerRevolution * 2) - stepsPerRevolution);
// resetFunc(); // Call the function at address 0 to initiate reset

  // digitalWrite(3, !digitalRead(2));

  // double current = 0.0;
  // int prev_current_i = 0;
  // int current_i = 0;
    
  // for(int i = 0; i < 100; i++){
  //   // increase to the next floating point step
  //   current += cent_step;

  //   // take the integer part as the initial next integer step
  //   current_i = int(current);
    
  //   // compute the carry, unused part of the floating point step
  //   carry += current - current_i;

  //   // add the integer part of the carry, if any to the integer step          
  //   int carry_i = int(carry);
  //   current_i += carry_i;

  //   // remove the used integer part of the carry
  //   carry -= carry_i;

  //   // compute the next steps needed from the previous and current integer step
  //   int steps = current_i - prev_current_i;
  //   prev_current_i = current_i;

  //   myStepper.step(-steps);
    
  //   // found after several rounds of timing multiple full minutes 
  //   // started losing accuracy afteraround 13 minutes
  //   // an accurate clock would need a timing source, 
  //   // I think there's a built-in way to do this with interupts 
  //   delay(548);
  // }
}





// void setup() {
//   pinMode(2, INPUT_PULLUP);
//   pinMode(3, OUTPUT);
// }

// void loop() {
//   digitalWrite(3, !digitalRead(2));
// }

