#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 100
rgb_color colors[LED_COUNT];

#define DELAY 0
#define FADE_RATE 0.95

//#define RESOLUTION 10000
//#define MIN_SPEED 100
//#define SPEED 1.0

#define RESOLUTION 1000000
//#define SPEED_STEP 0.1
#define MIN_SPEED 0.01
#define MAX_SPEED 0.1
#define SPEED MIN_SPEED

#define NUM_OBJECTS 3

#define MIN_CHANGE 500
#define CHANGE_RANGE 2000

#define BRIGHTNESS_PERCENT 25
#define BRIGHTNESS_DIVISOR 20

/*
typedef struct high_color
{
  int red, green, blue;
} high_color;

high_color high_colors[LED_COUNT];
*/

#define NUM_ROWS 10
#define NUM_COLS 17
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)

/*
unsigned char translation_grid[10][17] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6},
  { 7, 7, 8, 8, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17},
  { 18, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 32},
  { 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49},
  { 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66},
  { 67, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 81},
  { 82, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92},
  { 93, 93, 93, 94, 94, 94, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98},
  { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}
};
*/

// hand tuned pass 2
unsigned char translation_grid[10][17] = {
  { 0,    0,    0,      0,      0,    0,    0,    0,    0,    0,      0,    0,    0,    0,    0,    0,    0},
  { 1,    1,    1,      2,      2,    2,    3,    3,    3,    4,      4,    4,    5,    5,    5,    6-1,  6},
  { 7,    7+1,  8,      8+1,    9,    10,   10+1, 11,   12,   12+1,   13,   14,   14+1, 15,   16,   16,   17},  
  { 18,   18+1, 19+1,   20+1,   21+0, 22,   23,   24,   25,   25+1,   26+1, 27+1, 28+1, 29,   30,   31,   32},
  { 33,   34,   35,     36,     37,   38,   39,   40,   41,   42,     43,   44,   45,   46,   47,   48,   49},
  { 50,   51,   52,     53,     54,   55,   56,   57,   58,   59,     60,   61,   62,   63,   64,   65,   66},
  { 67,   67+1, 68+1,   69+1,   70+0, 71,   72,   73,   74,   74+1,   75+1, 76+1, 77+1, 78,   79,   80,   81},
  { 82,   82+1, 83,     83+1,   84,   85,   85+1, 86,   87,   87+1,   88,   89,   89+1, 90,   91,   91,   92},
  { 93,   93,   93,     94,     94,   94,   95,   95,   95,   96,     96,   96,   97,   97,   97,   98-1, 98},
  { 99,   99,   99,     99,     99,   99,   99,   99,   99,   99,     99,   99,   99,   99,   99,   99,   99}
};

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

#define NPALETTE 13
#define NPRETTY_COLORS 12
//rgb_color palette[NPALETTE] = { red, orange, yellow, green, blue, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color adjusted_palette[NPALETTE] = { red, orange, yellow, green, blue, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { red, green, blue, orange, yellow, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 

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

//swaps red & green, needed on led strands, not on strips
void fix_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = palette[i].red;
      palette[i].red = palette[i].green;  
      palette[i].green = value;
    }
  }
  memcpy(adjusted_palette, palette, sizeof(palette));
}

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent=BRIGHTNESS_PERCENT){
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

unsigned char led_pos(int row, int col){
  return translation_grid[row][col];
}

void set_pixel(rgb_color color, int row, int col){
  int pos = led_pos(row, col);
  colors[pos].red += color.red; 
  colors[pos].green += color.green; 
  colors[pos].blue += color.blue; 
}

float random_dir(){
//  int range = random(RESOLUTION - MIN_SPEED) + MIN_SPEED;
//  return (range / (RESOLUTION * 1.0)) - 0.5;
  return ((random(RESOLUTION) / (RESOLUTION * 1.0)) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
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

// orbit

int random_seed(){
  int seed = analogRead(0);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}

void shuffle(int array[]){
  
}

void setup()
{
  random_seed();
  fix_colors();
  set_brightness();
}

void loop()
{
  float row[NUM_OBJECTS];
  float col[NUM_OBJECTS];
  float drow[NUM_OBJECTS];
  float dcol[NUM_OBJECTS];
  int color[NUM_OBJECTS];
  int change[NUM_OBJECTS];
  int lrow[NUM_OBJECTS];
  int lcol[NUM_OBJECTS];
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

  // assumes NUM_OBJECT <= NGROUPS
  for(int i = 0; i < NUM_OBJECTS; i++){
    row[i] = random(NUM_ROWS);
    col[i] = random(NUM_COLS);

    lrow[i] = -1;
    lcol[i] = -1;
    
    drow[i] = random_dir();
    dcol[i] = random_dir();
    
    // color[i] = random_color();
    group[i] = choices[i];
    color[i] = random(group_start[group[i]], group_max[group[i]]);
    
    change[i] = random(CHANGE_RANGE) + MIN_CHANGE;
  }

  while(true){
    //erase();
    fade();
    //fade_fast();

    for(int i = 0; i < NUM_OBJECTS; i++){
      lrow[i] = int(row[i]);
      lcol[i] = int(col[i]);

      reflect_f(&col[i], &dcol[i], MAX_COL+1);
      reflect_f(&row[i], &drow[i], MAX_ROW+1);

      if(int(row[i]) != lrow[i] || int(col[i]) != lcol[i]){
        set_pixel(adjusted_palette[color[i]], row[i], col[i]);
      }

      if(change[i]-- == 0){
        change[i] = random(CHANGE_RANGE) + MIN_CHANGE;
        
        //color[i] = random_color();;
        color[i] = random(group_start[group[i]], group_max[group[i]]);
        
        drow[i] = random_dir();
        dcol[i] = random_dir();
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



