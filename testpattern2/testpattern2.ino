#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 400
#define VLED_COUNT 32
#define MAX_LED (VLED_COUNT)
rgb_color colors[LED_COUNT];

#define DELAY 0
#define FADE_RATE 3

#define BRIGHTNESS_PERCENT 2
#define BRIGHTNESS_DIVISOR 20.0
#define MIN_BRIGHTNESS 1.0

#define RESOLUTION 10000

#define SPEED_STEP 0.1
#define MIN_SPEED 0.01
#define MAX_SPEED 0.1
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
rgb_color egnaro = {10, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color grue = {0, 20, 10};
rgb_color cyan = {0, 20, 20};
rgb_color eurg = {0, 10, 20};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color magenta = {20, 0, 20};
rgb_color rurple = {20, 0, 10};

#define NPALETTE 15
#define NPRETTY_COLORS 13
//rgb_color palette[NPALETTE] = { red, orange, yellow, green, blue, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
rgb_color palette[NPALETTE] = { red, green, blue, orange, yellow, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
rgb_color adjusted_palette[NPALETTE] = { red, orange, yellow, green, blue, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 

#define NUM_OBJECTS 1

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent){
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
  int range = random(RESOLUTION - MIN_SPEED) + MIN_SPEED;
  return (range / (RESOLUTION * 1.0)) - 0.5;
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

void setup()
{

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

#define TIMES 1
#define OFFSET 10

void draw(int pos, rgb_color color){
  for(int i = 0; i < TIMES; i++)
  {
    int offset = (i * OFFSET) + pos;
    colors[offset] = add_color(colors[offset], color);
  }
}

void loop()
{
  set_brightness(BRIGHTNESS_PERCENT);

  float x[NUM_OBJECTS];
  //float lx[NUM_OBJECTS];
  float dx[NUM_OBJECTS];

  for(int i = 0; i < NUM_OBJECTS; i++){
    x[i] = i * (VLED_COUNT / NUM_OBJECTS);
    dx[i] = ((random(RESOLUTION) / (RESOLUTION * 1.0)) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  }

#define COLOR_CHANGE 20

  int ncolor = COLOR_CHANGE;
  int icolor = 0;
  while(true){
    //fade();
    //erase();
    fade_fast();
    //fade_fast2();

    for(int i = 0; i < NUM_OBJECTS; i++){
      float lx = x[i];
      reflect_f(&x[i], &dx[i], MAX_LED);
      if(lx != x[i]){
//        colors[int(x[i])] = add_color(colors[int(x[i])], adjusted_palette[i % NPRETTY_COLORS]);
//        colors[] = add_color(colors[int(x[i])], adjusted_palette[icolor]);
        draw(int(x[i]), adjusted_palette[icolor]);

        if(--ncolor == 0)
        {
          icolor = (++icolor % NPRETTY_COLORS);
          ncolor = COLOR_CHANGE;
        }
      }
    }
    
    ledStrip.write(colors, LED_COUNT); 
    delay(DELAY);
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



