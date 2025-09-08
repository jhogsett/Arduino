#include <random_seed.h>

#include <Stepper.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define RANDOM_SEED_PIN A7
static RandomSeed<RANDOM_SEED_PIN> randomizer;

const int stepsPerRevolution = 2048;
const double cent_step = 2048 / 60; 

Stepper myStepper(stepsPerRevolution, 14, 15, 16, 17);

#define SENSOR_PIN 2
#define LED_PIN 3
#define MOTOR_SPEED 9
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

void calibrate_clock_hand(int adjustment){
  carry = 0.0;
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
  while(abs(last_pos - (position = random(24))) > 24);

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

  // // The first NeoPixel in a strand is #0, second is 1, all the way up
  // // to the count of pixels minus one.
  // for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

  //   // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  //   // Here we're using a moderately bright green color:
  //   pixels.setPixelColor(i, pixels.Color(0, 150, 0));


  //   delay(DELAYVAL); // Pause before next pass through loop
  // }






  // unsigned long time;
  // while((time = millis()) < next_time)
  //   ;

  // second = ++second % 60;
  // float hand_position = second * (stepsPerRevolution / 60.0);

  // move_hand(hand_position);
  // next_time = time + PERIOD;

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

