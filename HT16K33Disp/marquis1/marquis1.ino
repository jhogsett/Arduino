//----------------------------------------
// Count form 00 to 99 on HT16K33 Display
// using HT16K33Disp library (Ver 1.0)
// by: Anas Kuzechie (May 03, 2022)
//----------------------------------------
#include <Wire.h>
#include <HT16K33Disp.h>
//-----------------------
HT16K33Disp disp1(0x70, 3);
// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);
//------------------------------------------------
void setup()
{
  byte brightness1[3] = {1, 8, 15};
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
#define PRICE_DOWN_FACTOR 0.9
#define PRICE_DOWN_MIN 25
#define TEMPLATE "ENJOY CANDY ONLY %s "

int _price = 0;
unsigned long _next_price_down = 0;

  // dtostrf(purse_f, 4, 2, buf2);

void display_price(int price, char *buffer){
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
    _price *= PRICE_DOWN_FACTOR;

    if(_price < PRICE_DOWN_MIN){
      _price = PRICE_DOWN_MIN;  

    } else 
      _next_price_down = time + PRICE_DOWN_TIME;

    return true;
    
  } else
    return false; 
}

void update_price(char * buffer){
  char buf[7];
  display_price(_price, buf);
  sprintf(buffer, TEMPLATE, buf);
  disp1.update_scroll_string(buffer);
}


bool running1 = false; //, running2 = false, running3 = false;
char buffer[50];
void loop()
{
  // disp1.scroll_string("-abcdefghijklmno- ", 2000, 500);

  disp1.show_string("abcd", false, false);
  delay(200);

  disp1.show_string("effg", false, false);
  delay(200);

  disp1.show_string("hcbi", false, false);
  delay(200);
  
  // unsigned long time = millis();

  // if(!_price){
  //   begin_price_down(2500);
  //   update_price(buffer);
  // }
  
  // if(step_price_down()){  
  //   update_price(buffer);
  // }

  // if(!running1)
  //   disp1.begin_scroll_string(buffer);

  // running1 = disp1.step_scroll_string();
}