
#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>
#include "billboard.h"
#include "billboards_handler.h"
#include "led_handler.h"

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

#define FIRST_LED 6
#define LAST_LED 11
#define NUM_LEDS 6

#define FIRST_PANEL_LED 9
#define LAST_PANEL_LED 11
#define NUM_PANEL_LEDS 3

#define FIRST_BUTTON_LED 6
#define LAST_BUTTON_LED 8
#define NUM_BUTTON_LEDS 3

#define FIRST_BUTTON 2
#define LAST_BUTTON 5
#define NUM_BUTTONS 4

#define ANY_BUTTON 2
#define GREEN_BUTTON 3
#define YELLOW_BUTTON 4
#define RED_BUTTON 5

#define FIRST_PLAY_BUTTON 3
#define NUM_PLAY_BUTTONS 3

#define GREEN_LED 9
#define YELLOW_LED 10
#define RED_LED 11

#define GREEN_BUTTON_LED 6
#define YELLOW_BUTTON_LED 7
#define RED_BUTTON_LED 8

#define ANY_COLOR_ID 0
#define FIRST_COLOR_ID 1
#define LAST_COLOR_ID 3
#define NUM_COLOR_IDS 3
#define GREEN_ID 1
#define YELLOW_ID 2
#define RED_ID 3
#define NO_COLOR_ID -1

#define DISPLAY_BUFFER 101
#define INT_BUFFER 10

#define DEFAULT_BLANK_TIME 500
#define DEFAULT_SHOW_TIME 500

#define BUTTON_WAIT_TIMEOUT 5000

#define SCROLL_SHOW 500
#define SCROLL_DELAY 100

#define DEFAULT_PANEL_LED_INTENSITY 16

#define BILLBOARD_BUFFER 60
#define BLANKING_TIME 750
#define HOME_TIMES 3

char billboard_buffer[BILLBOARD_BUFFER];

#define NUM_BILLBOARDS 2
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "PRESS ANY BUTTON TO START            "; 
const char *const templates[] PROGMEM = {template0, template1};

BillboardsHandler billboards_handler(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);

char buffer[DISPLAY_BUFFER];
char int_buffer[INT_BUFFER];
bool button_states[NUM_BUTTONS];
char *color_names[] = {"Any", "Green", "Amber", "Red"};

HT16K33Disp display(0x70, 3);

byte led_intensities[] = {0, 0, 0, 32, 40, 32};

LEDHandler all_leds(FIRST_LED, 6, led_intensities);
// LEDHandler panel_leds(FIRST_PANEL_LED, 3, led_intensities + 3);
// LEDHandler button_leds(FIRST_BUTTON_LED, 3, led_intensities);

// byte intensities[] = {72, 48, 72};
// LEDHandler leds(FIRST_PANEL_LED, 3, intensities);

void setup_display(){
  Wire.begin();
  byte brightness[3] = {1, 9, 15};
  display.Init(brightness); 
  display.clear();
}

void setup_leds(){
  for(byte i = FIRST_LED; i <= LAST_LED; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  unsigned long time = millis();
  // panel_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, 750, 350);
  // button_leds.begin(time, LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING, 800, 400);
  all_leds.begin(time, LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING | LEDHandler::STYLE_MIRROR, 1000, 1000);
}

void setup_buttons(){
  for(byte i = FIRST_BUTTON; i <= LAST_BUTTON; i++){
    pinMode(i, 0x03); // INPUT_PULLDOWN ?!
  }
}

char * load_f_string(const __FlashStringHelper* f_string){
  const char *p = (const char PROGMEM *)f_string;
  return strcpy_P(buffer, p);
}

void show_string(char *string, int blank_time = DEFAULT_BLANK_TIME, int show_time = DEFAULT_SHOW_TIME){
  display.scroll_string(string, SCROLL_SHOW, SCROLL_DELAY);
  if(show_time != 0){
    delay(show_time);
  }
  if(blank_time != 0){
    display.clear();
    delay(blank_time);
  }
}

void show_f_string(const __FlashStringHelper* f_string, int blank_time = DEFAULT_BLANK_TIME, int show_time = DEFAULT_SHOW_TIME){
  show_string(load_f_string(f_string), blank_time, show_time);
}

void show_f_string_data(const __FlashStringHelper* f_string, char *data, int blank_time = DEFAULT_BLANK_TIME, int show_time = DEFAULT_SHOW_TIME){
  const char *p = (const char PROGMEM *)f_string;
  sprintf_P(buffer, p, data);
  show_string(buffer, blank_time, show_time);
}

void show_f_string_data(const __FlashStringHelper* f_string, int data, int blank_time = DEFAULT_BLANK_TIME, int show_time = DEFAULT_SHOW_TIME){
  const char *p = (const char PROGMEM *)f_string;
  sprintf_P(buffer, p, data);
  show_string(buffer, blank_time, show_time);
}

byte color_id_to_button_led_pin(byte color_id){
  return color_id + FIRST_BUTTON_LED - 1;
}

byte color_id_to_panel_led_pin(byte color_id){
  return color_id + FIRST_PANEL_LED - 1;
}

void show_button_led(byte color_id, bool show=true){
  if(color_id == ANY_COLOR_ID){
    show_button_led(GREEN_ID, show);
    show_button_led(YELLOW_ID, show);
    show_button_led(RED_ID, show);
  }
  else
    digitalWrite(color_id_to_button_led_pin(color_id), show);
}

void show_panel_led(byte color_id, byte intensity = DEFAULT_PANEL_LED_INTENSITY){
  if(color_id == ANY_COLOR_ID){
    show_panel_led(GREEN_ID, intensity);
    show_panel_led(YELLOW_ID, intensity);
    show_panel_led(RED_ID, intensity);
  }
  else
    analogWrite(color_id_to_panel_led_pin(color_id), intensity);
}

void button_states_to_panel_leds(byte intensity = DEFAULT_PANEL_LED_INTENSITY){
  for(byte i = FIRST_COLOR_ID; i <= LAST_COLOR_ID; i++){
    show_panel_led(i, button_states[i] ? intensity : 0);    
  }
}

void setup(){
  // int i = 1/0;
  Serial.begin(115200);

  randomizer.randomize();
 
  setup_leds();
  setup_display();
  setup_buttons();
  // delay(1000);

}

// #define TEMPLATE "PLAY THE REACTION TIME GAME            "

// bool running1 = false; //, running2 = false, running3 = false;

int payout = 0;

// todo de-bounce

// wait on a specific button
bool wait_on_button(byte button, int wait_time = BUTTON_WAIT_TIMEOUT){
  unsigned long time_out = millis() + wait_time;
  while(millis() < time_out){
    if(digitalRead(button) == HIGH)
      return true;
  }
  return false;  
}

// wait on any button and record state
bool wait_on_any_button(int wait_time = BUTTON_WAIT_TIMEOUT){
  for(byte i = 0; i < NUM_BUTTONS; i++){
    button_states[i] = false;
  }

  if(wait_on_button(ANY_BUTTON, wait_time)){
    button_states[ANY_COLOR_ID] = true;
    button_states[GREEN_ID] = digitalRead(GREEN_BUTTON);
    button_states[YELLOW_ID] = digitalRead(YELLOW_BUTTON);
    button_states[RED_ID] = digitalRead(RED_BUTTON);
    return true;
  }  
  return false;  
}

// look for a specific button but accept any press
int look_for_button(byte color_id, int wait_time = BUTTON_WAIT_TIMEOUT, bool exclusive = false){
  if(wait_on_any_button(wait_time)){
    if(button_states[color_id])
      return color_id;
    else
      return ANY_COLOR_ID;
  }
  return NO_COLOR_ID;  
}

void loop()
{
  unsigned long time = millis();

  // Serial.println("asdf");
    
  // panel_leds.step(time);
  // button_leds.step(time);
  all_leds.step(time);
  billboards_handler.run(time, &display, NULL);  

}

















  
  
  // if(++i == 2500){
  //   i = 0;
  //   byte led = random(6, 12);
  //   byte on = random(2);
  // // pinMode(led, OUTPUT);
  //   digitalWrite(led, on == 1 ? HIGH : LOW);
  // }
  
  // unsigned long time = millis();
  
  // if(!running1)
  //   display.begin_scroll_string(buffer);

  // running1 = display.step_scroll_string(time);
// }