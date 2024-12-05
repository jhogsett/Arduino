
#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>
#include "billboard.h"
#include "billboards_handler.h"
#include "led_handler.h"

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

// hardware definitions
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

#define DEFAULT_BLANK_TIME 500
#define DEFAULT_SHOW_TIME 500

#define BUTTON_WAIT_TIMEOUT 5000

#define SCROLL_SHOW 500
#define SCROLL_DELAY 100

#define DEFAULT_PANEL_LED_INTENSITY 16

#define DEBOUNCE_TIME 7
#define LONG_PRESS_TIME 2000
#define PROMPT_TIMEOUT 10000
#define SLEEP_TIMEOUT 900000

#define DISPLAY_BUFFER 101
#define INT_BUFFER 13
char buffer[DISPLAY_BUFFER];
char copy_buffer[DISPLAY_BUFFER];

#define NUM_BILLBOARDS 3
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "12-05-24 Version"; 
const char template2[] PROGMEM = "Press A Button To Start"; 
const char *const templates[] PROGMEM = {template0, template1, template2};

#define BLANKING_TIME 750
#define HOME_TIMES 3
BillboardsHandler billboards_handler(buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES);

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

char * load_f_string(const __FlashStringHelper* f_string){
  const char *p = (const char PROGMEM *)f_string;
  return strcpy_P(copy_buffer, p);
}

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

void run_billboard(){
  unsigned long time = millis();
  panel_leds.step(time);
  // button_leds.step(time);
  // all_leds.step(time);
  billboards_handler.run(time, &display, NULL);  
}

// show the billboard and cycle waiting for any button press
// returns -1 on timeout, 1 on long press, 0 otherwise
int billboard_prompt(){
  unsigned long sleep_timeout = millis() + SLEEP_TIMEOUT;
  unsigned long time;

  billboards_handler.reset();
  panel_leds.begin(time, LEDHandler::STYLE_RANDOM, 750, 350);
  while((time = millis()) < sleep_timeout){
    run_billboard();

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

// prompt with text and cycle waiting for a button response
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
    }
  }
  return -1;
}

// prompt with text showing, no cycle waiting for a response
// but cancelable with a button press
bool title_prompt(char *title, byte times=1){
  all_leds.deactivate_leds(true);
  display.begin_scroll_loop(times);
  while(display.loop_scroll_string(millis(), title)){
    if(button_pressed()){
      break;
    }
  }
}

// prompt with panel leds showing only and cycle waiting for any button press
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

// repeatly show soothing LEDs while the device is idle
void sleep_mode(){
  display.clear();
  while(panel_led_prompt() == -1);
}

void options_menu(){
  title_prompt(load_f_string(F("* MENU")));
  while(prompt(load_f_string(F("SOUND   OFF"))) == -1);
  while(prompt(load_f_string(F("VIBRATE OFF"))) == -1);
}

void options_mode(){
  title_prompt(load_f_string(F("* OPTIONS")));
  switch(prompt(load_f_string(F("Sleep   Menu")))){
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
      options_menu();
      return;
  }
}

#define ROUND_DELAY 750

// slots game specific

#define SLOTS_SHOW_TIME   25
#define SLOTS_SCROLL_TIME 25

#define NUM_BAD_WORDS 9
static const char *bad_words[NUM_BAD_WORDS] = {"SHIT", "PISS", "CUNT", "FUCK", "COCK", "TITS", "FART", "POOP", "DICK"};
byte choice1, choice2, choice3;

void slots_round(){
  choice1 = random(NUM_BAD_WORDS);
  choice2 = random(NUM_BAD_WORDS);
  choice3 = random(NUM_BAD_WORDS);

  disp1.begin_scroll_loop(2);
  disp2.begin_scroll_loop(3);
  disp3.begin_scroll_loop(4);

  char * text = load_f_string(F("    SHIT  PISS  CUNT  FUCK  COCK  TITS  FART  POOP  DICK  "));

  bool running1 = true;
  bool running2 = true;
  bool running3 = true;
  while(running1 || running2 || running3){
    unsigned long time = millis();

    if(running1){
      running1 = disp1.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running1){
        disp1.show_string(bad_words[choice1]);
      }
    }

    if(running2){
      running2 = disp2.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running2){
        disp2.show_string(bad_words[choice2]);
      }
    }

    if(running3){
      running3 = disp3.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running3){
        disp3.show_string(bad_words[choice3]);
      }
    }
  }
}

void slots_game(){
  while(prompt(load_f_string(F("Dirty Slots"))) == -1);

  // todo timeout
  while(true){
    slots_round();
    while(button_pressed());
    delay(ROUND_DELAY);

    display.clear();
    sprintf(buffer, "%s%s%s", bad_words[choice1], bad_words[choice2], bad_words[choice3]);
    title_prompt(buffer);
    
    switch(prompt(load_f_string(F("More    Back")))){
      case -1:
        return;
      case 0:
        return;
      case 1:
        break;
      case 2:
        break;
      case 3:
        return;
    }
  }
}

// time game specific

#define MIN_DELAY 1500
#define MAX_DELAY 5000
#define ROUNDS 3

void micros_to_ms(char * buffer, unsigned long micros){
  int ms_dec, ms_frac;
  ms_dec = micros / 1000;
  ms_frac = micros % 1000;
  sprintf(buffer, "%u.%04u", ms_dec, ms_frac);
}

unsigned long best_mean = (unsigned long)-1;

void time_game(){
  title_prompt(load_f_string(F("The Time Game")));

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

    micros_to_ms(copy_buffer, reaction_time);
    sprintf(buffer, "Time %s ms", copy_buffer);
    display.scroll_string(buffer);
    delay(ROUND_DELAY);
    display.clear();
  }

  while(button_pressed());

  mean /= ROUNDS;
  micros_to_ms(copy_buffer, mean);
  sprintf(buffer, "AVG Time %s ms", copy_buffer);
  title_prompt(buffer);
  delay(ROUND_DELAY);

  if(mean < best_mean){
    best_mean = mean;
    micros_to_ms(copy_buffer, mean);
    sprintf(buffer, "* NEW BEST! %s ms", copy_buffer);
  } else {
    micros_to_ms(copy_buffer, best_mean);
    sprintf(buffer, "* Best Time %s ms", copy_buffer);
  }
  while(prompt(buffer) == -1);
}

void main_menu(){
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
      main_menu();
      break;
    case 1:
      options_mode();
      break;
  }
}

// time game choose a color and the average is stored with it, allowing for three profiles

