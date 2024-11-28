#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"
#include "price_handler.h"
#include "billboards_handler.h"

HT16K33Disp disp1(0x70, 1);

// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

#define BILLBOARD_BUFFER 60
#define PRICE_BUFFER 7
#define NUM_BILLBOARDS 5

#define PRICE_DOWN_TIME 333
#define PRICE_DOWN_STEP 10
#define PRICE_MIN 25
#define PRICE_MAX 995

#define BLANKING_TIME 1000

char billboard_buffer[BILLBOARD_BUFFER];

char current_price[PRICE_BUFFER];

Billboard billboards[NUM_BILLBOARDS] = {
  Billboard(&disp1, billboard_buffer, 10),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1)
};

const char template0[] PROGMEM = "%s"; 
const char template1[] PROGMEM = "    SWEETEN YOUR DAY only %s    "; 
const char template2[] PROGMEM = "    INDULGE YOUR CRAVINGS just %s    "; 
const char template3[] PROGMEM = "    BITE-SIZE BLISS just %s    "; 
const char template4[] PROGMEM = "    SWEET DREAMS START HERE only %s    "; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4};

PriceHandler price_handler(current_price, PRICE_DOWN_TIME, PRICE_DOWN_STEP, PRICE_MIN);
BillboardsHandler billboards_handler(billboard_buffer, NUM_BILLBOARDS, billboards, templates, BLANKING_TIME);

// bool running = false;
// char *current_template = NULL;
// Billboard *current_billboard = NULL;
// byte n_current_billboard = 0;
// bool showing_home = false;
// bool blanking = false;
// unsigned long blanking_until;

void setup() {  
  Serial.begin(115200);

  byte brightness1[3] = { 1, 8, 15 };
  // byte brightness2[1] = {8};
  // byte brightness3[1] = {15};

  Wire.begin();

  disp1.Init(brightness1);
  // disp2.Init(brightness2);
  // disp3.Init(brightness3);

  disp1.clear();
  // disp2.clear();
  // disp3.clear();

  delay(1000);
  
  // price_handler.begin(PRICE_MIN);
  price_handler.begin(PRICE_MAX);
  price_handler.refresh_price();      
}

// void update_billboard() {
//   sprintf_P(billboard_buffer, (char *)pgm_read_ptr(&(templates[n_current_billboard])), current_price);
// }

void loop() {
  unsigned long time = millis();

  if(price_handler.step()){
    price_handler.refresh_price();      
    billboards_handler.update_billboard(current_price);
  }

  billboards_handler.run(time, &disp1, current_price);  

  // // current billboard just finished, time to blank
  // if (!running && !blanking) {
  //   disp1.clear();
  //   blanking = true;
  //   blanking_until = time + BLANKING_TIME;
  //   return;
  // }

  // // process end of blanking
  // if(blanking) {
  //   if (time > blanking_until)
  //     blanking = false;
  //   else
  //     return;
  // }

  // // process start of new billboard
  // if (!running) {
  //   if (!showing_home) {
  //     showing_home = true;
  //     n_current_billboard = 0;
  //   } else {
  //     showing_home = false;
  //     n_current_billboard = random(1, NUM_BILLBOARDS);
  //   }

  //   current_billboard = &billboards[n_current_billboard];
  //   update_billboard();
  //   current_billboard->begin();
  // }

  // running = current_billboard->step();
}
