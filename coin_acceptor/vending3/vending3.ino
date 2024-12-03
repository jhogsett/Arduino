#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include <HT16K33Disp.h>
#include "billboard.h"
#include "price_handler.h"
#include "billboards_handler.h"
#include <random_seed.h>

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

HT16K33Disp disp(0x70, 1);

// #define PRICE_BUFFER 7
#define MESSAGE_BUFFER 40

#define BILLBOARD_BUFFER 60
#define PRICE_BUFFER 10
#define NUM_BILLBOARDS 5

#define PRICE_DOWN_TIME 300000
#define PRICE_DOWN_STEP 5
#define PRICE_MIN 25
#define PRICE_MAX 995

#define BLANKING_TIME 1000
#define HOME_TIMES 60

char price_buffer[PRICE_BUFFER];
char message_buffer[MESSAGE_BUFFER];

char billboard_buffer[BILLBOARD_BUFFER];
char current_price[PRICE_BUFFER];

const char template0[] PROGMEM = "%s"; 
const char template1[] PROGMEM = "    SWEETEN YOUR DAY only %s    "; 
const char template2[] PROGMEM = "    INDULGE YOUR CRAVINGS just %s    "; 
const char template3[] PROGMEM = "    BITE-SIZE BLISS just %s    "; 
const char template4[] PROGMEM = "    SWEET DREAMS START HERE only %s    "; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4};

PriceHandler price_handler(current_price, PRICE_DOWN_TIME, PRICE_DOWN_STEP, PRICE_MIN);
BillboardsHandler billboards_handler(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);

// https://arduino.stackexchange.com/questions/60764/coin-recognition-using-coin-accepter-in-arduino-uno
// without a 100nF cap between the coin pin and ground, the pulses can be noisy

#define MOTOR_SPEED 9
#define MOTOR_PIN1 4
#define MOTOR_PIN2 5
#define MOTOR_PIN3 6
#define MOTOR_PIN4 7

#define COIN_GOLD 2
#define COIN_NICKEL 3
#define COIN_DIME 4
#define COIN_QUARTER 5
#define COIN_SPECIAL 6

const int coinIntPin = 2;          //interruptPin 0 is digital pin 2
volatile boolean newCoin = false;                  
volatile int pulseCount;          //counts pulse for 
volatile long timeFPulse = 0;     // to determine when First impulse was interrupted
int coinType;
long interval;           // to determine interval from first and last impulse
const int max_interval = 500;
int credits = 0;
int deposited = 0;

const int stepsPerRevolution = 2048;

Stepper myStepper(stepsPerRevolution, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

void setup() {
  myStepper.setSpeed(MOTOR_SPEED);
  // myStepper.step(stepsPerRevolution);

  randomizer.randomize();

  byte brightness[1] = { 3 };
  Wire.begin();
  disp.Init(brightness);
  disp.clear();

  price_handler.begin(PRICE_MIN);
  price_handler.refresh_price();      
  billboards_handler.update_buffer(current_price);

  pinMode(coinIntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(coinIntPin), coinInserted, RISING);
}

void loop()
{
  while (newCoin == true)
  {
    interval = millis() - timeFPulse;
    if(interval > max_interval) 
    {                                                                  
      coinType = pulseCount;
      sendData(coinType);
      pulseCount = 0;
      newCoin = false;
      }  
  }

  unsigned long time = millis();

  if(price_handler.step()){
    price_handler.refresh_price();      
    billboards_handler.update_buffer(current_price);
  }

  billboards_handler.run(time, &disp, current_price);  
}



void coinInserted(){
    newCoin = true; 
    timeFPulse = millis();
    pulseCount++; 
}

void(* resetFunc) (void) = 0;

#define WHEEL_SEGMENTS 3
#define STEPS_PER_SEGMENT 693
int wheel_segment = 0;

void sendData(int coinType){
  if(coinType == COIN_SPECIAL){
    resetFunc();
  }

  if(coinType == COIN_NICKEL) deposited += 5;
  if(coinType == COIN_DIME) deposited += 10;
  if(coinType == COIN_QUARTER) deposited += 25;
  if(coinType == COIN_GOLD) deposited += 25;

  billboards_handler.reset();

  int current_price = price_handler.price(); 
  if(deposited >= current_price) {
    deposited -= current_price;
    
    price_handler.boost_price(2, PRICE_MAX);
    price_handler.refresh_price();      
    // billboards_handler.update_buffer(current_price);

    wheel_segment = (wheel_segment++) % WHEEL_SEGMENTS;
    int leap_step = wheel_segment == 0 ? 1 : 0;
    myStepper.step(STEPS_PER_SEGMENT - leap_step);
  }

  if(deposited == 0){
    disp.scroll_string("    THANK YOU    ");
    billboards_handler.update_buffer(current_price);
  } else {
    price_handler.format_price(deposited, price_buffer);
    sprintf(message_buffer, "%s", price_buffer);
    disp.scroll_string(message_buffer);
    billboards_handler.update_buffer(current_price);
  }
}
