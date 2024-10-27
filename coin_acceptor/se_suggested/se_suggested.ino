// https://arduino.stackexchange.com/questions/60764/coin-recognition-using-coin-accepter-in-arduino-uno

// pennies and dimes get confused
// without a 100nF cap between the coin pin and ground, the pulses can be noisy

#include  <Arduino.h>

const int coinIntPin = 2;          //interruptPin 0 is digital pin 2

volatile boolean newCoin = false;                  
volatile int pulseCount;          //counts pulse for 
// volatile int timeOut = 0;         //counts timeout after last coin inserted
volatile long timeFPulse = 0;     // to determine when First impulse was interrupted

int coinType;
int max_impulse = 25;              // this is the maximum impulses I use
//volatile long interval;           // to determine interval from first and last impulse
long interval;           // to determine interval from first and last impulse
int max_interval = 1000;
int total = 0;

void setup(){
    pinMode(2, INPUT_PULLUP);
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
            interval = millis() - timeFPulse;  //interval from first and last impulse
            // if(pulseCount == max_impulse || (interval > max_interval && pulseCount > 0)) // Max_impulse will avoid any extra impulses for any reason.
            if(interval > max_interval) // Max_impulse will avoid any extra impulses for any reason.
            {                                                                  
              coinType = pulseCount;
              pulseCount = 0;
              newCoin = false;
              sendData();
             }  
    }
}


void sendData(){
  total = total + coinType;
  Serial.print("Coin Type = ");
  Serial.print(coinType);
  Serial.print(" Total = ");
  Serial.println(total);
}
