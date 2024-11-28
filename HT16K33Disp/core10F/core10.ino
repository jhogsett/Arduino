#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"
#include "price_handler.h"
#include "billboards_handler.h"

HT16K33Disp disp1(0x70, 1);
HT16K33Disp disp2(0x71, 1);
HT16K33Disp disp3(0x72, 1);

#define BILLBOARD_BUFFER 60
#define PRICE_BUFFER 7
#define NUM_BILLBOARDS 2

#define PRICE_DOWN_TIME 333
#define PRICE_DOWN_STEP 10
#define PRICE_MIN 25
#define PRICE_MAX 995

#define BLANKING_TIME 1000

char billboard_buffer1[BILLBOARD_BUFFER];
char billboard_buffer2[BILLBOARD_BUFFER];
// char billboard_buffer3[BILLBOARD_BUFFER];

char current_price[PRICE_BUFFER];

Billboard billboards1[NUM_BILLBOARDS] = {
  Billboard(&disp1, billboard_buffer1, 10),
  // Billboard(&disp1, billboard_buffer1, 1),
  // Billboard(&disp1, billboard_buffer1, 1),
  // Billboard(&disp1, billboard_buffer1, 1),
  Billboard(&disp1, billboard_buffer1, 1)
};

Billboard billboards2[NUM_BILLBOARDS] = {
  Billboard(&disp2, billboard_buffer2, 10),
  // Billboard(&disp2, billboard_buffer2, 1),
  // Billboard(&disp2, billboard_buffer2, 1),
  // Billboard(&disp2, billboard_buffer2, 1),
  Billboard(&disp2, billboard_buffer2, 1)
};

// Billboard billboards3[NUM_BILLBOARDS] = {
//   Billboard(&disp3, billboard_buffer3, 10),
//   Billboard(&disp3, billboard_buffer3, 1),
//   Billboard(&disp3, billboard_buffer3, 1),
//   Billboard(&disp3, billboard_buffer3, 1),
//   Billboard(&disp3, billboard_buffer3, 1)
// };

const char template0[] PROGMEM = "%s"; 
const char template1[] PROGMEM = "    ONLY %s    "; 
// const char template2[] PROGMEM = "    INDULGE YOUR CRAVINGS just %s    "; 
// const char template3[] PROGMEM = "    BITE-SIZE BLISS just %s    "; 
// const char template4[] PROGMEM = "    SWEET DREAMS START HERE only %s    "; 
const char *const templates1[] PROGMEM = {template0, template1}; //, template2, template3, template4};
// const char *const templates2[] PROGMEM = {template2, template3};
// const char *const templates2[] PROGMEM = {template0, template1, template2, template3, template4};
// const char *const templates3[] PROGMEM = {template0, template1, template2, template3, template4};

PriceHandler price_handler(current_price, PRICE_DOWN_TIME, PRICE_DOWN_STEP, PRICE_MIN);
BillboardsHandler billboards_handler1(billboard_buffer1, NUM_BILLBOARDS, billboards1, templates1, BLANKING_TIME);
// BillboardsHandler billboards_handler2(billboard_buffer2, NUM_BILLBOARDS, billboards2, templates2, BLANKING_TIME);
// BillboardsHandler billboards_handler3(billboard_buffer3, NUM_BILLBOARDS, billboards3, templates3, BLANKING_TIME);

void setup() {  
  // Serial.begin(115200);

  // byte brightness1[3] = { 1, 8, 15 };
  byte brightness1[1] = { 1 };
  byte brightness2[1] = {8};
  byte brightness3[1] = {15};

  Wire.begin();

  disp1.Init(brightness1);
  disp2.Init(brightness2);
  disp3.Init(brightness3);

  disp1.clear();
  disp2.clear();
  disp3.clear();

  delay(1000);
  
  // price_handler.begin(PRICE_MIN);
  price_handler.begin(PRICE_MAX);
  price_handler.refresh_price();      
}

byte i = 0;
void loop() {
  unsigned long time = millis();

  // i = i++ % 3;  

  // if(price_handler.step()){
  //   price_handler.refresh_price();      
  //   // billboards_handler1.update_billboard(current_price);
  //   // billboards_handler2.update_billboard(current_price);
  //   // billboards_handler3.update_billboard(current_price);
  // }

  switch(i){
    case 0:
      billboards_handler1.run(time, &disp1, "asdf");  
      break;
    case 1:
      // int i = 1 / 0;
      // billboards_handler2.run(0, NULL, "");  
      break;
    case 2:
      // billboards_handler3.run(time, &disp3, current_price);  
      break;
  }
}
