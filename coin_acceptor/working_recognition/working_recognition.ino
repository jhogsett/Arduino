// https://arduino.stackexchange.com/questions/60764/coin-recognition-using-coin-accepter-in-arduino-uno

// pennies and dimes get confused (also they are harder because they're easier to give too much of a shove)
// without a 100nF cap between the coin pin and ground, the pulses can be noisy

#include  <Arduino.h>

const int coinIntPin = 2;          //interruptPin 0 is digital pin 2
volatile boolean newCoin = false;                  
volatile int pulseCount;          //counts pulse for 
volatile long timeFPulse = 0;     // to determine when First impulse was interrupted
int coinType;
long interval;           // to determine interval from first and last impulse
const int max_interval = 500;
int total = 0;
const int slow_blink_time = 125;
const int fast_blink_time = 25;
const int gap_time = slow_blink_time * 2;
const int total_blink_time = 1000;

void setup(){
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(coinIntPin, INPUT_PULLUP);
    Serial.begin(9600);              
    attachInterrupt(digitalPinToInterrupt(coinIntPin), coinInserted, FALLING);
}

void coinInserted(){
    newCoin = true; 
    timeFPulse = millis();
    pulseCount++; 
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


void blink_times(int times, int time){
    for(int i = 0; i < times; i++){
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(time);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(time);                      // wait for a second
    }
}

void sendData(int cointType){
  if(coinType == 1){
    total = total + 1;
    int blink_time = total_blink_time / total;
    blink_times(total, blink_time);
  }
  else{
    blink_times(coinType, fast_blink_time);
    total = 0;
  }
}
