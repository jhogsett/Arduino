
#include <Arduino.h>
#include <PololuLedStrip.h>

// extend visual apparent range by modulating last led's brightness
// establish brightness baseline and ability to adjust overall brightness
// apply a damping function


// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<A3> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 7
rgb_color colors[LED_COUNT] = 
{
  { 0, 32, 0 }, 
  { 0, 32, 0 }, 
  { 0, 32, 0 }, 
  { 0, 32, 0 }, 
  { 32, 32, 0 }, 
  { 32, 32, 0 }, 
  { 32, 0, 0 } 
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

void setup()
{
  // Serial.begin(115200);
}

#define INTERVAL 50
unsigned long next_time = 0;
int value = 50;

void loop()
{
  if(millis() < next_time)
    return;
  next_time = millis() + INTERVAL;

  int r = random(11) - 5;

  value += r;

  if(value > 100)
    value = 100;
  if(value < 0)
    value = 0;

  int count = value / 8;
  if(count > 7)
    count = 7;

  rgb_color dbuffer[LED_COUNT];
  memcpy(dbuffer, colors, count * sizeof(rgb_color));
  memcpy(dbuffer + count, empty, (7 - count) * sizeof(rgb_color));

  // ledStrip.write(colors, count);
  // ledStrip.write(empty, 7-count);
  ledStrip.write(dbuffer, 7);
}
