
#include <Arduino.h>
#include <PololuLedStrip.h>
#include <random_seed.h>

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

// extend visual apparent range by modulating last led's brightness
// establish brightness baseline and ability to adjust overall brightness
// apply a damping function


// // Create an ledStrip object and specify the pin it will use.
// PololuLedStrip<12> ledStrip;

// // Create a buffer for holding the colors (3 bytes per color).
// #define LED_COUNT 7
// rgb_color colors[LED_COUNT] = 
// {
//   { 0, 32, 0 }, 
//   { 0, 32, 0 }, 
//   { 0, 32, 0 }, 
//   { 0, 32, 0 }, 
//   { 32, 32, 0 }, 
//   { 32, 32, 0 }, 
//   { 32, 0, 0 } 
// };

// rgb_color empty[LED_COUNT] = 
// {
//   { 0, 0, 0 }, 
//   { 0, 0, 0 }, 
//   { 0, 0, 0 }, 
//   { 0, 0, 0 }, 
//   { 0, 0, 0 }, 
//   { 0, 0, 0 }, 
//   { 0, 0, 0 } 
// };

void setup()
{
  // Serial.begin(115200);
  randomizer.randomize();
}

// #define INTERVAL 50
// unsigned long next_time = 0;
// int value = 50;

// void loop()
// {
//   if(millis() < next_time)
//     return;
//   next_time = millis() + INTERVAL;

//   int r = random(11) - 5;

//   value += r;

//   if(value > 100)
//     value = 100;
//   if(value < 0)
//     value = 0;

//   int count = value / 8;
//   if(count > 7)
//     count = 7;

//   rgb_color dbuffer[LED_COUNT];
//   memcpy(dbuffer, colors, count * sizeof(rgb_color));
//   memcpy(dbuffer + count, empty, (7 - count) * sizeof(rgb_color));

//   // ledStrip.write(colors, count);
//   // ledStrip.write(empty, 7-count);
//   ledStrip.write(dbuffer, 7);
// }




























// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 7
rgb_color colors[LED_COUNT] = 
{
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 0, 15, 0 }, 
  { 15, 15, 0 }, 
  { 15, 15, 0 }, 
  { 15, 0, 0 } 
};

rgb_color empty[LED_COUNT] = 
{
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 }, 
  { 0, 0, 0 } 
};

// Global LED buffer to avoid stack allocation in step_sm()
rgb_color led_buffer[LED_COUNT];

#define INTERVAL 100
unsigned long next_time = 0;
int value = 127;

#define RANRANGE 21
#define HALFRANGE ((RANRANGE - 1) / 2)

void step_sm(unsigned long time)
{
	if(time < next_time)
		return;
	next_time = time + INTERVAL;

	int r = random(RANRANGE) - HALFRANGE;
	value += r;

	if(value > 255)
		value = 255;
	if(value < 0)
		value = 0;

	int sample = value * 2;
	int on_leds = (sample / 73) + 1;
	int remain = ((sample % 73) * 16) / 73;

	// Serial.println(remain);

//   remain = remain / 73
//   print(full_on)
//   print(remain)



//   int count = value / 8;
//   if(count > 7)
//     count = 7;
	// 

	memcpy(led_buffer, colors, on_leds * sizeof(rgb_color));
	memcpy(led_buffer + on_leds, empty, (LED_COUNT - on_leds) * sizeof(rgb_color));


	// for the last copied LED, modify it according to the remain value
	led_buffer[on_leds-1].red = (led_buffer[on_leds-1].red * remain) / 16;
	led_buffer[on_leds-1].green = (led_buffer[on_leds-1].green * remain) / 16;
	led_buffer[on_leds-1].blue = (led_buffer[on_leds-1].blue * remain) / 16;

  int option_contrast = 1;

	// modify whole display per display contrast
	for(byte i = 0; i < LED_COUNT; i++){
		led_buffer[i].red = (led_buffer[i].red * option_contrast)	/ 1;
		led_buffer[i].green = (led_buffer[i].green * option_contrast)	/ 1;
		led_buffer[i].blue = (led_buffer[i].blue * option_contrast)	/ 1;
	}

	ledStrip.write(led_buffer, LED_COUNT);
}

void loop(){
  while(true){
    step_sm(millis());
  }
}