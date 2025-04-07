/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Arduino.h>
#include <limits.h>
#include <Encoder.h>
#include <hsl_to_rgb.h>
#include "encoder_handler.h"

#define _IDA 1

#define CLKA 2
#define DTA 3
#define SWA 4

#define CLKB 5
#define DTB 6
#define SWB 7

// #define CLKC 8
// #define DTC 9
// #define SWC 10

// #define CLKD 0
// #define DTD 0
// #define SWD 11

#define LEDR 9
#define LEDG 10
#define LEDB 11

#define PULSES_PER_DETENT 2

// typedef struct rgb_color
// {
//   unsigned char red, green, blue;
// } rgb_color;
   
// // hue: 0-359, sat: 0-255, val (lightness): 0-255
// rgb_color hsl_to_rgb(int hue, int sat=255, int val=255) {
//   int r, g, b, base;

//   if (sat == 0) { // Achromatic color (gray).
//     r = val;
//     g = val;
//     b = val;
//   } else  {
//     base = ((255 - sat) * val)>>8;
//     switch(hue/60) {
//       case 0:
//         r = val;
//         g = (((val-base)*hue)/60)+base;
//         b = base;
//         break;
//       case 1:
//         r = (((val-base)*(60-(hue%60)))/60)+base;
//         g = val;
//         b = base;
//         break;
//       case 2:
//         r = base;
//         g = val;
//         b = (((val-base)*(hue%60))/60)+base;
//         break;
//       case 3:
//         r = base;
//         g = (((val-base)*(60-(hue%60)))/60)+base;
//         b = val;
//         break;
//       case 4:
//         r = (((val-base)*(hue%60))/60)+base;
//         g = base;
//         b = val;
//         break;
//       case 5:
//         r = val;
//         g = base;
//         b = (((val-base)*(60-(hue%60)))/60)+base;
//         break;
//     }
//   }
//   return (rgb_color){r, g, b};
// }   
    
EncoderHandler encoder_handlerA(1, CLKA, DTA, SWA, PULSES_PER_DETENT);
EncoderHandler encoder_handlerB(2, CLKB, DTB, SWB, PULSES_PER_DETENT);
// EncoderHandler encoder_handlerC(3, CLKC, DTC, SWC, PULSES_PER_DETENT);
// EncoderHandler encoder_handlerD(4, CLKD, DTD, SWD, PULSES_PER_DETENT);

int hue = 0;
int sat = 255;
int lit = 255;

void setup(){
  // Serial.begin(230400);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // pinMode(13, OUTPUT);
}

void loop() {
  rgb_color rgbc = hsl_to_rgb(hue, sat, lit);
  analogWrite(LEDR, rgbc.red);
  analogWrite(LEDG, rgbc.green);
  analogWrite(LEDB, rgbc.blue);
  if(encoder_handlerA.step()){
    switch(encoder_handlerA.last_diff){
      case -1:
        hue--;
        if(hue < 0)
          hue = 359;
        sat = 255;
        lit = 255;
        break;
      case 0:
        hue = 0;
        sat = 0;
        lit = 255;
        break;
      case 1:
        hue++;
        if(hue > 360)
          hue = 0;
        sat = 255;
        lit = 255;
        break;
      case 2:
        hue = 0;
        sat = 0;
        lit = 0;
        break;
    }
  }
  if(encoder_handlerB.step()){
    switch(encoder_handlerB.last_diff){
      case -1:
        sat--;
        if(sat < 0)
          sat = 0;
        // sat = 255;
        lit = 255;
        break;
      case 0:
        hue = 0;
        sat = 0;
        lit = 1;
        break;
      case 1:
        sat++;
        if(sat > 255)
          sat = 255;
        // sat = 255;
        lit = 255;
        break;
      case 2:
        hue = 0;
        sat = 0;
        lit = 0;
        break;
    }
  }
  // encoder_handlerC.step();
  // encoder_handlerD.step();
}
