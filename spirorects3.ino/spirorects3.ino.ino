#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 200
#define MAX_LED (LED_COUNT-1)
rgb_color colors[LED_COUNT];

unsigned char existence[LED_COUNT];
unsigned char bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

#define MIN_STEP 3
#define MAX_STEP 7
#define MIN_RECTS 1
#define MAX_RECTS 3

#define NUM_OBJECTS 24
#define FADE_RATE 0.75
#define BRIGHTNESS_PERCENT 5
#define DELAY 0
#define MIN_CHANGE 250
#define MAX_CHANGE 750

#define MIN_SPEED 0.05
#define MAX_SPEED 0.3

#define BRIGHTNESS_DIVISOR 20.0
#define RANDOMNESS_QUANTIZATION 10000000.0

#define NUM_ROWS 16
#define NUM_COLS 16
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)
#define MIN_ROW 0
#define MIN_COL 0

//#define RADIUS 7.25
#define RADIUS 8.0
//#define DIA (RADIUS * 2)
//#define ADJ ((NUM_COLS - DIA) / 2)

#define OFFSET 8.0
//#define OFFSET (RADIUS + ADJ)

#define ANGLE_ORIGIN 270.0
#define MAX_RADIUS RADIUS
#define MIN_RADIUS (RADIUS / 2)
#define MIN_RADIUS_CHANGE 0.40
#define MAX_RADIUS_CHANGE 0.80

#define FADE_TIMES 75
#define FADE_DELAY 8
#define FTB_RATE 0.999
#define FADE_WAIT 600

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
#define NPRETTY_COLORS 13
rgb_color rainbow[NPALETTE] = { red, orange, yellow, green, blue, purple };
rgb_color groups[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 
rgb_color rgb[NPALETTE] = { red, green, blue, cyan, yellow, magenta };
rgb_color cops[NPALETTE] = { red, white, blue, red, white, blue };
rgb_color interest[NPALETTE] = { orange, ltgreen, ltblue, seafoam, pink, purple };
#define PALETTE groups
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
  clear();
}

void fade_fast(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void fade_slow(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void fade_to_black(){
  delay(FADE_WAIT);
  for(int i = 0; i < FADE_TIMES; i++){
    fade_slow(FTB_RATE);
    ledStrip.write(colors, LED_COUNT); 
    delay(FADE_DELAY);
  }
}

float random_float(){
  return random(RANDOMNESS_QUANTIZATION) / RANDOMNESS_QUANTIZATION;  
}

float random_sign(float input){
  return random(2) == 1 ? input : input * -1.0;
}

float random_dir(){
  float result = ((random(RANDOMNESS_QUANTIZATION) / RANDOMNESS_QUANTIZATION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  result = random(2) == 1 ? result : result * -1.0;
  return result;
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
  if(xpos < 0 || xpos > MAX_COL) return true;
  if(ypos < 0 || ypos > MAX_ROW) return true;
  return translation[ypos][xpos] == X ? true : false;
}

bool is_inside(int xpos, int ypos){
  return !is_outside(xpos, ypos);
}

int reflect_f(float *xf, float *xd, float *yf, float *yd,float mxx = 1.0, float mxy = 1.0, float mnx = 0.0, float mny = 0.0){
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

void draw(rgb_color color, int x, int y, int id = 0){
  int led = translation[y % NUM_ROWS][x % NUM_COLS];
  if(led != X) colors[led] = add_color(colors[led], color);
  existence[led] |= bitmask[id];
}

void undraw(rgb_color color, int xpos, int ypos, int id = 0){
  int led = translation[xpos][ypos];
  colors[led] = subtract_color(colors[led], color);
  existence[led] &= ~bitmask[id];
}

void clear(){
  for(int i = 0; i < LED_COUNT; i++){
    existence[i] = 0;
  }
}

void box(rgb_color color, int x1, int y1, int x2, int y2, int id = 0){
  int diffx = x2 - x1;
  int diffy = y2 - y1;
  int signx = diffx < 0 ? -1 : 1;  
  int signy = diffy < 0 ? -1 : 1;  
  for(int x = x1; (signx > 0 ? x <= x2 : x >= x2); x += signx){
    for(int y = y1; (signy > 0 ? y <= y2 : y >= y2); y += signy){
      draw(color, x, y, id);
    }
  }
}

void _line(int t, float dt, int lfrom, int lto, bool vertical, bool lforward, bool tforward, rgb_color color, int id){
  float trailing_float = t;
  for(int leading_int = lfrom; (lforward ? leading_int <= lto : leading_int >= lto); leading_int += (lforward ? 1 : -1)){
    if(vertical) draw(color, trailing_float, leading_int, id);
    else draw(color, leading_int, trailing_float, id);
    trailing_float += ( tforward ? dt : -dt);
  }
}

void line(rgb_color color, int x1, int y1, int x2, int y2, int id = 0){
  float w = x2 - x1, h = y2 - y1;
  if(w == 0 && h == 0){
    draw(color, x1, y1, id);
  } else {
    bool wforward = w > 0, hforward = h > 0;
    h *= wforward ? 1 : -1;
    w *= hforward ? 1 : -1;
    float x_per_y = h == 0 ? 0 : w/h, y_per_x = w == 0 ? 0 : h/w;
    if(w == 0 || x_per_y < y_per_x){
      _line(x1, w == 0 ? 0 : x_per_y, y1, y2, true, hforward, wforward, color, id);
    } else if(h == 0 || x_per_y >= y_per_x){
      _line(y1, h == 0 ? 0 : y_per_x, x1, x2, false, wforward, hforward, color, id);
    }
  }
}

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

float angle_to_rad(float angle){
  return deg_to_rad(360.0 - add_angle(angle, ANGLE_ORIGIN));
}

void polor_to_rect(float radius, float angle, int *x, int *y, float offset_x = OFFSET, float offset_y = OFFSET){
  float rad = angle_to_rad(angle);
  *x = cos(rad) * radius + offset_x;
  *y = sin(rad) * radius + offset_y;
}

void plot(float radius, float angle, rgb_color color, int id = 0){
  float rad = angle_to_rad(angle);
  int x, y;
  polor_to_rect(radius, angle, &x, &y);
  draw(color, x, y, id);  
}

bool plot2(int *x, int *y, int *lx, int *ly, float radius, float angle, rgb_color color, int id = 0){
  bool did_draw = false;
  float rad = angle_to_rad(angle);
  polor_to_rect(radius, angle, x, y);
  if(*x != *lx || *y != *ly){
    draw(color, *x, *y, id);  
    *lx = *x;
    *ly = *y;
    did_draw = true;
  }
  return did_draw;
}

void setup()
{
  random_seed();
  setup_colors(true);

/*
  for(int i = 0; i < LED_COUNT; i++){
     existence[i] = 0;
  }
*/
}

void loop()
{
  set_brightness();
/*  float x[NUM_OBJECTS];
  float dx[NUM_OBJECTS];
  float y[NUM_OBJECTS];
  float dy[NUM_OBJECTS];
  int lx[NUM_OBJECTS];
  int ly[NUM_OBJECTS];

  for(int i = 0; i < NUM_OBJECTS; i++){
    x[i] = random(NUM_COLS);
    y[i] = random(NUM_ROWS);
    dx[i] = random_dir(); 
    dy[i] = random_dir(); 
    lx[i] = -1;
    ly[i] = -1;
  }
*/
/*
  rgb_color random_colors[NUM_OBJECTS];
  
  for(int i = 0; i < NUM_OBJECTS; i++){
    random_colors[i] = adjusted_palette[i];
  }
*/  
/*
  while(true){
    //fade_slow();
    erase();
    for(int i = 0; i < NUM_OBJECTS; i++){
      reflect_f(&x[i], &dx[i], &y[i], &dy[i], NUM_COLS, NUM_ROWS);
      int xpos = int(x[i]);
      int ypos = int(y[i]);
      
      // undraw(random_colors[i], lx[i], ly[i], i);
      // draw(random_colors[i], xpos, ypos, i);
      lx[i] = xpos;
      ly[i] = ypos;
    }

    for(int i = 0; i < (NUM_OBJECTS/2); i++){
      int x1 = x[(i * 2)];
      int y1 = y[(i * 2)];
      int x2 = x[(i * 2) + 1];
      int y2 = y[(i * 2) + 1];
      line(random_colors[i], x1, y1, x2, y2); 
    }

    ledStrip.write(colors, LED_COUNT); 
    delay(DELAY);
  }
*/



  float angle[NUM_OBJECTS];
  float radius[NUM_OBJECTS];
  float astep[NUM_OBJECTS];
  int x[NUM_OBJECTS];
  int y[NUM_OBJECTS];
  int lx[NUM_OBJECTS];
  int ly[NUM_OBJECTS];
  int cx[NUM_OBJECTS];
  int cy[NUM_OBJECTS];

#define NUM_COLORS (NUM_OBJECTS / 4) 
  rgb_color color[NUM_COLORS];


  while(true){
    int num_rects = random(MAX_RECTS - MIN_RECTS + 1) + MIN_RECTS; 
    
    for(int i = 0; i < num_rects * 4; i++){
      angle[i] = random_angle();
      radius[i] = random_radius(); // * 1.25;
      astep[i] = ((random_float() * (MAX_STEP - MIN_STEP)) + MIN_STEP) * (random(2) == 0 ? 1 : -1);
      lx[i] = 99;
      ly[i] = 99;
      cx[i] = random_float() * NUM_COLS;
      cy[i] = random_float() * NUM_ROWS;
    }

    for(int i = 0; i < NUM_COLORS; i++){
      color[i] = adjusted_palette[random_color()];
    }

    int change = (random_float() * (MAX_CHANGE - MIN_CHANGE)) + MIN_CHANGE;
    while(change-- > 0){
      fade_fast();
      //fade_slow();
      //erase();
  
      for(int i = 0; i < num_rects; i++){
        int j = i * 4;
        int k = j + 1;
        int l = j + 2;
        int m = j + 3;
//        polor_to_rect(radius[j], angle[j], &x[j], &y[j], cx[j], cy[j]);
//        polor_to_rect(radius[k], angle[k], &x[k], &y[k], cx[k], cy[k]);
//        polor_to_rect(radius[l], angle[l], &x[l], &y[l], cx[l], cy[l]);
//        polor_to_rect(radius[m], angle[m], &x[m], &y[m], cx[m], cy[m]);
        polor_to_rect(radius[j], angle[j], &x[j], &y[j]);
        polor_to_rect(radius[k], angle[k], &x[k], &y[k]);
        polor_to_rect(radius[l], angle[l], &x[l], &y[l]);
        polor_to_rect(radius[m], angle[m], &x[m], &y[m]);
  
        int ax1 = (x[j] + x[k]) / 2;
        int ay1 = (y[j] + y[k]) / 2;
        int ax2 = (x[l] + x[m]) / 2;
        int ay2 = (y[l] + y[m]) / 2;
  
        line(color[i], ax1, ay1, ax2, ay2);
        //box(color[i], ax1, ay1, ax2, ay2);

        //draw(white, ax1, ay1);
        //draw(white, ax2, ay2);

//        draw(color[i], x[j], y[j]);
//        draw(color[i], x[k], y[k]);
//        draw(color[i], x[l], y[l]);
//        draw(color[i], x[m], y[m]);

        line(color[i], x[j], y[j], x[k], y[k]);
        line(color[i], x[l], y[l], x[m], y[m]);

        angle[j] = add_angle(angle[j], astep[j]);
        angle[k] = add_angle(angle[k], astep[k]);
        angle[l] = add_angle(angle[l], astep[l]);
        angle[m] = add_angle(angle[m], astep[m]);
      }
  
      ledStrip.write(colors, LED_COUNT); 
      delay(DELAY);
    }

    fade_to_black();
  }
}












