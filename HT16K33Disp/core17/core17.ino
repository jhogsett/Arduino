#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"
#include "price_handler.h"
#include "billboards_handler.h"
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

char billboard_buffer[BILLBOARD_BUFFER];

char current_price[PRICE_BUFFER];

const char template0[] PROGMEM = "%s"; 
const char template1[] PROGMEM = "    SWEETEN YOUR DAY only %s    "; 
const char template2[] PROGMEM = "    INDULGE YOUR CRAVINGS just %s    "; 
const char template3[] PROGMEM = "    BITE-SIZE BLISS just %s    "; 
const char template4[] PROGMEM = "    SWEET DREAMS START HERE only %s    "; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4};

PriceHandler price_handler(current_price, PRICE_DOWN_TIME, PRICE_DOWN_STEP, PRICE_MIN);

BillboardsHandler billboards_handler1(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);
BillboardsHandler billboards_handler2(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);
BillboardsHandler billboards_handler3(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);

void setup() {  
  // Serial.begin(115200);
  randomizer.randomize();

  byte brightness1[3] = { 1, 8, 15 };
  byte brightness2[1] = {8};
  byte brightness3[1] = {15};

  Wire.begin();

  disp1.init(brightness1);
  disp2.init(brightness2);
  disp3.init(brightness3);

  delay(1000);

  price_handler.begin(PRICE_MIN);
  // price_handler.begin(PRICE_MAX);
  price_handler.refresh_price();      
  billboards_handler1.update_buffer(current_price);
}

bool boost=true;

unsigned long next_frame = millis();
#define DURATION 1000

bool running1, running2, running3 = false;

void loop() {
  unsigned long time = millis();

  // if(time >= next_frame){
  //   next_frame = time + DURATION;

  // if(boost){
  //   price_handler.boost_price(2, PRICE_MAX);
  //   // begin(PRICE_MAX);
  //   price_handler.refresh_price();      
  //   // billboards_handler1.update_buffer(current_price);
  //   // billboards_handler2.update_buffer(current_price);
  //   boost = false;
  // }

  // if(price_handler.step()){
  //   price_handler.refresh_price();      
  //   // billboards_handler1.update_buffer(current_price);
  // }

  // billboards_handler1.run(time, &disp1, current_price);  
  // billboards_handler2.run(time, &disp2, current_price); 

    // itoa(millis(), billboard_buffer, 16);
    // disp1.scroll_string("TEST1234", 100, 100);
    // disp2.scroll_string("TEST1234", 100, 100);
    // disp3.scroll_string("TEST1234", 100, 100);

    if(!running1)
      disp1.begin_scroll_string("    SHIT    FUCK    PISS    DAMN    FART    POOP    ", 33, 33);

    if(!running2)
      disp2.begin_scroll_string("    SHIT    FUCK    PISS    DAMN    FART    POOP    ", 33, 33);

    if(!running3)
      disp3.begin_scroll_string("    SHIT    FUCK    PISS    DAMN    FART    POOP    ", 33, 33);

    running1 = disp1.step_scroll_string(time);
    running2 = disp2.step_scroll_string(time);
    running3 = disp3.step_scroll_string(time);

    // itoa(millis(), billboard_buffer, 16);
    // disp2.show_string(billboard_buffer);

    // itoa(millis(), billboard_buffer, 16);
    // disp3.show_string(billboard_buffer);



  // }
}
