// with code from: Anas Kuzechie (May 03, 2022)

#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"

HT16K33Disp disp1(0x70, 1);

// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

#define MAX_BUFFER 60
char buffer[MAX_BUFFER];

#define NUM_BILLBOARDS 5
Billboard billboards[NUM_BILLBOARDS] = {
  Billboard(&disp1, buffer, 10),
  Billboard(&disp1, buffer, 1),
  Billboard(&disp1, buffer, 1),
  Billboard(&disp1, buffer, 1),
  Billboard(&disp1, buffer, 1)
};

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
}

#define PRICE_DOWN_TIME 5000
#define PRICE_DOWN_AMT 25
#define PRICE_DOWN_MIN 25
#define PRICE_MAX 975

#define TEMPLATE0 "%s"
#define TEMPLATE1 "    SWEETEN your DAY only %s    "
#define TEMPLATE2 "    INDULGE your CRAVINGS just %s    "
#define TEMPLATE3 "    BITE-size BLISS just %s    "
#define TEMPLATE4 "    SWEET DREAMS START HERE only %s    "

#define BLANKING_TIME 1000


const char template0[] PROGMEM = TEMPLATE0; 
const char template1[] PROGMEM = TEMPLATE1; 
const char template2[] PROGMEM = TEMPLATE2; 
const char template3[] PROGMEM = TEMPLATE3; 
const char template4[] PROGMEM = TEMPLATE4; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4};


int _price = 0;
unsigned long _next_price_down = 0;

void format_price(int price, char *buffer) {
  int dollars = price / 100;
  char cents = price - (dollars * 100);
  if (dollars > 10)
    sprintf(buffer, "$%2d.%02d", dollars, cents);
  else if (dollars > 0)
    sprintf(buffer, "$%1d.%02d", dollars, cents);
  else
    sprintf(buffer, "$ .%02d", cents);
}

void begin_price_down(int price) {
  _price = price;
  _next_price_down = millis() + PRICE_DOWN_TIME;
}

bool step_price_down() {
  unsigned long time = millis();

  if (time >= _next_price_down) {
    _price -= PRICE_DOWN_AMT;

    if (_price < PRICE_DOWN_MIN) {
      _price = PRICE_DOWN_MIN;

    } else
      _next_price_down = time + PRICE_DOWN_TIME;

    return true;

  } else
    return false;
}

char current_price[7];


void update_price() {
  format_price(_price, current_price);
}

void update_price_template(byte ntemplate) {
  char * tbuffer[50];
  char *t;
  switch (ntemplate) {
    case 0:
      t = TEMPLATE0;
      break;
    case 1:
      t = TEMPLATE1;
      break;
    case 2:
      t = TEMPLATE2;
      break;
    case 3:
      t = TEMPLATE3;
      break;
    case 4:
      t = TEMPLATE4;
      break;
  }
  sprintf_P(buffer, (char *)pgm_read_ptr(&(templates[ntemplate])), current_price);
}

bool running = false;

char *current_template = NULL;
Billboard *current_billboard = NULL;
byte n_billboard = 0;
bool showing_home = false;
bool blanking = false;
unsigned long blanking_until;

void loop() {
  unsigned long time = millis();

  if (!_price) {
    begin_price_down(PRICE_MAX);
    update_price();
    update_price_template(n_billboard);
  }

  if (step_price_down()) {
    update_price();
    update_price_template(n_billboard);
  }

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
      n_billboard = 0;
    } else {
      showing_home = false;
      n_billboard = random(1, NUM_BILLBOARDS);
    }

    current_billboard = &billboards[n_billboard];
    update_price_template(n_billboard);
    current_billboard->begin();
  }

  running = current_billboard->step();
}
