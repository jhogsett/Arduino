#include <random_seed.h>

#include <Stepper.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// #define BLACK_DEVICE 
#define WHITE_DEVICE 

#define RANDOM_SEED_PIN A7
static RandomSeed<RANDOM_SEED_PIN> randomizer;

#define SENSOR_PIN 2
#define LED_PIN 3
#define FIRST_MOTOR_PIN 14
#define MOTOR_SPEED 13
#define SLOW_DELAY 0
#define FORE 1
#define BACK -1
#define RESTART_WAIT 3000L
#define RESET_COUNT 5
#define FOUND LOW
#define NOTFOUND HIGH
#define PERIOD 300

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 36 // Popular NeoPixel ring size

const int stepsPerRevolution = 2048;
const double cent_step = 2048 / 60; 

Stepper myStepper(stepsPerRevolution, 
                  FIRST_MOTOR_PIN, 
                  FIRST_MOTOR_PIN+1, 
                  FIRST_MOTOR_PIN+2, 
                  FIRST_MOTOR_PIN+3);


// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int cal_steps1 = 0;
int cal_steps2 = 0;
#if defined(BLACK_DEVICE)
int centering = 9;
#elif defined(WHITE_DEVICE)
int centering = -2;
#endif

void(* resetFunc)(void) = 0;

float hand_position = 0.0;

void calibrate_clock_hand(int adjustment){
  hand_position = 0.0;
  cal_steps1 = 0;
  cal_steps2 = 0;

  pixels.fill(pixels.Color(64, 0, 0));
  pixels.show();

  // if already on the sensor, roll forward until it's not found
  if(digitalRead(SENSOR_PIN) == FOUND){
    for(int i = 0; i < stepsPerRevolution; i++){
      if(digitalRead(SENSOR_PIN) == NOTFOUND)
        break;
      myStepper.step(FORE);
    }
  }

  pixels.fill(pixels.Color(64, 32, 0));
  pixels.show();
  // delay(1000);

  // roll forward until the sensor is found
  for(int i = 0; i < stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == FOUND)
      break;
    myStepper.step(FORE);
    // delay(SLOW_DELAY);
  }

  pixels.fill(pixels.Color(0, 64, 0));
  pixels.show();
  // delay(1000);

  // roll forward slowly until the sensor is not found and count the steps
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(FORE);
    cal_steps1++;
    delay(SLOW_DELAY);
  }

  pixels.fill(pixels.Color(0, 32, 32));
  pixels.show();
  // delay(1000);

  Serial.println("------");
  Serial.print("Fwd Steps: ");
  Serial.println(cal_steps1);

  // roll back until the sensor is found
  for(int i = 0; i < stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == FOUND)
      break;
    myStepper.step(BACK);
    delay(SLOW_DELAY);
  }

  pixels.fill(pixels.Color(0, 0, 64));
  pixels.show();
  // delay(1000);

  // roll back slowly until the sensor is not found and count the steps
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(BACK);
    cal_steps2++;
    delay(SLOW_DELAY);
  }

  Serial.print("Bak Steps: ");
  Serial.println(cal_steps2);

  int cal_steps = (cal_steps1 + cal_steps2) / 2;

  pixels.fill(pixels.Color(32, 0, 64));
  pixels.show();
  // delay(1000);

  // roll forward slowly the average of the counted steps
  for(int i = 0; i < cal_steps; i++){
    myStepper.step(FORE);
    delay(SLOW_DELAY);
  }  

  pixels.fill(pixels.Color(32, 0, 32));
  pixels.show();

  // roll back slowly half the number of counted steps
  // plus the device-specific centering fudge amount
  for(int i = 0; i < (cal_steps / 2) + adjustment; i++){
    myStepper.step(BACK);
    delay(SLOW_DELAY);
  }  

}

#define fabs(x) ((x)>0.0?(x):-(x))

bool motor_pins[4] = {LOW, LOW, LOW, LOW};
#define MOTOR_SETTLE_TIME 100

void pause_motor(){
  for(int i = 0; i < 4; i++){
    motor_pins[i] = digitalRead(FIRST_MOTOR_PIN + i);
    Serial.println(motor_pins[i]);
    digitalWrite(FIRST_MOTOR_PIN + i, LOW);
  }
}

void resume_motor(){
  for(int i = 0; i < 4; i++){
    digitalWrite(FIRST_MOTOR_PIN + i, motor_pins[i]);
  }
  delay(MOTOR_SETTLE_TIME);
}

void move_hand(float new_position){
  resume_motor();

  float dif;
  if(new_position > hand_position){
    // could be a real less or a fake more due to the wrap around
    dif = new_position - hand_position;
    float fdif = abs((new_position - stepsPerRevolution) - hand_position);

    if(fdif > 0.0 && fdif < dif){
      // move back past the origin
      hand_position -= int(fdif);
      myStepper.step(-fdif);
    } else {
      if(dif <= stepsPerRevolution / 2){
        hand_position += int(dif);
        myStepper.step(dif);
      } else {
        hand_position -= int(dif);
        myStepper.step(-dif);
      }
    }
  } 
  else if(new_position < hand_position){
    // could be a real less or a fake more due to the wrap around
    dif = hand_position - new_position;
    float fdif = (new_position + stepsPerRevolution) - hand_position;

    if(fdif < dif){
      hand_position += int(fdif);
      myStepper.step(fdif);
    } else {
      if(dif <= stepsPerRevolution / 2){
        hand_position -= int(dif);
        myStepper.step(-dif);
      } else {
        hand_position += int(dif);
        myStepper.step(dif);
      }
    }
  }

  if(hand_position > stepsPerRevolution){
    hand_position -= stepsPerRevolution;
  } else if(hand_position < 0.0){
    hand_position += stepsPerRevolution;
  }

  pause_motor();
}

void home_hand(){
  move_hand(0.0);
}

// call when supposedly at home position
void check_hand_calibration(){
  if(digitalRead(SENSOR_PIN) == NOTFOUND){
    Serial.println("Sensor not home");
    return;
  }

  // roll back slowly until the sensor is not found
  int rollback_steps = 0;
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(BACK);
    rollback_steps++;
    delay(SLOW_DELAY);
  }

  // roll forward slowly until the sensor is found
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == FOUND)
      break;
    myStepper.step(FORE);
    delay(SLOW_DELAY);
  }

  // roll forward slowly until the sensor is not found
  int rollfore_steps = 0;
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(FORE);
    rollfore_steps++;
    delay(SLOW_DELAY);
  }

  Serial.println("Roll back From Home");
  Serial.println(rollback_steps);

  Serial.println("Found Steps");
  Serial.println(rollfore_steps);

}


int second = -1;
unsigned long next_time = 0L;

void setup() {
  Serial.begin(115200);

  randomizer.randomize();

  myStepper.setSpeed(MOTOR_SPEED);

  // perform a full revolutionon start up to allow synchronizing the clock position
  // myStepper.step(stepsPerRevolution);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Serial.println();

  // positive 100 steps
  // myStepper.step(500 * FORE);
  // negative 100 steps
  // myStepper.step(500 * BACK);

  // myStepper.step(FORE * random(stepsPerRevolution * 2) - stepsPerRevolution);
  calibrate_clock_hand(centering);
  delay(5000);

  next_time = millis() + PERIOD;
}

#define CHECK_AFTER 20

int last_pos = 0;
int check_count = 0;
void loop() {

  pixels.clear(); // Set all pixel colors to 'off'

  int position;
  while(abs(last_pos - (position = random(24))) > 6);

  pixels.setPixelColor(position, pixels.Color(16, 0, 16));
  pixels.show();   // Send the updated pixel colors to the hardware.

  float hand_position = position * (stepsPerRevolution / 24.0);

  // unsigned long time;
  // while((time = millis()) < next_time)
  //   ;

  // second = ++second % 60;
  // float hand_position = second * (stepsPerRevolution / 60.0);

  move_hand(hand_position);
  // next_time = time + PERIOD;

  delay(500);

  if(++check_count > CHECK_AFTER){
    check_count = 0;

    pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(32, 0, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.

    home_hand();
    delay(500);

    pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(16, 16, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.

    check_hand_calibration();
    delay(500);

    pixels.clear(); // Set all pixel colors to 'off'
    pixels.setPixelColor(0, pixels.Color(0, 0, 32));
    pixels.show();   // Send the updated pixel colors to the hardware.

    calibrate_clock_hand(centering);

    delay(500);
  }
}

