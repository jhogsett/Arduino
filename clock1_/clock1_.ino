#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 200
#define MAX_LED (LED_COUNT-1)

rgb_color colors[LED_COUNT];
//unsigned char existence[VLED_COUNT];

#define DELAY 5

#define FADE_RATE 0.95

#define BRIGHTNESS_PERCENT 15
#define BRIGHTNESS_DIVISOR 20.0
#define MIN_BRIGHTNESS 1.0

#define RESOLUTION 10000000.0

#define MIN_SPEED 0.20
#define MAX_SPEED 1.0
#define SPEED MIN_SPEED

#define NUM_ROWS 16
#define NUM_COLS 16
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)
#define MIN_ROW 0
#define MIN_COL 0

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

#define NPALETTE 6
#define NPRETTY_COLORS 6
rgb_color palette[NPALETTE] = { red, orange, yellow, green, blue, purple }; //, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { red, green, blue, orange, yellow, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { pink, blue, orange, seafoam, white, magenta, ltblue, ltgreen,  purple, red, green, yellow, cyan, gray, black }; 
//rgb_color palette[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 

/*
#define NGROUPS 3
#define GROUP_RED_START 0
#define GROUP_RED_MAX 4
#define GROUP_GREEN_START 4
#define GROUP_GREEN_MAX 8
#define GROUP_BLUE_START 8
#define GROUP_BLUE_MAX 12
#define GROUP_WHITE_START 12
#define GROUP_WHITE_MAX 13
*/

/*int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};
*/

rgb_color adjusted_palette[NPALETTE];

// bottom up starts on the left
int X = 255;
unsigned char translation[16][16] = {
  {   X,   X,   X,   X,   X,   0,   1,   2,   3,   4,   5,   X,   X,   X,   X,   X,  },
  {   X,   X,   X,   X,  13,  12,  11,  10,   9,   8,   7,   6,   X,   X,   X,   X,  },
  {   X,   X,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   X,   X,  },
  {   X,   X,  37,  36,  35,  34,  33,  32,  31,  30,  29,  28,  27,  26,   X,   X,  },
  {   X,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   X,  },
  {  67,  66,  65,  64,  63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  },
  {  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  },
  {  99,  98,  97,  96,  95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  },
  { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,  },
  { 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116,  },
  { 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147,  },
  {   X, 161, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148,   X,  },
  {   X,   X, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,   X,   X,  },
  {   X,   X, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174,   X,   X,  },
  {   X,   X,   X,   X, 186, 187, 188, 189, 190, 191, 192, 193,   X,   X,   X,   X,  },
  {   X,   X,   X,   X,   X, 199, 198, 197, 196, 195, 194,   X,   X,   X,   X,   X,  },
};

#define NUM_OBJECTS 6

#define WIDTH 3

//#define DECAY 0.0

//unsigned char bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

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

void fade_fast(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void fade_slow(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= FADE_RATE;
  }
}

float random_dir(){
  float result = ((random(RESOLUTION) / RESOLUTION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  result = random(2) == 1 ? result : result * -1.0;
  return result;
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

/*
bool position_occupied(int pos){
  return existence[pos] == 0 ? false : true;
}
*/

bool is_outside(int xpos, int ypos){
  return translation[ypos][xpos] == X ? true : false;
}

int reflect_f(float *xf, float *xd, float *yf, float *yd,float mxx = 1.0, float mxy = 1.0, float mnx = 0.0, float mny = 0.0){
//  float ld = *d;

  float nextx = *xf + *xd;
  float nexty = *yf + *yd;

  int nextxi = max(0, min(MAX_COL, int(nextx)));
  int nextyi = max(0, min(MAX_ROW, int(nexty)));
  
  bool clash = is_outside(nextxi, nextyi);

  if((nextx > mxx || nextx < mnx) || clash){
    *xd *= -1.0;
    clash = false;
  }  
  *xf += *xd;

  if((nexty > mxy || nexty < mny) || clash){
    *yd *= -1.0;
  }
  *yf += *yd;




/*  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;*/
}

int wrap_i(int i, int d, int mx){
  int next = i + d;
  if(next >= mx){
    return 0;
  } else {
    return next;
  }
}

int wrap_f(float *f, float *d, float mx = 1.0, float mn = 0.0){
  float next = *f + *d;

  if(next >= mx){
    next = mn;
  } else if(next < mn){
    next = mx;
  } 
  
  *f = next;
}

rgb_color add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

rgb_color subtract_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, color1.red - color2.red);
  new_color.green = max(0, color1.green - color2.green);
  new_color.blue = max(0, color1.blue - color2.blue);
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

void setup()
{
  randomSeed(analogRead(0));
  set_brightness();
  setup_colors();

/*
  for(int i = 0; i < LED_COUNT; i++){
     existence[i] = 0;
  }
  */
}

void draw(rgb_color color, int xpos, int ypos, int id){
  int led = translation[ypos][xpos];
  if(led == X) return;
  colors[led] = add_color(colors[led], color);
}

void undraw(rgb_color color, int xpos, int ypos, int id){
  int led = translation[xpos][ypos];
  colors[led] = subtract_color(colors[led], color);
}

void loop()
{
  set_brightness(BRIGHTNESS_PERCENT);

  float x[NUM_OBJECTS];
  float dx[NUM_OBJECTS];
  float y[NUM_OBJECTS];
  float dy[NUM_OBJECTS];
  int lx[NUM_OBJECTS];
  int ly[NUM_OBJECTS];

//  int group[NUM_OBJECTS];
//  int period[NUM_OBJECTS];
//  int count[NUM_OBJECTS];
//  bool state[NUM_OBJECTS];
//  rgb_color show_color[NUM_OBJECTS];

//  int choices[NGROUPS];
//  for(int i = 0; i < NGROUPS; i++){
//    choices[i] = -1;
//  }

/*
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
*/
  for(int i = 0; i < NUM_OBJECTS; i++){
    x[i] = random(NUM_COLS);
    y[i] = random(NUM_ROWS);
    dx[i] = random_dir(); 
    dy[i] = random_dir(); 
    lx[i] = -1;
    ly[i] = -1;
  }

  rgb_color random_colors[NUM_OBJECTS];
  
  for(int i = 0; i < NUM_OBJECTS; i++){
    random_colors[i] = adjusted_palette[i];
  }

  int counter = 0;
  while(true){
//    fade_fast();
    fade_slow();

    for(int i = 0; i < NUM_OBJECTS; i++){
      
//      reflect_f(&x[i], &dx[i], NUM_COLS);
//      reflect_f(&y[i], &dy[i], NUM_ROWS);

      reflect_f(&x[i], &dx[i], &y[i], &dy[i], NUM_COLS, NUM_ROWS);
      
//      wrap_f(&x[i], &dx[i], NUM_COLS);
//      wrap_f(&y[i], &dy[i], NUM_ROWS);

      int xpos = int(x[i]);
      int ypos = int(y[i]);
      
      // undraw(random_colors[i], lx[i], ly[i], i);
      draw(random_colors[i], xpos, ypos, i);

      lx[i] = xpos;
      ly[i] = ypos;
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

float deg_to_rad(float angle){
  return angle * DEG_TO_RAD; 
}


