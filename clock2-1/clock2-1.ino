#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 200
#define MAX_LED (LED_COUNT-1)

rgb_color colors[LED_COUNT];
//unsigned char existence[VLED_COUNT];

#define NUM_OBJECTS 6
#define FADE_RATE 0.99
#define BRIGHTNESS_PERCENT 20
#define DELAY 0

#define MIN_SPEED 2.0
#define MAX_SPEED 4.0

#define BRIGHTNESS_DIVISOR 20.0
#define RANDOMNESS_QUANTIZATION 10000000.0

#define NUM_ROWS 16
#define NUM_COLS 16
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)
#define MIN_ROW 0
#define MIN_COL 0

#define RADIUS 7.25
#define OFFSET 8.0
#define ANGLE_ORIGIN 270.0
#define MAX_RADIUS RADIUS
#define MIN_RADIUS 1.0
#define MIN_RADIUS_CHANGE 0.40
#define MAX_RADIUS_CHANGE 0.80

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
rgb_color rainbow[NPALETTE] = { red, orange, yellow, green, blue, purple };
//rgb_color groups[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 
#define PALETTE rainbow
#define NPRETTY_COLORS 6
rgb_color adjusted_palette[NPALETTE];

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

/*
int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};
*/

#define X 255
// bottom up starts on the left
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

void set_brightness(int brightness_percent = BRIGHTNESS_PERCENT){
  float percent = brightness_percent / 100.0;
  for(int i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(PALETTE[i], percent);
  }
}

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
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

float random_float(){
  return random(RANDOMNESS_QUANTIZATION) / RANDOMNESS_QUANTIZATION;  
}

float random_sign(float input){
  return random(2) == 1 ? input : input * -1.0;
}

float random_dir(float max_speed = MAX_SPEED, float min_speed = MIN_SPEED, bool randomsign = true){
  float result = (random_float() * (max_speed - min_speed)) + min_speed;
  return randomsign ? random_sign(result) : result;
}

float random_angle(bool randomsign = true){
  float result = random_float() * 360.0;
  return randomsign ? random_sign(result) : result;
}

float random_radius(){
  return (random_float() * (MAX_RADIUS - MIN_RADIUS)) + MIN_RADIUS;
}

int random_color(){
  return random(NPRETTY_COLORS);
}

bool is_outside(int xpos, int ypos){
  return translation[ypos][xpos] == X ? true : false;
}

float reflect_f(float *f, float *d, float mx = 1.0, float mn = 0.0){
  float ld = *d;
  float next = *f + *d;
  if(next > mx || next < mn){
    *d *= -1.0;
  }
  *f += *d;

/*
  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;
  */
}

int reflect_f2(float *xf, float *xd, float *yf, float *yd,float mxx = 1.0, float mxy = 1.0, float mnx = 0.0, float mny = 0.0){
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

/*
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
*/

rgb_color add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

/*
rgb_color subtract_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, color1.red - color2.red);
  new_color.green = max(0, color1.green - color2.green);
  new_color.blue = max(0, color1.blue - color2.blue);
  return new_color;
}

// todo add weight
rgb_color blend_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, (color1.red + color2.red) / 2);
  new_color.green = max(0, (color1.green + color2.green) /2);
  new_color.blue = max(0, (color1.blue + color2.blue) / 2);
  return new_color;
}
*/

//swaps red & green, needed on led strands, not on strips
void setup_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = PALETTE[i].red;
      PALETTE[i].red = PALETTE[i].green;  
      PALETTE[i].green = value;
    }
  }
  memcpy(adjusted_palette, PALETTE, sizeof(PALETTE));
}

int random_seed(){
  int seed = analogRead(0);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}

void draw(rgb_color color, int xpos, int ypos, int id = 0){
  if(xpos < 0 || xpos > MAX_COL || ypos < 0 || ypos > MAX_ROW) return;
  int led = translation[ypos][xpos];
  if(led == X) return;
  colors[led] = add_color(colors[led], color);
}

void box(rgb_color color, int x1, int y1, int x2, int y2, int id = 0){
  int diffx = x2 - x1;
  int diffy = y2 - y1;
  int signx = diffx < 0 ? -1 : 1;  
  int signy = diffy < 0 ? -1 : 1;  
  for(int x = x1; x <= x2; x += signx){
    for(int y = y1; y <= y2; y += signy){
      draw(color, x, y, id);
    }
  }
}

void line(rgb_color color, int x1, int y1, int x2, int y2, int id = 0){
  int diffx = x2 - x1;
  int diffy = y2 - y1;
  int signx = diffx < 0 ? -1 : 1;  
  int signy = diffy < 0 ? -1 : 1;  
  for(int x = x1; x <= x2; x += signx){
    for(int y = y1; y <= y2; y += signy){
      draw(color, x, y, id);
    }
  }
}


/*
void undraw(rgb_color color, int xpos, int ypos, int id){
  int led = translation[xpos][ypos];
  colors[led] = subtract_color(colors[led], color);
}
*/

float add_angle(float angle1, float angle2){
  float angle = angle1 + angle2;
  if(angle < 0.0){
    angle += 360.0;
  } else if(angle > 360.0){
    angle -= 360.0;
  }
  return angle;
}

float deg_to_rad(float angle){
  return angle * DEG_TO_RAD; 
}

void plot(float radius, float angle, rgb_color color, int id = 0){
  float r = deg_to_rad(360.0 - add_angle(angle, ANGLE_ORIGIN));
  int xpos = int(cos(r) * radius + OFFSET);
  int ypos = int(sin(r) * radius + OFFSET);
  draw(color, xpos, ypos, id);  
}

bool plot2(int *xpos, int *ypos, int *lx, int *ly, float radius, float angle, rgb_color color, int id = 0){
  bool did_draw = false;
  float r = deg_to_rad(360.0 - add_angle(angle, ANGLE_ORIGIN));
  *xpos = int(cos(r) * radius + OFFSET);
  *ypos = int(sin(r) * radius + OFFSET);
  if(*xpos != *lx || *ypos != *ly){
    draw(color, *xpos, *ypos, id);  
    *lx = *xpos;
    *ly = *ypos;
    did_draw = true;
  }
  return did_draw;
}

void setup()
{
  random_seed();
  set_brightness();
  setup_colors(true);

/*
  for(int i = 0; i < LED_COUNT; i++){
     existence[i] = 0;
  }
*/
}

void loop()
{

  float a[NUM_OBJECTS];
  float r[NUM_OBJECTS];
  float da[NUM_OBJECTS];
  float dr[NUM_OBJECTS];

/*
  int group[NUM_OBJECTS];
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
*/

  float rangle = random_angle(false);
  //float rradius = random_radius();
  float rdangle = random_dir(MAX_SPEED, MIN_SPEED, false);
  float rdradius = random_dir(MAX_RADIUS_CHANGE, MIN_RADIUS_CHANGE, false);

  for(int i = 0; i < NUM_OBJECTS; i++){
//    a[i] = rangle;
    a[i] = (360.0 / NUM_OBJECTS) * i;
    //r[i] = rradius;
    r[i] = RADIUS - i;
    da[i] = rdangle;
    dr[i] = rdradius;
 }

  rgb_color random_colors[NUM_OBJECTS];
  
  for(int i = 0; i < NUM_OBJECTS; i++){
    random_colors[i] = adjusted_palette[i];
  }

  int xpos;
  int ypos;
  int lxpos = -1;
  int lypos = -1;
  
  while(true){
    //fade_fast();
    fade_slow();

/*
    for(int i = 0; i < NUM_OBJECTS; i++){
      a[i] = add_angle(a[i], da[i]);

      reflect_f(&r[i], &dr[i], MAX_RADIUS, MIN_RADIUS);
      
      bool did_plot = plot2(&xpos, &ypos, &lxpos, &lypos, r[i], a[i], random_colors[i]);

/*      if(did_plot){
        plot(r[i],  90.0 - a[i], random_colors[i]);
        plot(r[i], 180.0 - a[i], random_colors[i]);
        plot(r[i], 270.0 - a[i], random_colors[i]);
      }
*/
/*
      if(did_draw){
        for(float j = rad; j >=1; j--){
          plot(j, i, red); //random_colors[min(5, int(8-j))]);
        }
      }
*/      
    
    int x1 = 0, y1 = 0;
    while(is_outside(x1, y1)){
      x1 = random(NUM_COLS);
      y1 = random(NUM_ROWS);
    }

    int x2 = 0, y2 = 0;
    while(is_outside(x2, y2)){
      x2 = random(NUM_COLS);
      y2 = random(NUM_ROWS);
    }

    box(random_colors[random_color()], x1, y1, x2, y2); 

    ledStrip.write(colors, LED_COUNT); 
    delay(DELAY);
  }
}












