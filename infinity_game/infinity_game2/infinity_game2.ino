#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>
#include <EEPROM.h>
#include "billboard.h"
#include "billboards_handler.h"
#include "led_handler.h"

// #define USE_SERIAL

typedef void (*voidFuncPtr)(void);

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

// button handling
#define DEBOUNCE_TIME 5
#define LONG_PRESS_TIME 2000
#define PROMPT_TIMEOUT 30000
#define SLEEP_TIMEOUT 900000

volatile bool button_states[NUM_BUTTONS];
volatile unsigned long press_time;

// text buffering
#define DISPLAY_BUFFER 71
#define FSTRING_BUFFER 71
#define COPY_BUFFER 71

char display_buffer[DISPLAY_BUFFER];
char fstring_buffer[FSTRING_BUFFER];
char copy_buffer[COPY_BUFFER];

// display handling
#define DISPLAY_SHOW_TIME   700
#define DISPLAY_SCROLL_TIME 90
#define OPTION_FLIP_SCROLL_TIME 100

HT16K33Disp display(0x70, 3);
HT16K33Disp disp1(0x70, 1);
HT16K33Disp disp2(0x71, 1);
HT16K33Disp disp3(0x72, 1);
HT16K33Disp displays[] = {disp1, disp2, disp3};
static const byte display_brightnesses[] = {1, 9, 15}; 

// LEDs handling
#define DEFAULT_ALL_LEDS_SHOW_TIME    1000
#define DEFAULT_ALL_LEDS_BLANK_TIME   1000
#define DEFAULT_PANEL_LEDS_SHOW_TIME   750
#define DEFAULT_PANEL_LEDS_BLANK_TIME  350
#define DEFAULT_BUTTON_LEDS_SHOW_TIME  800
#define DEFAULT_BUTTON_LEDS_BLANK_TIME 400
#define TITLE_PANEL_LEDS_SHOW_TIME     100
#define TITLE_PANEL_LEDS_BLANK_TIME    0
#define TITLE_PANEL_LEDS_STYLE (LEDHandler::STYLE_PLAIN)

static const byte led_intensities[] = {0, 0, 0, 32, 40, 40};
LEDHandler all_leds(FIRST_LED, 6, led_intensities);
LEDHandler panel_leds(FIRST_PANEL_LED, 3, led_intensities + 3);
LEDHandler button_leds(FIRST_BUTTON_LED, 3, led_intensities);

// Prompt related
#define TITLE_SHOW_TIMES 3

// General game play
#define DEFAULT_PURSE 1000

// TIME GAME related 
#define MIN_DELAY 1500
#define MAX_DELAY 5000
#define ROUNDS 3
#define TIME_WIN 1000

#define ROUND_DELAY 750

// Slots Game related

#define SLOTS_SHOW_TIME   20
#define SLOTS_SCROLL_TIME 20

#define WIN_TRIPLE 5
#define WIN_DOUBLE 2
#define WIN_WORD  1
#define WIN_WORD_CUTOFF 1
#define DEFAULT_BET 10
#define WIN_WORD_BONUS 2
#define WIN_JACKPOT 100

// Betting related

#define NUM_BET_AMOUNTS 4
#define BET_ALL 3
long bet_amounts[] = { 10, 25, 50, 0 };
long current_bet = 0;

// Billboard display related
#define HOME_TIMES 5
#define BLANKING_TIME 600

#define NUM_BILLBOARDS 6
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "Press Any Button to Play"; 
const char template2[] PROGMEM = "Play Silly Slots"; 
const char template3[] PROGMEM = "Play The TimeGame"; 
const char template4[] PROGMEM = "%s"; 
const char template5[] PROGMEM = "LONG PRESS for OPTIONS"; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4, template5};

BillboardsHandler billboards_handler(display_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES, false, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);


/////////////////////////////////////
// PLAY DATA HANDLING
/////////////////////////////////////

bool option_sound = false;
bool option_vibrate = false;
long purse = DEFAULT_PURSE;
unsigned long best_time = (unsigned long)-1;

struct SavedData{
  bool option_sound;
  bool option_vibrate;
  unsigned long purse;
  unsigned long best_time;
};

void load_save_data(){
  SavedData saved_data; 
  EEPROM.get(0, saved_data);    
 option_sound = saved_data.option_sound;
 option_vibrate = saved_data.option_vibrate;
  purse = saved_data.purse;
  best_time = saved_data.best_time;  
}

void save_data(){
  SavedData saved_data; 
  saved_data.option_sound = option_sound;
  saved_data.option_vibrate = option_vibrate;
  saved_data.purse = purse;
  saved_data.best_time = best_time; 
  EEPROM.put(0, saved_data);    
}

void reset_play(){
  option_sound = true;
  option_vibrate = true;
  purse = DEFAULT_PURSE;
  best_time = (unsigned long)-1;
  save_data();
}

/////////////////////////////////////
// UTILITIES
/////////////////////////////////////

char * load_f_string(const __FlashStringHelper* f_string){
  const char *p = (const char PROGMEM *)f_string;
  return strcpy_P(fstring_buffer, p);
}

void micros_to_ms(char * buffer, unsigned long micros){
  int ms_dec, ms_frac;
  ms_dec = micros / 1000;
  ms_frac = micros % 1000;
  sprintf(buffer, "%u.%04u", ms_dec, ms_frac);
}

/////////////////////////////////////
// INITIALIZATION
/////////////////////////////////////

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
  panel_leds.begin(time, LEDHandler::STYLE_RANDOM, DEFAULT_PANEL_LEDS_SHOW_TIME, DEFAULT_PANEL_LEDS_BLANK_TIME);
  button_leds.begin(time, LEDHandler::STYLE_PLAIN | LEDHandler::STYLE_BLANKING, DEFAULT_BUTTON_LEDS_SHOW_TIME, DEFAULT_BUTTON_LEDS_BLANK_TIME);
  all_leds.begin(time, LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING | LEDHandler::STYLE_MIRROR, DEFAULT_ALL_LEDS_SHOW_TIME, DEFAULT_ALL_LEDS_BLANK_TIME);
}

void setup_buttons(){
  for(byte i = 0; i < NUM_BUTTONS; i++){
    pinMode(i + FIRST_BUTTON, 0x03); // INPUT_PULLDOWN ?!
    button_states[i] = false;
  }
}

void setup(){
#ifdef USE_SERIAL
  Serial.begin(115200);
#endif
  randomizer.randomize();
 
  setup_leds();
  setup_display();
  setup_buttons();
  
  load_save_data();

  attachInterrupt(digitalPinToInterrupt(ANY_BUTTON), button_pressed_i, RISING);
  button_states[ANY_COLOR_ID] = false;
}

/////////////////////////////////////
// LOW-LEVEL BUTTON HAHDLING
/////////////////////////////////////

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
  unsigned long debounce_timeout =  press_time + DEBOUNCE_TIME;
  while(millis() < debounce_timeout){
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

/////////////////////////////////////
// HIGH LEVEL PROMPTS
/////////////////////////////////////

void run_billboard(char *data){
  unsigned long time = millis();
  panel_leds.step(time);
  // button_leds.step(time);
  // all_leds.step(time);
  billboards_handler.run(time, &display, data);  
}

// show the billboard and cycle waiting for any button press
void billboard_prompt(voidFuncPtr on_time_out, voidFuncPtr on_press, voidFuncPtr on_long_press){
  while(button_pressed());
  unsigned long sleep_timeout = millis() + SLEEP_TIMEOUT;
  unsigned long time;

  all_leds.deactivate_leds(true);
  billboards_handler.reset();
  panel_leds.begin(time, LEDHandler::STYLE_RANDOM, 750, 350);

  micros_to_ms(display_buffer, best_time);
  sprintf(copy_buffer, load_f_string(F("Cash $%ld Best Time %s ms")), purse, display_buffer); 

  while((time = millis()) < sleep_timeout){
    run_billboard(copy_buffer);

    if(button_pressed()){
      all_leds.activate_leds(button_states, true);

      int long_press_state;
      while((long_press_state = wait_on_long_press()) == 0);
      all_leds.deactivate_leds(true);

      if(long_press_state == 1){
        on_long_press();
        return;
      }
      else {
        on_press();
        return;
      }
    }
  }
  on_time_out();
  return;
}

// prompt with text and cycle waiting for a button response
// returns -1=timed out, 0=long press, button ID otherwise
int button_led_prompt(char *prompt, bool *states=NULL){
  unsigned long time;
  unsigned long timeout_time = millis() + PROMPT_TIMEOUT;

  all_leds.deactivate_leds(true);
  if(states)
    button_leds.activate_leds(states);

  display.begin_scroll_loop();
  while((time = millis()) < timeout_time){
    display.loop_scroll_string(time, prompt, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);    

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
    // restore button leds in case of debounce time failure
    if(states)
      button_leds.activate_leds(states);

  }
  return -1;
}

// prompt with text showing, no cycle waiting for a response
// but cancelable with a button press
bool title_prompt(char *title, byte times=1, int show_panel_leds=false){
  all_leds.deactivate_leds(true);
  display.begin_scroll_loop(times);
  if(show_panel_leds)
    panel_leds.begin(millis(), TITLE_PANEL_LEDS_STYLE, TITLE_PANEL_LEDS_SHOW_TIME, TITLE_PANEL_LEDS_BLANK_TIME);
  // breaking out of the loop is handled by the display call
  while(true){
    unsigned long time = millis();
    if(display.loop_scroll_string(time, title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME)){
      if(show_panel_leds)
          panel_leds.step(time);
        if(button_pressed())
          break;
    } else
      break;
  }
}

// prompt with panel leds showing only and cycle waiting for any button press
int panel_led_prompt(){
  all_leds.deactivate_leds(true);
  panel_leds.begin(millis(), LEDHandler::STYLE_RANDOM | LEDHandler::STYLE_BLANKING, 1500, 1500);

  // the sleep mode never times out
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

// returns if timed out waiting for input
void branch_prompt(char *prompt, voidFuncPtr on_option_1, voidFuncPtr on_option_2, voidFuncPtr on_option_3, voidFuncPtr on_long_press=NULL, bool *states=NULL){
  unsigned long prompt_timeout = millis() + PROMPT_TIMEOUT;
  unsigned long time;

  byte choice;
  while((time = millis()) < prompt_timeout){
    choice = button_led_prompt(prompt, states);
    switch(choice){
      case -1:
        return;
      case 0:
        if(on_long_press)
          on_long_press();
        // always return so long pressing is a way to go back
        return;
      case 1:
        if(on_option_1){
          on_option_1();
          return;
        }
        break;
      case 2:
        if(on_option_2){
          on_option_2();
          return;
        }
        break;
      case 3:
        if(on_option_3){
          on_option_3();
          return;
        }
        break;
    }
  }
}

// returns -1 on timeout or long press, otherwise current choice
// current_choice and return value are zero-based
// toggle_position is one-based to be consistent with button states
int toggle_prompt(char *prompt, char **labels, byte current_choice, byte toggle_position, byte num_choices){
  unsigned long prompt_timeout = millis() + PROMPT_TIMEOUT;
  unsigned long time;

  while((time = millis()) < prompt_timeout){
    sprintf(display_buffer, prompt, labels[current_choice]);

    bool states[] = {false, false, false, false};
    states[toggle_position] = true;
    
    int choice = button_led_prompt(display_buffer, states);
    if(choice == 0 || choice == -1)
      return -1;

    if(choice == toggle_position){
      current_choice++;
      if(current_choice >= num_choices){
        current_choice = 0;
      }       
      displays[toggle_position].scroll_string(labels[current_choice], 1, OPTION_FLIP_SCROLL_TIME);
      continue;
    } else
      return current_choice;
    }
}

// Game Play Displays

void display_purse(){
  sprintf(display_buffer, "CASH $%ld", purse);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);
}

void display_win(long win){
  sprintf(display_buffer, "WIN * $%ld", win);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);
}

void display_jackpot(long win){
  sprintf(display_buffer, "Jackpot Words * $%ld", win);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);
}

char *numeric_bet_str(long bet){
  sprintf(copy_buffer, load_f_string(F("$%ld")), bet);
  return copy_buffer;
}

char *standard_bet_str(byte bet){
  if(bet == BET_ALL){
    sprintf(copy_buffer, load_f_string(F("ALL")));
    return copy_buffer;
  } else
    return numeric_bet_str(bet_amounts[bet]);
}


// SLEEP MODE

// repeatly show soothing LEDs while the device is idle
void sleep_mode(){
  display.clear();
  while(panel_led_prompt() == -1);
}

// OPTIONS

void options_menu(){
  char *labels[] = {"Off", "On", "No", "Yes"};
  option_sound = toggle_prompt(load_f_string(F("SOUND   %s")), labels, option_sound ? 1 : 0, 3, 2) ? true : false;
  option_vibrate = toggle_prompt(load_f_string(F("VIBRATE %s")), labels, option_vibrate ? 1 : 0, 3, 2) ? true : false;
  if(toggle_prompt(load_f_string(F("RESET   %s")), labels+2, 0, 3, 2))
    reset_play();
}

void options_mode(){
  title_prompt(load_f_string(F("* OPTIONS")));
  bool buttons[] = {false, true, false, true};
  branch_prompt(load_f_string(F("MENU   SLEEP")), options_menu, sleep_mode, sleep_mode, NULL, buttons);
}

// SLOT game

#define NUM_WORDS 10
static const char *rude_words[NUM_WORDS] = {"FUCK", "SHIT", "CUNT", "COCK", "PISS", "TITS", "FART", "POOP", "DICK", "BOOB"};
static const char *nice_words[NUM_WORDS] = {"WEED", "VAPE", "BEER", "WINE", "TACO", "GOLD", "MINT", "GIFT", "JADE", "CAKE"};
byte choice1, choice2, choice3;

void slots_round(bool rude){
  disp1.begin_scroll_loop(1);
  disp2.begin_scroll_loop(2);
  disp3.begin_scroll_loop(3);

  char * text;
  char **words;
  if(rude){
    text = load_f_string(F("    FUCK  SHIT  CUNT  COCK  PISS  TITS  FART  POOP  DICK  BOOB"));
    words = rude_words;
  } else {  
    text = load_f_string(F("    WEED  VAPE  BEER  WINE  TACO  GOLD  MINT  GIFT  JADE  CAKE"));
    words = nice_words;
  }

  bool running1 = true;
  bool running2 = true;
  bool running3 = true;
  while(running1 || running2 || running3){
    unsigned long time = millis();

    if(running1){
      running1 = disp1.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running1){
        choice1 = random(NUM_WORDS);
        disp1.show_string(words[choice1]);
      }
    }

    if(running2){
      running2 = disp2.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running2){
        choice2 = random(NUM_WORDS);
        disp2.show_string(words[choice2]);
      }
    }

    if(running3){
      running3 = disp3.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running3){
        choice3 = random(NUM_WORDS);
        disp3.show_string(words[choice3]);
      }
    }
  }
}

bool triple_word_chosen(){
  return choice1 == choice2 && choice2 == choice3;
}

bool double_word_chosen(){
  return choice1 == choice2 || choice2 == choice3 || choice1 == choice3;
}

bool special_word_chosen(){
  return choice1 < WIN_WORD_CUTOFF || choice2 < WIN_WORD_CUTOFF || choice3 < WIN_WORD_CUTOFF;
}

bool jackpot_words_chosen(byte word1, byte word2, byte word3){
  return choice1 == word1 && choice2 == word2 && choice3 == word3;
}

void slots_game(){
  title_prompt(load_f_string(F("Silly Slots")), TITLE_SHOW_TIMES, true);
  
  byte jackpot_choice1 = random(NUM_WORDS);
  byte jackpot_choice2 = random(NUM_WORDS);
  byte jackpot_choice3 = random(NUM_WORDS);
   
  bool rude;
  bool buttons[] = {false, true, false, true};
  switch(button_led_prompt(load_f_string(F("NICE    RUDE")), buttons)){
    case -1:
      return;
    case 0:
      return;
    case 1:
      rude = false;
      break;
    case 2:
      rude = random(2) == 0 ? true : false;
      break;
    case 3:
      rude = true;
      break;
  }

  unsigned long sleep_timeout = millis() + SLEEP_TIMEOUT;
  unsigned long time;

  while((time = millis()) < sleep_timeout){
    bet_amounts[BET_ALL] = purse;
    sprintf(display_buffer, load_f_string(F("Bet %s Back")), standard_bet_str(current_bet));
    bool states[] = {false, true, false, false};
    switch(button_led_prompt(display_buffer, states)){
      case -1:
        return;
      case 0:
        return;
      case 1:
        break;
      case 2:
        current_bet++;
        if(current_bet >= NUM_BET_AMOUNTS)
          current_bet = 0;
        sprintf(display_buffer, load_f_string(F("    %s")), standard_bet_str(current_bet));
        disp2.scroll_string(display_buffer, 1, OPTION_FLIP_SCROLL_TIME);
        continue;
      case 3:
        return;
    }

    int win = 0;
    bool jackpot = false;
    purse -= bet_amounts[current_bet];
    save_data();

    slots_round(rude);  

    while(button_pressed());

    char **words;
    if(rude){
      words = rude_words;
    } else {  
      words = nice_words;
    }

    sprintf(display_buffer, "%s%s%s", words[choice1], words[choice2], words[choice3]);
    title_prompt(display_buffer);

// #define JACKPOT_BONUS 10
    if(jackpot_words_chosen(jackpot_choice1, jackpot_choice2, jackpot_choice3)){
      win = WIN_JACKPOT;
      jackpot = true;
    } else if(triple_word_chosen()){
      win = WIN_TRIPLE;
      if(special_word_chosen())
        win *= WIN_WORD_BONUS;
    } else if(double_word_chosen()){
      win = WIN_DOUBLE;
      if(special_word_chosen())
        win *= WIN_WORD_BONUS;
    } else if(choice1 < WIN_WORD_CUTOFF || choice2 < WIN_WORD_CUTOFF || choice3 < WIN_WORD_CUTOFF) {
      win = WIN_WORD;
    } 

    win *= bet_amounts[current_bet];

    if(jackpot)
      display_jackpot(win);
    else if(win)
      display_win(win);
    else 
      // see the non-winning results in lieu of being told you lost
      delay(ROUND_DELAY);

    purse += win;
    save_data();
    
    display_purse();
  }
}

void time_game(){
  title_prompt(load_f_string(F("The TimeGame")), TITLE_SHOW_TIMES, true);

  int response;                                 
  response = button_led_prompt(load_f_string(F("Press to Go")));
  if(response == 0 || response == -1)
    return;

  display.clear();
  delay(ROUND_DELAY);

  unsigned long mean = 0;
  for(byte i = 0; i < ROUNDS; i++){
    button_leds.activate_all(true);
    display.scroll_string(load_f_string(F("Wait 4 FLASH")), DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
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
    sprintf(display_buffer, "%s ms", copy_buffer);
    display.scroll_string(display_buffer, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
    delay(ROUND_DELAY);
    display.clear();
  }

  while(button_pressed());

  mean /= ROUNDS;
  micros_to_ms(copy_buffer, mean);
  sprintf(display_buffer, "AVERAGE %s ms", copy_buffer);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);

  if(mean < best_time){
    best_time = mean;
    micros_to_ms(copy_buffer, mean);
    sprintf(display_buffer, "NEW BEST * %s ms", copy_buffer);
    title_prompt(display_buffer);
    delay(ROUND_DELAY);

    display_win(TIME_WIN);
    purse += TIME_WIN;
    display_purse();
    
    save_data();

    sprintf(display_buffer, "NEW BEST * %s ms", copy_buffer);
  } else {
    micros_to_ms(copy_buffer, best_time);
    sprintf(display_buffer, "* Best Time %s ms", copy_buffer);
  }
  while(button_led_prompt(display_buffer) == -1);
}

void main_menu(){
  bool buttons[] = {false, true, false, true};
  branch_prompt(load_f_string(F("Slot    Time")), slots_game, NULL, time_game, NULL, buttons);
}

void loop()
{
  billboard_prompt(sleep_mode, main_menu, options_mode);
}

//if title show times < 3 doesn't show it

// last frame of scroll animation seems to skip ahead 1

// time game choose a color and the average is stored with it, allowing for three profiles

// for slots, pick a secret combo and win the jackpot if hit

// if scoring includes special word, double 

// stop watch, alarm timer, clock