
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
#define AMBER_BUTTON 4
#define RED_BUTTON 5

#define FIRST_PLAY_BUTTON 3
#define NUM_PLAY_BUTTONS 3

#define GREEN_LED 9
#define AMBER_LED 10
#define RED_LED 11

#define GREEN_BUTTON_LED 6
#define AMBER_BUTTON_LED 7
#define RED_BUTTON_LED 8

#define ANY_COLOR_ID 0
#define FIRST_COLOR_ID 1
#define LAST_COLOR_ID 3
#define NUM_COLOR_IDS 3
#define GREEN_ID 1
#define AMBER_ID 2
#define RED_ID 3
#define NO_COLOR_ID -1

#define DISPLAY_BUFFER 101
#define INT_BUFFER 13

#define DEFAULT_BLANK_TIME 500
#define DEFAULT_SHOW_TIME 500

#define BUTTON_WAIT_TIMEOUT 5000

#define SCROLL_SHOW 500
#define SCROLL_DELAY 100

#define DEFAULT_PANEL_LED_INTENSITY 16

#define BILLBOARD_BUFFER 60
#define BLANKING_TIME 750
#define HOME_TIMES 3

#define DEBOUNCE_TIME 10
#define LONG_PRESS_TIME 2000
#define PROMPT_TIMEOUT 10000
#define SLEEP_TIMEOUT 900000

char billboard_buffer[BILLBOARD_BUFFER];

#define NUM_BILLBOARDS 3
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "12-05-24 Version"; 
const char template2[] PROGMEM = "Press A Button To Start"; 
const char *const templates[] PROGMEM = {template0, template1, template2};

BillboardsHandler billboards_handler(billboard_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);

char buffer[DISPLAY_BUFFER];
char int_buffer[INT_BUFFER];
char int_buffer2[INT_BUFFER];
// char *color_names[] = {"Any", "Green", "Amber", "Red"};

volatile bool button_states[NUM_BUTTONS];
volatile unsigned long press_time;

HT16K33Disp display(0x70, 3);
HT16K33Disp disp1(0x70, 1);
HT16K33Disp disp2(0x71, 1);
HT16K33Disp disp3(0x72, 1);

static const byte display_brightnesses[] = {1, 9, 15}; 
static const byte led_intensities[] = {0, 0, 0, 32, 40, 32};

LEDHandler all_leds(FIRST_LED, 6, led_intensities);
LEDHandler panel_leds(FIRST_PANEL_LED, 3, led_intensities + 3);
LEDHandler button_leds(FIRST_BUTTON_LED, 3, led_intensities);

void setup_display(){
  Wire.begin();
  display.init(display_brightnesses); 
  /* the duplicated displays do not need reinitialization
  disp1.init(brightness+0); 
  disp2.init(brightness+1); 
  disp3.init(brightness+2); */
  display.clear();
}

void setup_leds(){
  for(byte i = FIRST_LED; i <= LAST_LED; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  unsigned long time = millis();
  panel_leds.begin(time, LEDHandler::STYLE_RANDOM, 750, 350);
  button_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, 800, 400);
  all_leds.begin(time, LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING | LEDHandler::STYLE_MIRROR, 1000, 1000);
}

void setup_buttons(){
  for(byte i = 0; i < NUM_BUTTONS; i++){
    pinMode(i + FIRST_BUTTON, 0x03); // INPUT_PULLDOWN ?!
    button_states[i] = false;
  }
}

char * load_f_string(const __FlashStringHelper* f_string){
  const char *p = (const char PROGMEM *)f_string;
  return strcpy_P(buffer, p);
}

void setup(){
  Serial.begin(115200);
  randomizer.randomize();
 
  setup_leds();
  setup_display();
  setup_buttons();

  attachInterrupt(digitalPinToInterrupt(ANY_BUTTON), button_pressed_i, RISING);
  button_states[ANY_COLOR_ID] = false;
}

// interupt if any button is pressed
// saves the time pressed for debouncing, long press etc
// saves all the button states
void button_pressed_i(){
  press_time = millis();
  button_states[ANY_COLOR_ID] = true;
  button_states[GREEN_ID] = digitalRead(GREEN_BUTTON);
  button_states[AMBER_ID] = digitalRead(AMBER_BUTTON);
  button_states[RED_ID] = digitalRead(RED_BUTTON);
}

// use in conjunction with the ISR
bool button_pressed(){
  // do nothing if no button has been presssed according to the ISR
  if(!button_states[ANY_COLOR_ID])
    return false;

  // enforce a debounce period
  // if the button is unpressed during this time, cancel the press
  // this depends on being called immediately after the ISR has recorded a press, 
  //   ie. in a loop
  while(millis() < press_time + DEBOUNCE_TIME){
    if(!digitalRead(ANY_BUTTON)){
      button_states[ANY_COLOR_ID] = false;
      return false;
    }
  }

  button_states[ANY_COLOR_ID] = false;
  button_states[GREEN_ID] = digitalRead(GREEN_BUTTON);
  button_states[AMBER_ID] = digitalRead(AMBER_BUTTON);
  button_states[RED_ID] = digitalRead(RED_BUTTON);
  return true;
}

bool button_still_pressed(){
  return digitalRead(ANY_BUTTON);
}

// use in conjunction with button_pressed()
// 0=still waiting, 1=long press, -1=canceled  
int wait_on_long_press(){
  unsigned long target_time = press_time + LONG_PRESS_TIME;

  if(millis() < target_time){
    // still waiting on long press
    if(button_still_pressed())
      return 0;
    else
      return -1;    
  } else {
    // long press time has been met
    if(button_still_pressed())
      return 1;
    else
      return -1;    
  }
}

// bool wait_on_press(){
//   while(button_still_pressed());

//   while(!button_pressed())
//     return false;

//   return true;
// }

//   unsigned long target_time = press_time + LONG_PRESS_TIME;

//   if(millis() < target_time){
//     // still waiting on long press
//     if(button_still_pressed())
//       return 0;
//     else
//       return -1;    
//   } else {
//     // long press time has been met
//     if(button_still_pressed())
//       return 1;
//     else
//       return -1;    
//   }
// }

// int payout = 0;


// // wait on a specific button
// bool wait_on_button(byte button, int wait_time = BUTTON_WAIT_TIMEOUT){
//   unsigned long time_out = millis() + wait_time;
//   while(millis() < time_out){
//     if(digitalRead(button) == HIGH)
//       return true;
//   }
//   return false;  
// }

// // wait on any button and record state
// bool wait_on_any_button(int wait_time = BUTTON_WAIT_TIMEOUT){
//   for(byte i = 0; i < NUM_BUTTONS; i++){
//     button_states[i] = false;
//   }

//   if(wait_on_button(ANY_BUTTON, wait_time)){
//     button_states[ANY_COLOR_ID] = true;
//     button_states[GREEN_ID] = digitalRead(GREEN_BUTTON);
//     button_states[AMBER_ID] = digitalRead(AMBER_BUTTON);
//     button_states[RED_ID] = digitalRead(RED_BUTTON);
//     return true;
//   }  
//   return false;  
// }

// // look for a specific button but accept any press
// int look_for_button(byte color_id, int wait_time = BUTTON_WAIT_TIMEOUT, bool exclusive = false){
//   if(wait_on_any_button(wait_time)){
//     if(button_states[color_id])
//       return color_id;
//     else
//       return ANY_COLOR_ID;
//   }
//   return NO_COLOR_ID;  
// }

// void wait_on_button(byte color_id){

// }

void run_billboard(){
  unsigned long time = millis();
  panel_leds.step(time);
  // button_leds.step(time);
  // all_leds.step(time);
  billboards_handler.run(time, &display, NULL);  
}

// returns -1 on timeout, 1 on long press, 0 otherwise
int billboard_prompt(){
  unsigned long sleep_timeout = millis() + SLEEP_TIMEOUT;
  unsigned long time;

  billboards_handler.reset();
  panel_leds.begin(time, LEDHandler::STYLE_RANDOM, 750, 350);
  while((time = millis()) < sleep_timeout){
    run_billboard();
    // panel_leds.step(time);
    // // button_leds.step(time);
    // // all_leds.step(time);
    // billboards_handler.run(time, &display, NULL);  

    if(button_pressed()){
      all_leds.activate_leds(button_states, true);

      int long_press_state;
      while((long_press_state = wait_on_long_press()) == 0);
      all_leds.deactivate_leds(true);

      if(long_press_state == 1)
        return 1;
      else
        return 0;
    }
  }
  return -1;
}

// returns -1=timed out, 0=long press, button ID otherwise
int prompt(char *prompt){
  unsigned long time;
  unsigned long timeout_time = millis() + PROMPT_TIMEOUT;

  all_leds.deactivate_leds(true);
  display.begin_scroll_loop();
  while((time = millis()) < timeout_time){
    display.loop_scroll_string(time, prompt);    

    if(button_pressed()){
      all_leds.activate_leds(button_states, true);

      int long_press_state;
      while((long_press_state = wait_on_long_press()) == 0);
      all_leds.deactivate_leds(true);

      if(long_press_state == 1)
        return 0;
      else {
        if(button_states[GREEN_ID])
          return GREEN_ID;
        else if(button_states[AMBER_ID])
          return AMBER_ID;
        else if(button_states[RED_ID])
          return RED_ID;
      }
        // return 0;
    }
  }
  return -1;

  //   if(!button_pressed())
  //     continue;

  //   all_leds.activate_leds(button_states, true);

  //   while(button_still_pressed());  
  //   all_leds.deactivate_leds(true);

  //   if(button_states[GREEN_ID])
  //     return GREEN_ID;
  //   else if(button_states[AMBER_ID])
  //     return AMBER_ID;
  //   else if(button_states[RED_ID])
  //     return RED_ID;
  // }

  // return -1;
}

// // returns -1=timed out
// int prompt(char *prompt){
//   unsigned long time;
//   unsigned long timeout_time = millis() + PROMPT_TIMEOUT;

//   all_leds.deactivate_leds(true);
//   display.begin_scroll_loop();
//   while((time = millis()) < timeout_time){
//     display.loop_scroll_string(time, prompt);    

//     if(!button_pressed())
//       continue;

//     all_leds.activate_leds(button_states, true);

//     while(button_still_pressed());  
//     all_leds.deactivate_leds(true);

//     if(button_states[GREEN_ID])
//       return GREEN_ID;
//     else if(button_states[AMBER_ID])
//       return AMBER_ID;
//     else if(button_states[RED_ID])
//       return RED_ID;
//   }

//   return -1;
// }

bool title_prompt(char *title, byte times=1){
  all_leds.deactivate_leds(true);
  display.begin_scroll_loop(times);
  while(display.loop_scroll_string(millis(), title)){
    if(button_pressed()){
      break;
    }
  }
}

int panel_led_prompt(){
  all_leds.deactivate_leds(true);

  panel_leds.begin(millis(), LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING, 1500, 1500);

  while(true){
    panel_leds.step(millis());

    if(!button_pressed())
      continue;

    all_leds.activate_leds(button_states, true);

    while(button_still_pressed());  
    all_leds.deactivate_leds(true);

    return 1;
  }

  return -1;
}

void sleep_mode(){
  display.clear();
  while(panel_led_prompt() == -1);
}

// todo prompt with long press to get back
void settings_mode(){
  title_prompt(load_f_string(F("[ SETTINGS ]")));
  switch(prompt(load_f_string(F("Zzz    Back")))){
    case -1:
      return;
    case 0:
      return;
    case 1:
      sleep_mode();
      return;
    case 2:
      // sleep_mode();
      return;
    case 3:
      // time_game();
      return;
  }

  // while(prompt(load_f_string(F("* OPTIONS"))) == -1);
}

void slots_game(){
  while(prompt(load_f_string(F("Dirty Slots"))) == -1);
}

#define MIN_DELAY 1500
#define MAX_DELAY 5000
#define ROUNDS 3
#define ROUND_DELAY 750

void micros_to_ms(char * buffer, unsigned long micros){
  int ms_dec, ms_frac;
  ms_dec = micros / 1000;
  ms_frac = micros % 1000;
  sprintf(buffer, "%u.%04u", ms_dec, ms_frac);
}

unsigned long best_mean = (unsigned long)-1;

void time_game(){
  title_prompt(load_f_string(F("The Time Game")));

  // title_prompt(load_f_string(F("Second TItle Prompt")));


  // delay(ROUND_DELAY);

  button_leds.activate_all(true);
  while(prompt(load_f_string(F("Press to Start"))) == -1);

  unsigned long mean = 0;

  for(byte i = 0; i < ROUNDS; i++){
    button_leds.activate_all(true);
    display.scroll_string(load_f_string(F("Press on FLASH")));
    delay(ROUND_DELAY);
    display.clear();
    button_leds.activate_all(false);

    int del = random(MIN_DELAY, MAX_DELAY+1);
    delay(del);
    
    unsigned long start_time = micros();
    panel_leds.flash_leds();

    while(digitalRead(ANY_BUTTON) == LOW);
    unsigned long reaction_time = micros() - start_time;
    while(digitalRead(ANY_BUTTON) == HIGH);

    mean += reaction_time;    

    micros_to_ms(int_buffer, reaction_time);
    sprintf(buffer, "Time %s ms", int_buffer);
    display.scroll_string(buffer);
    delay(ROUND_DELAY);
    display.clear();
  }

  while(button_pressed());

  mean /= ROUNDS;
  micros_to_ms(int_buffer, mean);
  sprintf(buffer, "AVG Time %s ms", int_buffer);
  title_prompt(buffer);
  delay(ROUND_DELAY);

  if(mean < best_mean){
    best_mean = mean;
    micros_to_ms(int_buffer, mean);
    sprintf(buffer, "* NEW BEST! %s ms", int_buffer);
  } else {
    micros_to_ms(int_buffer2, best_mean);
    sprintf(buffer, "* Best Time %s ms", int_buffer2);
  }
  while(prompt(buffer) == -1);
}


void main_mode(){
  switch(prompt(load_f_string(F("Slot    Time")))){
    case -1:
      return;
    case 0:
      return;
    case 1:
      slots_game();
      return;
    case 2:
      sleep_mode();
      return;
    case 3:
      time_game();
      return;
  }
}

void loop()
{
  switch(billboard_prompt()){
    case -1:
      sleep_mode();
      break;
    case 0:
      main_mode();
      break;
    case 1:
      settings_mode();
      break;
  }
}

// time game choose a color and the average is stored with it, allowing for three profiles

