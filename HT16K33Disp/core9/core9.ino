// with code from: Anas Kuzechie (May 03, 2022)

#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"
#include "price_handler.h"

HT16K33Disp disp1(0x70, 1);

// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

#define BILLBOARD_BUFFER 60
char billboard_buffer[BILLBOARD_BUFFER];

#define PRICE_BUFFER 7
char current_price[PRICE_BUFFER];

#define NUM_BILLBOARDS 5
Billboard billboards[NUM_BILLBOARDS] = {
  Billboard(&disp1, billboard_buffer, 10),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1),
  Billboard(&disp1, billboard_buffer, 1)
};

#define PRICE_DOWN_TIME 333
#define PRICE_DOWN_STEP 10
#define PRICE_MIN 25
#define PRICE_MAX 995

PriceHandler price_handler(current_price, PRICE_DOWN_TIME, PRICE_DOWN_STEP, PRICE_MIN);




// #define TEMPLATE0 "%s"
// #define TEMPLATE1 "    SWEETEN your DAY only %s    "
// #define TEMPLATE2 "    INDULGE your CRAVINGS just %s    "
// #define TEMPLATE3 "    BITE-SIZE BLISS just %s    "
// #define TEMPLATE4 "    SWEET DREAMS START HERE only %s    "

#define BLANKING_TIME 1000

const char template0[] PROGMEM = "%s"; 
const char template1[] PROGMEM = "    SWEETEN YOUR DAY only %s    "; 
const char template2[] PROGMEM = "    INDULGE YOUR CRAVINGS just %s    "; 
const char template3[] PROGMEM = "    BITE-SIZE BLISS just %s    "; 
const char template4[] PROGMEM = "    SWEET DREAMS START HERE only %s    "; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4};

bool running = false;

char *current_template = NULL;
Billboard *current_billboard = NULL;
byte n_current_billboard = 0;
bool showing_home = false;
bool blanking = false;
unsigned long blanking_until;
bool discounting = false;

void setup() {
  // Serial.begin(115200);

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

// int _price = 0;
// unsigned long _next_price_down = 0;

// void format_price(int price, char *billboard_buffer) {
//   int dollars = price / 100;
//   char cents = price - (dollars * 100);
//   if (dollars > 10)
//     sprintf(billboard_buffer, "$%2d.%02d", dollars, cents);
//   else if (dollars > 0)
//     sprintf(billboard_buffer, "$%1d.%02d", dollars, cents);
//   else
//     sprintf(billboard_buffer, "$ .%02d", cents);
// }

// void begin_price_down(int price) {
//   _price = price;
//   _next_price_down = millis() + PRICE_DOWN_TIME;
// }

// bool step_price_down() {
//   unsigned long time = millis();

//   if (time >= _next_price_down) {
//     _price -= PRICE_DOWN_AMT;

//     if (_price < PRICE_DOWN_MIN) {
//       _price = PRICE_DOWN_MIN;

//     } else
//       _next_price_down = time + PRICE_DOWN_TIME;

//     return true;

//   } else
//     return false;
// }


// void update_price() {
//   format_price(_price, current_price);
// }

void update_billboard() {
  // char * tbuffer[50];
  // char *t;
  // switch (ntemplate) {
  //   case 0:
  //     t = TEMPLATE0;
  //     break;
  //   case 1:
  //     t = TEMPLATE1;
  //     break;
  //   case 2:
  //     t = TEMPLATE2;
  //     break;
  //   case 3:
  //     t = TEMPLATE3;
  //     break;
  //   case 4:
  //     t = TEMPLATE4;
  //     break;
  // }
  sprintf_P(billboard_buffer, (char *)pgm_read_ptr(&(templates[n_current_billboard])), current_price);
}

void loop() {
  unsigned long time = millis();

    if(price_handler.step()){
      price_handler.refresh_price();      
      update_billboard();
    }
  // }

  // if (!_price) {
  //   begin_price_down(PRICE_MAX);
  //   update_price();
  //   update_billboard(n_current_billboard);
  // }

  // if (step_price_down()) {
  //   update_price();
  //   update_billboard(n_current_billboard);
  // }

  // current billboard just finished, time to blank
  if (!running && !blanking) {
    disp1.clear();
    blanking = true;
    blanking_until = time + BLANKING_TIME;
    return;
  }

  // process end of blanking
  if(blanking) {
    if (time > blanking_until)
      blanking = false;
    else
      return;
  }

  // process start of new billboard
  if (!running) {
    if (!showing_home) {
      showing_home = true;
      n_current_billboard = 0;
    } else {
      showing_home = false;
      n_current_billboard = random(1, NUM_BILLBOARDS);
    }

    current_billboard = &billboards[n_current_billboard];
    update_billboard();
    current_billboard->begin();
  }

  running = current_billboard->step();
}
