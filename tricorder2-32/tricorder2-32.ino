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
#define LED_COUNT 32
rgb_color colors[LED_COUNT];


#define COLOR_VARIATIONS 4

#define HIGH_BRIGHTNESS 32
#define LOW_BRIGHTNESS 8

#define MIN_CHANGE 5
#define CHANGE_VARIATION 5

#define MIN_DELAY 10
#define DELAY_VARIATION 10

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

    int last = 0;

    int change = random(CHANGE_VARIATION) + MIN_CHANGE;
    int del = random(DELAY_VARIATION) + MIN_DELAY;

    while(change--){
      for(int j = 0; j < LED_COUNT; j++)
      {
        colors[last] = blank;
        colors[j] = color;
        last = j;
        
        ledStrip.write(colors, LED_COUNT);  
        delay(del);
      }

//      for(int j = LED_COUNT-2; j >= 1; j--)
//      {
//        colors[last] = blank;
//        colors[j] = color;
//        last = j;
//        
//        ledStrip.write(colors, LED_COUNT);  
//        delay(del);
//      }
    }

//    for(uint16_t j = 0; j < LED_COUNT; j++)
//    {
//      colors[j] = blank;
//      ledStrip.write(colors, LED_COUNT);  
//    }
}

