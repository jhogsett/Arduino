#include <random_seed.h>
#include <hsl_to_rgb.h>

#define RANDOM_SEED_PIN A1
#define PIN_FIRST 9
#define PIN_LAST 11

static RandomSeed<RANDOM_SEED_PIN> randomizer;

void setup() {
  randomizer.randomize();

  for(int j = PIN_FIRST; j <= PIN_LAST; j++)
    pinMode(j, OUTPUT);
}

#define DEL 20
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

// #define RED_INT 10L
// #define GREEN_INT 10L
// #define BLUE_INT 10L

// #define MAX_SPEED 5

int hue = 0;
int sat = 255;
int lit = 255;
int quant = 3;

void loop() {
  for(int i = 0; i <= 359; i++){
    rgb_color rgbc = hsl_to_rgb(i, sat, lit);
    
    // analogWrite(RED_PIN, max(1, rgbc.red));
    // analogWrite(GREEN_PIN, max(1, rgbc.green));
    // analogWrite(BLUE_PIN, max(1, rgbc.blue));

    // analogWrite(RED_PIN, rgbc.red);
    // analogWrite(GREEN_PIN, rgbc.green);
    // analogWrite(BLUE_PIN, rgbc.blue);

    // analogWrite(RED_PIN, (quant + rgbc.red) / quant);
    // analogWrite(GREEN_PIN, (quant + rgbc.green) / quant);
    // analogWrite(BLUE_PIN, (quant + rgbc.blue) / quant);

    analogWrite(RED_PIN, (rgbc.red >> quant) << quant);
    analogWrite(GREEN_PIN, (rgbc.green >> quant) << quant);
    analogWrite(BLUE_PIN, (rgbc.blue >> quant) << quant);

    delay(DEL);
  }
}
