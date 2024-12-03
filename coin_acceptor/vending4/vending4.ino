#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include <HT16K33Disp.h>

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

char buf[10];
char buf2[10];

Stepper myStepper(stepsPerRevolution, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

HT16K33Disp disp(0x70, 1);

void setup() {
  myStepper.setSpeed(MOTOR_SPEED);
  // myStepper.step(stepsPerRevolution);

  byte brightness[1] = {3};
  Wire.begin();
  disp.Init(brightness); 
  disp.clear();

  pinMode(coinIntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(coinIntPin), coinInserted, RISING);

  disp.scroll_string("    **** ENJOY CANDY **** ONLY 25 CENTS ****    ");
  disp.show_string("$ .25");
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

  float purse_f = deposited / 100.0;
  dtostrf(purse_f, 4, 2, buf2);
  sprintf(buf, "$%s", buf2);
  disp.show_string(buf);

#define CANDY_COST 25
  if(deposited >= CANDY_COST) {
    deposited -= CANDY_COST;
    
    wheel_segment = (wheel_segment++) % WHEEL_SEGMENTS;
    int leap_step = wheel_segment == 0 ? 1 : 0;
    myStepper.step(STEPS_PER_SEGMENT - leap_step);

    float purse_f = deposited / 100.0;
    dtostrf(purse_f, 5, 2, buf2);
    sprintf(buf, "Funds $%s", buf2);
    disp.scroll_string(buf);
    
    disp.clear();
    delay(1000);

  disp.scroll_string("    **** ENJOY CANDY **** ONLY 25 CENTS ****    ");
  disp.show_string("$ .25");
  }
}

