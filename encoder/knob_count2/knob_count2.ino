/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Arduino.h>
#include <Encoder.h>

// labeled DT on device
#define CLK 2

// labeled CLK on device
#define DT 3

#define SW 4
#define LED A3


// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(CLK, DT);
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
    digitalWrite(LED, LOW);
  }
  
  void step(){
    unsigned long time = millis();
    if(time >= next_time){
      if(blink_counter > 0){
        if(currently_on){
          currently_on = false;
          digitalWrite(LED, LOW);
          next_time = time + _blank_time;
          blink_counter--;
        } else {
          currently_on = true;
          digitalWrite(LED, HIGH);
          next_time = time + _blink_time;
        }
        
        if(blink_counter == 0){
          if(currently_on){
            digitalWrite(LED, LOW);
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

#define LED_PIN 5
#define GAP_TIME 1000
#define BLINK_TIME 100
#define BLANK_TIME 200

Blinker led1(5);

void setup(){
  // Serial.begin(9600);
  // Serial.println("Basic Encoder Test:");
  
  pinMode(LED, OUTPUT);
  
  led1.begin(0, BLINK_TIME, BLANK_TIME, GAP_TIME);
}

long oldPosition  = 0;
int voltage = 0;
int factor = 1;

void loop() {
  led1.step();

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    int diff = newPosition - oldPosition;

    oldPosition = newPosition;

    voltage = (voltage + (diff * factor));    
    
    if(voltage < 0){
      voltage = 0;
    } else if(voltage > 255){
      voltage = 255;
    }
    
    int value = voltage / 2;
    if(value >= 0)
      led1.begin(value, -1, -1, -1, true);
  }
}
