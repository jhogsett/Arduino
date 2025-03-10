/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

// #define ENCODER_DO_NOT_USE_INTERRUPTS

#include <Encoder.h>

// labeled DT on device
#define CLK 3

// labeled CLK on device
#define DT 2

#define SW 4
#define LED 5

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(CLK, DT);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
  
  pinMode(LED, OUTPUT);
}

long oldPosition  = 0;
int voltage = 0;
int factor = 2;

void loop() {
  analogWrite(LED, voltage);

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    int diff = newPosition - oldPosition;

    Serial.println(diff);

    oldPosition = newPosition;

    voltage = (voltage + (diff * factor));    
    
    if(voltage < 0){
      voltage = 0;
    } else if(voltage > 255){
      voltage = 255;
    }
    
    // Serial.println(newPosition);
  }
}
