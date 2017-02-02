#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Attach a 10k resistor between aref & A0, and a photoresistor between A0 & GND
#define LIGHT_SENSOR_PIN A0
#define RANDOM_SEED_PIN A1

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 38
#define MAX_LED (LED_COUNT)
rgb_color colors[LED_COUNT];

// How many parallel effects are running
#define NUM_SETS 2

// How many color dots per effect
#define NUM_OBJECTS 5

#define VLED_COUNT (LED_COUNT / NUM_SETS)
#define MAX_VLED (VLED_COUNT)

// Bit flags for collision-detection
unsigned int existence[NUM_SETS][LED_COUNT];
unsigned int bitmask[16] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};

// Slow down the effects by increasing the delay
#define DELAY 0

// What perfectage the colors reduce each frame if using fade()
#define FADE_RATE 0.50

// Automatic brightness minimum and maximum setting
#define MIN_BRIGHTNESS_PERCENT 10
#define MAX_BRIGHTNESS_PERCENT 100
#define BRIGHTNESS_PERCENT_RANGE (MAX_BRIGHTNESS_PERCENT - MIN_BRIGHTNESS_PERCENT)
#define DEFAULT_BRIGHTNESS_PERCENT MIN_BRIGHTNESS_PERCENT

// Automatic brightness ligth samples
#define LIGHT_SAMPLE_COUNT 5
#define LIGHT_SAMPLE_DELAY 10
int light_samples[LIGHT_SAMPLE_COUNT] = {0,0,0,0,0};

// Defines the quantum of random numbers for speed using random_dir() 
#define RESOLUTION 1000000.0

// The minimum and maximum randomly choosen speed in dots per frame
#define MIN_SPEED 0.01
#define MAX_SPEED 0.05
#define SPEED MIN_SPEED

// Used to interpret and adjust brightness on standard colors
#define BRIGHTNESS_DIVISOR 20.0

// Standard colors
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
rgb_color palette[NPALETTE] = { red, yellow, green, blue, purple, orange };

// Alternate Palettes
//rgb_color palette[NPALETTE] = { red, green, blue, orange, yellow, purple, cyan, magenta, grue, eurg,  egnaro, rurple, white, gray, black }; 
//rgb_color palette[NPALETTE] = { pink, blue, orange, seafoam, white, magenta, ltblue, ltgreen,  purple, red, green, yellow, cyan, gray, black }; 
//rgb_color palette[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 

// Holds the palette of brightness-adjusted colors
rgb_color adjusted_palette[NPALETTE];

// Adjust the brightness level for an individual color
rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

// Set the brightness-adjusted palette colors to a certain brightness
void set_brightness(int brightness_percent = DEFAULT_BRIGHTNESS_PERCENT){
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

float random_dir(){
  float result = ((random(RESOLUTION) / RESOLUTION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
  return random(2) == 1 ? result : result * -1.0;
}

int random_color(){
  return random(NPRETTY_COLORS);
}

// Is this position occupied by another object?
bool position_occupied(int pos, int set, int id){
  return (existence[set][pos] & ~bitmask[id]) == 0 ? false : true;
}

// The heart of the bouncing ball algorithm
float reflect_f(float *f, float *d, int set, int id, float mx = 1.0, float mn = 0.0){
  float next = *f + *d;
  int nexti = int(next);
  bool clash =  position_occupied(nexti, set, id);
  
  if(next > mx || next < mn || clash){
    *d *= -1.0;
  }
  *f += *d;

  return clash;
}

// Math operations on colors 
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

// Compute a random seed by sampling an unconnected analog input pin 
int random_seed(){
  int seed = analogRead(RANDOM_SEED_PIN);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}

// Set an object as occupying a position
void occupy_position(int pos, int set, int id){
  existence[set][pos] |= bitmask[id];
}

// Remove an object from a position
void vacate_position(int pos, int set, int id){
  existence[set][pos] &= ~bitmask[id];
}

// Render an object
void draw(rgb_color color, int pos, int set, int id){
  //int warped = WARPED[pos];
  int final_pos = pos + (set * VLED_COUNT);
  colors[final_pos] = add_color(colors[final_pos], color);
  occupy_position(pos, set, id);
}

// Unrender an object
void undraw(rgb_color color, int pos, int set, int id){
  //int warped = WARPED[pos];
  //colors[warped] = subtract_color(colors[warped], color);
  vacate_position(pos, set, id);
}

void setup()
{
  random_seed();
  setup_colors(false);
  set_brightness();

  for(int j = 0; j < NUM_SETS; j++){
    for(int i = 0; i < VLED_COUNT; i++){
       existence[j][i] = 0;
    }
  }
}

void loop()
{
  auto_adjust_brightness();

  float x[NUM_SETS][NUM_OBJECTS];
  float dx[NUM_SETS][NUM_OBJECTS];
  int lx[NUM_SETS][NUM_OBJECTS];

  for(int j = 0; j < NUM_SETS; j++){
    for(int i = 0; i < NUM_OBJECTS; i++){
      x[j][i] = i * (VLED_COUNT / NUM_OBJECTS);
      dx[j][i] = random_dir(); 
      lx[j][i] = -1;
    }
  }

  // Store colors chosen for each set of objects
  rgb_color object_colors[NUM_SETS][NUM_OBJECTS];

  // In this case choose rainbow colors
  for(int j = 0; j < NUM_SETS; j++){
    for(int i = 0; i < NUM_OBJECTS; i++){
      // Mirror the colors between the two sides
      object_colors[j][i] = adjusted_palette[j > 0 ? (NUM_OBJECTS-1) - i : i];
    }
  }
  
  while(true){
    fade_fast();

    //fade();
    //erase();

    bool clash;
  
    for(int set = 0; set < NUM_SETS; set++){
      for(int i = 0; i < NUM_OBJECTS; i++){

        int id = i + set * NUM_OBJECTS;
        clash = reflect_f(&x[set][i], &dx[set][i], set, id, MAX_VLED);
  
        int tpos = int(x[set][i]);

          undraw(object_colors[set][i], lx[set][i], set, id);
          draw(object_colors[set][i], tpos, set, id);
          lx[set][i] = tpos;
      }
    }

    ledStrip.write(colors, LED_COUNT); 
    delay(DELAY);
  }
}

int get_average_light_level(){
  int result;
  for(int i = 0; i < LIGHT_SAMPLE_COUNT; i++){
    result = get_next_light_sample();  
  }
}

int get_next_light_sample(){
  int accum = 0;
  for(int i = LIGHT_SAMPLE_COUNT-1; i > 0; i--){
    light_samples[i] = light_samples[i-1];
    accum += light_samples[i];
  }
  light_samples[0] = get_one_light_sample();
  accum += light_samples[0];

  return accum / LIGHT_SAMPLE_COUNT;
}

int get_one_light_sample() {
  delay(LIGHT_SAMPLE_DELAY);
  return analogRead(LIGHT_SENSOR_PIN);  
}

int auto_adjust_brightness(){
  int light_level = get_average_light_level();

  // 1023 = complete darkness
  // 0 = infinite brightness
  
  float detected_brightness = (1023.0 - light_level) / 1024.0;
  int ranged_brightness_percent = int(BRIGHTNESS_PERCENT_RANGE * detected_brightness);

  set_brightness(MIN_BRIGHTNESS_PERCENT + ranged_brightness_percent);
}


