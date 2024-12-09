// with code from: Anas Kuzechie (May 03, 2022)

#include <Wire.h>
#include <HT16K33Disp.h>
#include "billboard.h"

HT16K33Disp disp1(0x70, 1);

// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

char bbstring1[50];
// char bbstring2[50];
// char bbstring3[50];

Billboard billboard1(&disp1, bbstring1, 10);
Billboard billboard2(&disp1, bbstring1, 1);
Billboard billboard3(&disp1, bbstring1, 1);
Billboard billboard4(&disp1, bbstring1, 1);
Billboard billboard5(&disp1, bbstring1, 1);

void setup(){
  byte brightness1[3] = {1, 8, 15};
  // byte brightness2[1] = {8};
  // byte brightness3[1] = {15};

  Wire.begin();

  disp1.init(brightness1); 
  // disp2.Init(brightness2); 
  // disp3.Init(brightness3); 

  disp1.clear();
  // disp2.clear();
  // disp3.clear();

  delay(1000);
}

#define PRICE_DOWN_TIME 250
#define PRICE_DOWN_AMT 0.01
#define PRICE_DOWN_MIN 25
#define PRICE_MAX 999

#define TEMPLATE1 "%s"
#define TEMPLATE2 "SWEETEN YOUR DAY %s"
#define TEMPLATE3 "INDULGE YOUR CRAVINGS %s"
#define TEMPLATE4 "BITE-SIZE BLISS %s"
#define TEMPLATE5 "SWEET DREAMS START HERE %s"

int _price = 0;
unsigned long _next_price_down = 0;

void format_price(int price, char *buffer){
  int dollars = price / 100;
  char cents = price - (dollars * 100);
  if(dollars > 10)
    sprintf(buffer, "$%2d.%02d", dollars, cents);
  else if(dollars > 0)
    sprintf(buffer, "$%1d.%02d", dollars, cents);
  else
    sprintf(buffer, "$ .%02d", cents);
}

void begin_price_down(int price){
  _price = price;
  _next_price_down = millis() + PRICE_DOWN_TIME;
}

bool step_price_down(){
  unsigned long time = millis();

  if(time >= _next_price_down){
    _price -= PRICE_DOWN_AMT;

    if(_price < PRICE_DOWN_MIN){
      _price = PRICE_DOWN_MIN;  

    } else 
      _next_price_down = time + PRICE_DOWN_TIME;

    return true;
    
  } else
    return false; 
}

char current_price[7];

char * current_buffer = bbstring1;
char * current_template = TEMPLATE1;

void update_price(){
  format_price(_price, current_price);
}

void update_price_template(){
  sprintf(current_buffer, current_template, current_price);
}

bool running = false;

Billboard * current_billboard = NULL;
byte n_billboard = 2;

void loop()
{
  unsigned long time = millis();

  if(!_price){
    begin_price_down(PRICE_MAX);
    update_price();
    update_price_template();
  }
  
  if(step_price_down()){  
    update_price();
    update_price_template();
  }

  if(!running){
    switch(n_billboard){
      case 1:
        n_billboard = 2;
        current_billboard = &billboard2;
        current_template = TEMPLATE2;
        update_price_template();
        break;

      case 2:
        n_billboard = 3;
        current_billboard = &billboard3;
        current_template = TEMPLATE3;
        update_price_template();
        break;

      case 3:
        n_billboard = 4;
        current_billboard = &billboard4;
        current_template = TEMPLATE4;
        update_price_template();
        break;

      case 4:
        n_billboard = 5;
        current_billboard = &billboard5;
        current_template = TEMPLATE5;
        update_price_template();
        break;

      case 5:
        n_billboard = 1;
        current_billboard = &billboard1;
        current_template = TEMPLATE1;
        update_price_template();
        break;
    }
    current_billboard->begin();
  }
  
  running = current_billboard->step();
}
