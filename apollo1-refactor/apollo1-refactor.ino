

#define DATA_OUT_PIN 12               // data out pin for sending color data to the LEDs
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting
#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG

#define LED_COUNT 72
#define MLED_COUNT 36
#define MAX_LED (LED_COUNT-1)
#define MAX_MLED (MLED_COUNT)
#define DELAY 0
#define FADE_RATE 0.95
#define MIN_BRIGHTNESS_PERCENT 10
#define MAX_BRIGHTNESS_PERCENT 100
#define RESOLUTION 10000000.0
#define MIN_SPEED 0.20
#define MAX_SPEED 0.40
#define SPEED MIN_SPEED
#define NUM_OBJECTS 6
#define DECAY 0.0
#define MIN_CHANGE 2000
#define CHANGE_RANGE 3000
#define CHANGE_DELAY 1000
#define CHANGE_FADE 100
#define CHANGE_FADE_DELAY 10

#include <PololuLedStrip.h>
#include <math.h>
#include <colors.h>
#include <color_math.h>
#include <random_seed.h>
#include <auto_brightness.h>
//#include <buffer.h> has a bunch of depencies like effects
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<DATA_OUT_PIN> ledStrip;
RandomSeed<RANDOM_SEED_PIN> randomizer;
AutoBrightness<LIGHT_SENSOR_PIN> auto_brightness;
rgb_color colors[LED_COUNT];
unsigned char existence[MLED_COUNT];
unsigned char bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//Buffer buffer;

void flood(rgb_color color){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = color;
    existence[i] = 0;
  }
}

void erase(){
  flood(black);
}

float random_dir(){
  float result = ((random(RESOLUTION) / RESOLUTION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  result = random(2) == 1 ? result : result * -1.0;
  return result;
}

#include "fade.h"

bool position_occupied(int pos, int id){
  return (existence[pos] & ~bitmask[id]) == 0 ? false : true;
}

float reflect_f(float *f, float *d, int id, float mx = 1.0, float mn = 0.0){
  float ld = *d;
  float last = *f;
  float next = *f + *d;

  int lasti = int(last);
  int nexti = int(next);
  
  bool clash =  position_occupied(nexti, id);
  
  if(next > mx || next < mn || clash){
    *d *= -1.0;
  }
  *f += *d;

  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;
}

void setup()
{
  randomizer.randomize();
  ColorMath::setup_colors(false);
  auto_brightness.begin(MIN_BRIGHTNESS_PERCENT, MAX_BRIGHTNESS_PERCENT);
  auto_brightness.auto_adjust_brightness();
//  buffer.begin(&ledStrip, colors);
  
  for(int i = 0; i < MLED_COUNT; i++){
     existence[i] = 0;
  }
}

#include "draw.h"


void loop()
{
  auto_brightness.auto_adjust_brightness();

  float x[NUM_OBJECTS];
  float dx[NUM_OBJECTS];
  int lx[NUM_OBJECTS];
  int group[NUM_OBJECTS];
  int period[NUM_OBJECTS];
  int count[NUM_OBJECTS];
  bool state[NUM_OBJECTS];
  rgb_color show_color[NUM_OBJECTS];

  int choices[NGROUPS];
  for(int i = 0; i < NGROUPS; i++){
    choices[i] = -1;
  }

  for(int i = 0; i < NGROUPS; i++){
    while(true){
      int color = random(NGROUPS);
  
      bool try_again = false;
      for(int j = 0; j < i; j++){
        if(color == choices[j]){
          try_again = true;
          break;
        }
      }
      if(try_again == true){
        continue;
      }

      choices[i] = color;
      break;
    }  
  }

  int change = random(CHANGE_RANGE) + MIN_CHANGE;

  rgb_color random_colors[NUM_OBJECTS];
  float dir = random_dir();

#define DISTR_MAX 36
#define DISTR_ADJ 0.0

  for(int i = 0; i < NUM_OBJECTS; i++){
    x[i] = int((i * (DISTR_MAX / float(NUM_OBJECTS))) + DISTR_ADJ);
    
    dx[i] = dir;
    lx[i] = -1;

    group[i] = choices[i % NGROUPS];
    random_colors[i] = adjusted_palette[random(group_start[group[i]], group_max[group[i]])];
    show_color[i] = random_colors[i / 2];
  }

  while(true){
    fade_fast();

    for(int i = 0; i < NUM_OBJECTS; i++){
      reflect_f(&x[i], &dx[i], i, MAX_MLED);

      int tpos = int(x[i]);
        undraw(random_colors[i], lx[i], i);
        draw(show_color[i], tpos, i);
        lx[i] = tpos;
    
    }

    ledStrip.write(colors, LED_COUNT); 
    delay(DELAY);

    if(change-- == 0){

      fade_down();

      auto_brightness.auto_adjust_brightness();

      dir = random_dir();

      for(int i = 0; i < NGROUPS; i++){
        choices[i] = -1;
      }

      for(int i = 0; i < NGROUPS; i++){
        while(true){
          int color = random(NGROUPS);
      
          bool try_again = false;
          for(int j = 0; j < i; j++){
            if(color == choices[j]){
              try_again = true;
              break;
            }
          }
          if(try_again == true){
            continue;
          }
    
          choices[i] = color;
          break;
        }  
      }

      for(int i = 0; i < NUM_OBJECTS; i++){
        x[i] = int((i * (DISTR_MAX / float(NUM_OBJECTS))) + DISTR_ADJ);
        dx[i] = dir;
        lx[i] = -1;
        group[i] = choices[i % NGROUPS];
        random_colors[i] = adjusted_palette[random(group_start[group[i]], group_max[group[i]])];
        show_color[i] = random_colors[i/2];
      }
      
      change = random(CHANGE_RANGE) + MIN_CHANGE;
    }
  }
}


