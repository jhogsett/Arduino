#include <random_seed.h>

#include <Stepper.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <Wire.h>
#include <DS3231-RTC.h>

DS3231 myRTC;

#define RANDOM_SEED_PIN A7
static RandomSeed<RANDOM_SEED_PIN> randomizer;

const int stepsPerRevolution = 2048;
const double cent_step = 2048 / 60; 

Stepper myStepper(stepsPerRevolution, 14, 15, 16, 17);

#define SENSOR_PIN 2
#define LED_PIN 3
#define MOTOR_SPEED 9
#define SLOW_DELAY 5
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

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int cal_steps1 = 0;
int cal_steps2 = 0;
int centering = 9;


// fractional amount not stepped forward
float carry = 0.0;

void(* resetFunc)(void) = 0;

float hand_position = 0.0;

// void calibrate_clock_hand(int adjustment){
//   carry = 0.0;
//   hand_position = 0.0;
//   cal_steps = 0;

//   // if already on the sensor, roll forward until it's not found
//   if(digitalRead(SENSOR_PIN) == FOUND){
//     for(int i = 0; i < stepsPerRevolution; i++){
//       if(digitalRead(SENSOR_PIN) == NOTFOUND)
//         break;
//       myStepper.step(FORE);
//     }
//   }

//   // roll forward untli the sensor is found
//   for(int i = 0; i < stepsPerRevolution; i++){
//     if(digitalRead(SENSOR_PIN) == FOUND)
//       break;
//     myStepper.step(FORE);
//   }

//   // roll slowly until the sensor is not found and count the steps
//   for(int i = 0; i < 2 * stepsPerRevolution; i++){
//     if(digitalRead(SENSOR_PIN) == NOTFOUND)
//       break;
//     myStepper.step(FORE);
//     cal_steps++;
//     delay(SLOW_DELAY);
//   }

//   // roll back slowly half the number of counted steps
//   // plus the device-specific centering fudge amount
//   for(int i = 0; i < (cal_steps / 2) + adjustment; i++){
//     myStepper.step(BACK);
//     delay(SLOW_DELAY);
//   }  

//   Serial.println("---Calibration Steps");
//   Serial.println(cal_steps);
// }

void calibrate_clock_hand(int adjustment){
  carry = 0.0;
  hand_position = 0.0;
  cal_steps1 = 0;
  cal_steps2 = 0;

  // if already on the sensor, roll forward until it's not found
  if(digitalRead(SENSOR_PIN) == FOUND){
    for(int i = 0; i < stepsPerRevolution; i++){
      if(digitalRead(SENSOR_PIN) == NOTFOUND)
        break;
      myStepper.step(FORE);
    }
  }

  // roll forward until the sensor is found
  for(int i = 0; i < stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == FOUND)
      break;
    myStepper.step(FORE);
    // delay(SLOW_DELAY);
  }

  // roll forward slowly until the sensor is not found and count the steps
  for(int i = 0; i < 2 * stepsPerRevolution; i++){
    if(digitalRead(SENSOR_PIN) == NOTFOUND)
      break;
    myStepper.step(FORE);
    cal_steps1++;
    delay(SLOW_DELAY);
  }

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

  // roll forward slowly the average of the counted steps
  for(int i = 0; i < cal_steps; i++){
    myStepper.step(FORE);
    delay(SLOW_DELAY);
  }  

  // roll back slowly half the number of counted steps
  // plus the device-specific centering fudge amount
  for(int i = 0; i < (cal_steps / 2) + adjustment; i++){
    myStepper.step(BACK);
    delay(SLOW_DELAY);
  }  

}

#define fabs(x) ((x)>0.0?(x):-(x))

void move_hand(float new_position){
  // Serial.println("-------");
  // Serial.println(hand_position);
  // Serial.println(new_position);

  float dif;
  if(new_position > hand_position){

    // could be a real less or a fake more due to the wrap around

    dif = new_position - hand_position;
    float fdif = abs((new_position - stepsPerRevolution) - hand_position);

    // Serial.println("%%%%%");
    // Serial.println(dif);
    // Serial.println(fdif);
    // Serial.println("%%%%%");

    if(fdif > 0.0 && fdif < dif){
      // Serial.println("*****");
      // Serial.println(fdif);
      // Serial.println("*****");

      // move back past the origin
      hand_position -= int(fdif);
      carry -= fdif - int(fdif);

      myStepper.step(-(fdif + int(carry)));
      carry -= int(carry);

    } else {
      if(dif <= stepsPerRevolution / 2){
        hand_position += int(dif);
        carry += dif - int(dif);

        myStepper.step(dif + int(carry));
        carry -= int(carry);

      } else {
        hand_position -= int(dif);
        carry -= dif - int(dif);

        myStepper.step(-(dif + int(carry)));
        carry -= int(carry);
      }
    }
  } 
  else if(new_position < hand_position){

    // could be a real less or a fake less due to the wrap around

    dif = hand_position - new_position;
    float fdif = (new_position + stepsPerRevolution) - hand_position;

    if(fdif < dif){
      // Serial.println("*****");
      // Serial.println(fdif);
      // Serial.println("*****");
      // move ahead past the origin
      hand_position += int(fdif);
      carry += fdif - int(fdif);

      myStepper.step(fdif  + int(carry));
      carry -= int(carry);
    } else {

      // dif = hand_position - new_position;

      if(dif <= stepsPerRevolution / 2){
        hand_position -= int(dif);
        carry -= dif - int(dif);
        myStepper.step(-(dif + int(carry)));
        carry -= int(carry);
      } else {
        hand_position += int(dif);
        carry += dif - int(dif);

        myStepper.step(dif + int(carry));
        carry -= int(carry);
      }
    }
  }

  // Serial.println(hand_position);
  // Serial.println(carry);

// 2013.87
// 0.00

  if(hand_position > stepsPerRevolution){
    hand_position -= stepsPerRevolution;
  } else if(hand_position < 0.0){
    hand_position += stepsPerRevolution;
  }

  // hand_position = new_position;

}

void home_hand(){
  move_hand(-carry);
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

  Wire.begin();

  randomizer.randomize();

  myStepper.setSpeed(MOTOR_SPEED);

  // perform a full revolutionon start up to allow synchronizing the clock position
  // myStepper.step(stepsPerRevolution);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(63);
  pixels.clear();
  pixels.show();

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

#define CHECK_AFTER 20

bool century = false;
bool h12Flag;
bool pm;
int last_second = -1;
int last_minute = -1;
long first_hue = 0;

#define MAX_HUE (5*65536)
#define HUE_STEP 1024

#define RTC_CHECK_DELAY 50

void loop() {

  // do something only once per second
  int second = myRTC.getSecond();
  if(second == last_second){
    delay(RTC_CHECK_DELAY);
    return;
  }
  last_second = second;

  int year = myRTC.getYear();
  int month = myRTC.getMonth(century);
  int day = myRTC.getDate();
  int hour = myRTC.getHour(h12Flag, pm);
  int minute = myRTC.getMinute();

  // advance the hue
  // pixels.rainbow(first_hue, 1, 255, 8, false);
  pixels.fill(Adafruit_NeoPixel::ColorHSV(first_hue, 255, 4), 0, 24);
  pixels.fill(Adafruit_NeoPixel::ColorHSV(first_hue + 32768L, 255, 127), 24, 12);
  first_hue += HUE_STEP;
  first_hue %= MAX_HUE;

  int hour12 = hour % 12 + 1;

  for(int i = 0; i < 12; i++){
    if(i < hour12)
      pixels.setPixelColor(i * 2, pixels.Color(8, 64, 8));
    // else
    //   pixels.setPixelColor(i * 2, pixels.Color(0, 0, 0));
  }

  pixels.show();   // Send the updated pixel colors to the hardware.

  // move hand once per minute due to accumulated errors moving each second
  if(minute != last_minute){
    float hand_position = minute * (stepsPerRevolution / (60.0));
    move_hand(hand_position);
  }
  last_minute = minute;

  // int total_seconds = minute * 60 + second;
  // float hand_position = total_seconds * (stepsPerRevolution / (3600.0));
  // move_hand(hand_position);
}

// notes
// calibrate at the top of the hour?
