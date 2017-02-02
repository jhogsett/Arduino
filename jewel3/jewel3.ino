/* LedStripColorTester: Example Arduino sketch that lets you
 * type in a color on a PC and see it on the LED strip.
 *
 * To use this, you will need to plug an Addressable RGB LED
 * strip from Pololu into pin 12.  After uploading the sketch,
 * select "Serial Monitor" from the "Tools" menu.  In the input
 * box, type a color and press enter.
 *
 * The format of the color should be "R,G,B!" where R, G, and B
 * are numbers between 0 and 255 representing the brightnesses
 * of the red, green, and blue components respectively.
 *
 * For example, to get green, you could type:
 *   40,100,0!
 *
 * You can leave off the exclamation point if you change the
 * Serial Monitor's line ending setting to be "Newline" instead
 * of "No line ending".
 *
 * Please note that this sketch only transmits colors to the
 * the color.
 * LED strip after it receives them from the computer, so if
 * the LED strip loses power it will be off until you resend
 */
 
#include <PololuLedStrip.h>

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 7
#define FIRST_LED 1
rgb_color colors[LED_COUNT];

#define DELAY 90

#define COLOR_VARIATIONS 3

#define HIGH_BRIGHTNESS 127
#define LOW_BRIGHTNESS 127

int random_seed(){
  int seed = analogRead(0);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}


void setup()
{
  random_seed();

}

void loop()
{
    rgb_color color;

    color.red = random(COLOR_VARIATIONS) * HIGH_BRIGHTNESS;
    color.green = random(COLOR_VARIATIONS) * HIGH_BRIGHTNESS;
    color.blue = random(COLOR_VARIATIONS) * HIGH_BRIGHTNESS;

    rgb_color blank;
    blank.red = random(COLOR_VARIATIONS) * LOW_BRIGHTNESS;
    blank.green = random(COLOR_VARIATIONS) * LOW_BRIGHTNESS;
    blank.blue = random(COLOR_VARIATIONS) * LOW_BRIGHTNESS;

//    rgb_color blank;
//    blank.red = 255 - color.red;
//    blank.green = 255 - color.green;
//    blank.blue = 255 - color.blue;

    uint16_t last = 0;

    int change = random(10) + 10;
    int del = random(50) + 50;
    int offset = 0;

    while(change--){
      for(uint16_t j = FIRST_LED; j < LED_COUNT - 3; j++)
      {
        colors[(j + offset) % 7] = color;
      }

      for(uint16_t j = FIRST_LED + 4; j < LED_COUNT; j++)
      {
        colors[(j + offset) % 7] = blank;
      }

      // Write to the LED strip.
      ledStrip.write(colors, LED_COUNT);  

      delay(del);

      offset = (offset + 1) % 7;

//      for(uint16_t j = 0; j < LED_COUNT; j++)
//      {
//        colors[j] = blank;
//        ledStrip.write(colors, LED_COUNT);  
//      }
    }
}

