#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

HT16K33Disp disp1(0x70, 1);
HT16K33Disp disp2(0x71, 1);
HT16K33Disp disp3(0x72, 1);

#define BILLBOARD_BUFFER 60
#define PRICE_BUFFER 7
#define NUM_BILLBOARDS 5

#define PRICE_DOWN_TIME 60000
#define PRICE_DOWN_STEP 5
#define PRICE_MIN 25
#define PRICE_MAX 995

#define BLANKING_TIME 1000
#define HOME_TIMES 1

void setup() {  
  randomizer.randomize();

  byte brightness1[3] = { 1, 8, 15 };
  byte brightness2[1] = {8};
  byte brightness3[1] = {15};

  Wire.begin();

  disp1.init(brightness1);
  disp2.init(brightness2);
  disp3.init(brightness3);
}

bool running1, running2, running3 = false;

void loop() {
  unsigned long time = millis();

    if(!running1)
      disp1.begin_scroll_string("0x70 WORKS    ", 100, 100);

    if(!running2)
      disp2.begin_scroll_string("0x71 WORKS    ", 100, 100);

    if(!running3)
      disp3.begin_scroll_string("0x72 WORKS    ", 100, 100);

    running1 = disp1.step_scroll_string(time);
    running2 = disp2.step_scroll_string(time);
    running3 = disp3.step_scroll_string(time);
}
