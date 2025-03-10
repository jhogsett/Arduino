/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Arduino.h>
#include <Encoder.h>

#define CLKA 2
#define DTA 3
#define SWA 4
#define LEDA 9

#define CLKB 5
#define DTB 6
#define SWB 7
#define LEDB 10

#define CLKC 8
#define DTC 9
#define SWC 10
#define LEDC 11

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEncA(CLKA, DTA);
Encoder myEncB(CLKB, DTB);
Encoder myEncC(CLKC, DTC);
//   avoid using pins with LEDs attached

class Blinker
{
public:

  static const int DEFAULT_COUNT = 1;
  static const int DEFAULT_BLINK = 333;
  static const int DEFAULT_BLANK = 667;
  static const int DEFAULT_GAP = 1000;

  Blinker(int led_pin){
    _led_pin = led_pin;   
    _blink_count = DEFAULT_COUNT;
    _blink_time = DEFAULT_BLINK; 
    _blank_time = DEFAULT_BLANK;
    _gap_time = DEFAULT_GAP;
  }

  void begin(int blink_count=-1, int blink_time=-1, int blank_time=-1, int gap_time=-1, bool keep_state=false){
    _blink_count = blink_count == -1 ? _blink_count : blink_count;
    _blink_time = blink_time == -1 ? _blink_time : blink_time;
    _blank_time = blank_time == -1 ? _blank_time : blank_time;
    _gap_time = gap_time == -1 ? _gap_time : gap_time;

    if(!keep_state){
      blink_counter = _blink_count;  
      currently_on = false;
    }
    digitalWrite(_led_pin, LOW);
  }
  
  void step(){
    unsigned long time = millis();
    if(time >= next_time){
      if(blink_counter > 0){
        if(currently_on){
          currently_on = false;
          digitalWrite(_led_pin, LOW);
          next_time = time + _blank_time;
          blink_counter--;
        } else {
          currently_on = true;
          digitalWrite(_led_pin, HIGH);
          next_time = time + _blink_time;
        }
        
        if(blink_counter == 0){
          if(currently_on){
            digitalWrite(_led_pin, LOW);
          }
          next_time = time + _gap_time;
        }
      } else {
        // gap time done
        blink_counter = _blink_count;  
      }
    }  
  }

private:

  int _led_pin;
  int _blink_count;
  int _blink_time;
  int _blank_time;
  int _gap_time;
  int blink_counter;
  bool currently_on = false;
  unsigned long next_time = 0L;

};

#define GAP_TIME 1000
#define BLINK_TIME 100
#define BLANK_TIME 200

Blinker ledA(LEDA);
Blinker ledB(LEDB);
Blinker ledC(LEDC);

void setup(){
  // Serial.begin(9600);
  // Serial.println("Basic Encoder Test:");
  
  pinMode(LEDA, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDC, OUTPUT);
  
  ledA.begin(0, BLINK_TIME, BLANK_TIME, GAP_TIME);
  ledB.begin(0, BLINK_TIME, BLANK_TIME, GAP_TIME);
  ledC.begin(0, BLINK_TIME, BLANK_TIME, GAP_TIME);
}

long oldPositionA  = 0;
int voltageA = 0;

long oldPositionB  = 0;
int voltageB = 0;

long oldPositionC  = 0;
int voltageC = 0;

int factor = 1;

void loop() {
  ledA.step();
  ledB.step();
  ledC.step();

  long newPositionA = myEncA.read();
  if (newPositionA != oldPositionA) {
    int diffA = newPositionA - oldPositionA;

    oldPositionA = newPositionA;

    voltageA = (voltageA + (diffA * factor));    
    
    if(voltageA < 0){
      voltageA = 0;
    } else if(voltageA > 511){
      voltageA = 511;
    }
    
    int valueA = voltageA / 2;
    if(valueA >= 0)
      ledA.begin(valueA, -1, -1, -1, true);
      // analogWrite(LEDA, valueA);
  }

  long newPositionB = myEncB.read();
  if (newPositionB != oldPositionB) {
    int diffB = newPositionB - oldPositionB;

    oldPositionB = newPositionB;

    voltageB = (voltageB + (diffB * factor));    
    
    if(voltageB < 0){
      voltageB = 0;
    } else if(voltageB > 511){
      voltageB = 511;
    }
    
    int valueB = voltageB / 2;
    if(valueB >= 0)
      ledB.begin(valueB, -1, -1, -1, true);
      // analogWrite(LEDB, valueB);
  }

  long newPositionC = myEncC.read();
  if (newPositionC != oldPositionC) {
    int diffC = newPositionC - oldPositionC;

    oldPositionC = newPositionC;

    voltageC = (voltageC + (diffC * factor));    
    
    if(voltageC < 0){
      voltageC = 0;
    } else if(voltageC > 511){
      voltageC = 511;
    }
    
    int valueC = voltageC / 2;
    if(valueC >= 0)
      ledC.begin(valueC, -1, -1, -1, true);
      // analogWrite(LEDC, valueC);
  }

}
