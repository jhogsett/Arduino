#include <Adafruit_NeoPixel.h> 

#define PIN 38
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

void setup () {  
  
  pixels.begin();

}

void loop () {

  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(random(64), random(64), random(64)));
    pixels.show();
    delay(DELAYVAL);
  }

}