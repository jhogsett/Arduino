// try: dim at ends per cosine, bump into each other and transfer momentum

#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 72
#define VLED_COUNT 105
#define MAX_LED (LED_COUNT-1)
#define MAX_VLED (VLED_COUNT-1)
rgb_color colors[LED_COUNT];

#define DELAY 0
#define FADE_RATE 3

#define BRIGHTNESS_PERCENT 7
#define BRIGHTNESS_DIVISOR 20.0
#define MIN_BRIGHTNESS 1.0

#define RESOLUTION 1000000.0

#define SPEED_STEP 0.1
#define MIN_SPEED 0.15
#define MAX_SPEED 0.3
#define SPEED MIN_SPEED

#define NUM_ROWS 10
#define NUM_COLS 17
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)

rgb_color black = {0, 0, 0};
rgb_color gray = {10, 10, 10};
rgb_color white = {20, 20, 20};
rgb_color red = {20, 0, 0};
rgb_color orange = {20, 10, 0};
rgb_color yellow = {20, 20, 0};
rgb_color ltgreen = {10, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color seafoam = {0, 20, 10};
rgb_color cyan = {0, 20, 20};
rgb_color ltblue = {0, 10, 20};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color magenta = {20, 0, 20};
rgb_color pink = {20, 0, 10};

#define NPALETTE 15
#define NPRETTY_COLORS 13
//rgb_color palette[NPALETTE] = { red, orange, yellow, green, blue, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { red, green, blue, orange, yellow, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { pink, blue, orange, seafoam, white, magenta, ltblue, ltgreen,  purple, red, green, yellow, cyan, gray, black }; 

rgb_color palette[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 

#define NGROUPS 3
#define GROUP_RED_START 0
#define GROUP_RED_MAX 4
#define GROUP_GREEN_START 4
#define GROUP_GREEN_MAX 8
#define GROUP_BLUE_START 8
#define GROUP_BLUE_MAX 12
#define GROUP_WHITE_START 12
#define GROUP_WHITE_MAX 13

int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};

rgb_color adjusted_palette[NPALETTE];
int sin105_translation[105] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 71, 71};

#define NUM_OBJECTS 3

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent = BRIGHTNESS_PERCENT){
  float percent = brightness_percent / 100.0;
  for(int i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(palette[i], percent);
  }
}

void flood(rgb_color color){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = color;
  }
}

void erase(){
  flood(black);
}

void fade(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void fade_fast(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void fade_fast2(int rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = max(0, *(p + i) - rate);
  }
}

float random_dir(){
  return ((random(RESOLUTION) / RESOLUTION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  //int range = random(RESOLUTION - MIN_SPEED) + MIN_SPEED;
  //return (range / (RESOLUTION * 1.0)) - 0.5;
}

int random_color(){
  return random(NPRETTY_COLORS);
}

int reflect_i(int *i, int *d, int mx, int mn = 0){
  int ld = *d;
  int next = *i + *d;
  if(next > mx || next < mn){
    *d *= -1;
  }
  *i += *d;

  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;    
}

float reflect_f(float *f, float *d, float mx = 1.0, float mn = 0.0){
  float ld = *d;
  float next = *f + *d;
  if(next > mx || next < mn){
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

int wrap_i(int i, int d, int mx){
  int next = i + d;
  if(next >= mx){
    return 0;
  } else {
    return next;
  }
}

float wrap_f(float f, float d, float mx = 1.0){
  float next = f + d;
  if(next >= mx){
    return 0.0;
  } else {
    return next;
  }
}

rgb_color add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

//swaps red & green, needed on led strands, not on strips
void setup_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = palette[i].red;
      palette[i].red = palette[i].green;  
      palette[i].green = value;
    }
  }
  memcpy(adjusted_palette, palette, sizeof(palette));
}

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
  setup_colors(false);
  set_brightness();
}

/*
#define MAX_BRIGHTNESS 20.0
#define BRIGHTNESS_STEP 2.0
*/

void poof(int color, int times=10, int brightness=50){
  set_brightness(brightness);
  for(int i = 0; i < times; i++){
    flood(black);
    ledStrip.write(colors, LED_COUNT);
    delay(33);
    flood(adjusted_palette[color]);
    ledStrip.write(colors, LED_COUNT);
    delay(33);
  }

  flood(black);
  ledStrip.write(colors, LED_COUNT);
  delay(33);
}

void loop()
{
  set_brightness(BRIGHTNESS_PERCENT);

  float x[NUM_OBJECTS];
  float dx[NUM_OBJECTS];
  int group[NUM_OBJECTS];

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

  for(int i = 0; i < NUM_OBJECTS; i++){
    x[i] = i * (VLED_COUNT / NUM_OBJECTS);
    dx[i] = random_dir(); 
  }

  rgb_color random_colors[NGROUPS];
  for(int i = 0; i < NGROUPS; i++){
    group[i] = choices[i];
    random_colors[i] = adjusted_palette[random(group_start[group[i]], group_max[group[i]])];
  }

  while(true){
    //fade();
    //erase();
    fade_fast();
    //fade_fast2();

    for(int i = 0; i < NUM_OBJECTS; i++){
      float lx = x[i];
      reflect_f(&x[i], &dx[i], MAX_VLED);
      if(lx != x[i]){
        int pos = int(x[i]);
        int tpos = sin105_translation[pos];
        colors[tpos] = add_color(colors[tpos], random_colors[i]);
      }
    }
    
    ledStrip.write(colors, LED_COUNT); 
  }


}

int low_limit(int value, int limit){
  return value < limit ? limit : value;  
}

int high_limit(int value, int limit){
  return value > limit ? limit : value;  
}

int keep_in(int value, int lower, int upper){
  return low_limit(high_limit(value, upper), lower);
}



