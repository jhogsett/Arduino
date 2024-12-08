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

#define DEBOUNCE_TIME 5
#define LONG_PRESS_TIME 2000
#define PROMPT_TIMEOUT 30000
#define SLEEP_TIMEOUT 900000

#define OPTION_FLIP_SCROLL_TIME 100

#define DISPLAY_BUFFER 71
#define FSTRING_BUFFER 71
// #define COPY_BUFFER 71
#define INT_BUFFER 13

#define DISPLAY_SHOW_TIME   600
#define DISPLAY_SCROLL_TIME 90

char display_buffer[DISPLAY_BUFFER];
char fstring_buffer[FSTRING_BUFFER];
// char copy_buffer[COPY_BUFFER];
char int_buffer[INT_BUFFER];

#define NUM_BILLBOARDS 3
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "Press A Button To START"; 
const char template2[] PROGMEM = "Long Press for OPTIONS"; 
const char *const templates[] PROGMEM = {template0, template1, template2};

#define BLANKING_TIME 600
#define HOME_TIMES 5
BillboardsHandler billboards_handler(display_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES, false, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);

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
  return strcpy_P(fstring_buffer, p);
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
  // Serial.begin(115200);
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

  all_leds.deactivate_leds(true);
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
bool title_prompt(char *title, byte times=1){
  all_leds.deactivate_leds(true);
  display.begin_scroll_loop(times);
  while(display.loop_scroll_string(millis(), title, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME)){
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

bool option_sound = false;
bool option_vibrate = false;

// void option_prompt(

void options_menu(){
  title_prompt(load_f_string(F("* MENU")));

  // sprintf(display_buffer, load_f_string(F("SOUND   %s")), bet_amounts[current_bet]);
  // while(button_led_prompt(load_f_string(F("SOUND   OFF"))) == -1);
  //   sprintf(display_buffer, load_f_string(F("Bet $%d Back")), bet_amounts[current_bet]);
  //   bool states[] = {false, true, false, false};
  //   switch(button_led_prompt(display_buffer, states)){
  //     case -1:
  //       return;
  //     case 0:
  //       return;
  //     case 1:
  //       break;
  //     case 2:
  //       current_bet++;
  //       if(current_bet >= NUM_BET_AMOUNTS)
  //         current_bet = 0;
  //       sprintf(display_buffer, load_f_string(F("    $%d")), bet_amounts[current_bet]);
  //       disp2.scroll_string(display_buffer, 1, OPTION_FLIP_SCROLL_TIME);
  //       continue;
  //     case 3:
  //       return;
  //   }

  while(button_led_prompt(load_f_string(F("SOUND   OFF"))) == -1);

  while(button_led_prompt(load_f_string(F("VIBRATE OFF"))) == -1);
}

void options_mode(){
  title_prompt(load_f_string(F("* OPTIONS")));
  switch(button_led_prompt(load_f_string(F("Zzz     Menu")))){
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

#define SLOTS_SHOW_TIME   20
#define SLOTS_SCROLL_TIME 20

// #define USE_PROGMEM

#define NUM_WORDS 10
#ifdef USE_PROGMEM
const char rude0[] PROGMEM = "FUCK";
const char rude1[] PROGMEM = "SHIT";
const char rude2[] PROGMEM = "CUNT";
const char rude3[] PROGMEM = "COCK";
const char rude4[] PROGMEM = "PISS";
const char rude5[] PROGMEM = "TITS";
const char rude6[] PROGMEM = "FART";
const char rude7[] PROGMEM = "POOP";
const char rude8[] PROGMEM = "DICK";
const char rude9[] PROGMEM = "GOSH";
const char *const rude_words[] PROGMEM = {rude0, rude1, rude2, rude3, rude4, rude5, rude6, rude7, rude8, rude9};

const char nice0[] PROGMEM = "WEED";
const char nice1[] PROGMEM = "VAPE";
const char nice2[] PROGMEM = "BEER";
const char nice3[] PROGMEM = "WINE";
const char nice4[] PROGMEM = "CASH";
const char nice5[] PROGMEM = "GOLD";
const char nice6[] PROGMEM = "MINT";
const char nice7[] PROGMEM = "GIFT";
const char nice8[] PROGMEM = "NUTE";
const char nice9[] PROGMEM = "MAYO";
const char *const nice_words[] PROGMEM = {nice0, nice1, nice2, nice3, nice4, nice5, nice6, nice7, nice8, nice9};
#else
static const char *rude_words[NUM_WORDS] = {"FUCK", "SHIT", "CUNT", "COCK", "PISS", "TITS", "FART", "POOP", "DICK", "BOOB"};
static const char *nice_words[NUM_WORDS] = {"WEED", "VAPE", "BEER", "WINE", "TACO", "GOLD", "MINT", "GIFT", "JADE", "CAKE"};
#endif
byte choice1, choice2, choice3;



long purse = 1000;
#define WIN_TRIPLE 10
#define WIN_DOUBLE 5
#define WIN_WORD  1
#define WIN_WORD_CUTOFF 1
#define WIN_WORDS  1
#define WIN_WORDS_CUTOFF 6
#define DEFAULT_BET 10

#define NUM_BET_AMOUNTS 4
#define BET_ALL 3
int bet_amounts[] = { 10, 25, 50, 0 };
byte current_bet = 0;


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

#ifdef USE_PROGMEM
  char word1[5];
  char word2[5];
  char word3[5];
#endif

  bool running1 = true;
  bool running2 = true;
  bool running3 = true;
  while(running1 || running2 || running3){
    unsigned long time = millis();

    if(running1){
      running1 = disp1.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running1){
        choice1 = random(NUM_WORDS);
#ifdef USE_PROGMEM
        // strcpy_P(word1, (char *)pgm_read_ptr(&(words[choice1])));
        disp1.show_string("aggr");
#else        
        disp1.show_string(words[choice1]);
#endif
      }
    }

    if(running2){
      running2 = disp2.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running2){
        choice2 = random(NUM_WORDS);
#ifdef USE_PROGMEM
        // strcpy_P(word2, (char *)pgm_read_ptr(&(words[choice2])));
        disp2.show_string("asad");
#else        
        disp2.show_string(words[choice2]);
#endif
      }
    }

    if(running3){
      running3 = disp3.loop_scroll_string(time, text, SLOTS_SHOW_TIME, SLOTS_SCROLL_TIME); 
      if(!running3){
        choice3 = random(NUM_WORDS);
#ifdef USE_PROGMEM
        // strcpy_P(word3, (char *)pgm_read_ptr(&(words[choice3])));
        disp3.show_string("1234");
#else        
        disp3.show_string(words[choice3]);
#endif
      }
    }
  }
}

void display_purse(){
  sprintf(display_buffer, "CASH $%d", purse);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);
}

void display_win(int win){
  sprintf(display_buffer, "WIN * $%d", win);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);
}

char *numeric_bet_str(int bet){
  sprintf(int_buffer, load_f_string(F("$%d")), bet);
  return int_buffer;
}

char *standard_bet_str(int bet){
  if(bet == BET_ALL){
    sprintf(int_buffer, load_f_string(F("ALL")));
    return int_buffer;
  } else
    return numeric_bet_str(bet_amounts[bet]);
}

void slots_game(){
  unsigned long sleep_timeout = millis() + SLEEP_TIMEOUT;
  unsigned long time;

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
        disp2.scroll_string(display_buffer, 1, OPTION_FLIP_SCROLL_TIME); // TODO
        continue;
      case 3:
        return;
    }

    int win = 0;
    purse -= bet_amounts[current_bet];

    slots_round(rude);  

    while(button_pressed());

    char **words;
    if(rude){
      words = rude_words;
    } else {  
      words = nice_words;
    }

#ifdef USE_PROGMEM
    sprintf_P(display_buffer, "%s%s%s", (char *)pgm_read_ptr(&(words[choice1])), (char *)pgm_read_ptr(&(words[choice2])), (char *)pgm_read_ptr(&(words[choice3])));
#else
    sprintf(display_buffer, "%s%s%s", words[choice1], words[choice2], words[choice3]);
#endif
    title_prompt(display_buffer);

    if(choice1 == choice2 && choice2 == choice3){
      win = WIN_TRIPLE;
    } else if(choice1 == choice2 || choice2 == choice3 || choice1 == choice3){
      win = WIN_DOUBLE;
    } else if(choice1 < WIN_WORD_CUTOFF || choice2 < WIN_WORD_CUTOFF || choice3 < WIN_WORD_CUTOFF) {
      win = WIN_WORD;
    } 

    win *= bet_amounts[current_bet];

    if(win){
      display_win(win);
    } else 
      // see the non-winning results in lieu of being told you lost
      delay(ROUND_DELAY);

    purse += win;

    display_purse();
  }
}
// time game specific

#define MIN_DELAY 1500
#define MAX_DELAY 5000
#define ROUNDS 3

void micros_to_ms(char * display_buffer, unsigned long micros){
  int ms_dec, ms_frac;
  ms_dec = micros / 1000;
  ms_frac = micros % 1000;
  sprintf(display_buffer, "%u.%04u", ms_dec, ms_frac);
}

unsigned long best_mean = (unsigned long)-1;

#define TIME_WIN 1000

void replace_int(){
}

void time_game(){
                              //............
  title_prompt(load_f_string(F("The TimeGame")));
                                         //............
  while(button_led_prompt(load_f_string(F("Press To Go"))) == -1);

  display.clear();
  delay(ROUND_DELAY);

  unsigned long mean = 0;
  for(byte i = 0; i < ROUNDS; i++){
    button_leds.activate_all(true);      //............
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

    micros_to_ms(int_buffer, reaction_time);
    sprintf(display_buffer, "%s ms", int_buffer);
    display.scroll_string(display_buffer, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);
    delay(ROUND_DELAY);
    display.clear();
  }

  while(button_pressed());

  mean /= ROUNDS;
  micros_to_ms(int_buffer, mean);
  sprintf(display_buffer, "AVERAGE %s ms", int_buffer);
  title_prompt(display_buffer);
  delay(ROUND_DELAY);

  if(mean < best_mean){
    best_mean = mean;
    micros_to_ms(int_buffer, mean);
    sprintf(display_buffer, "NEW BEST * %s ms", int_buffer);
    title_prompt(display_buffer);
    delay(ROUND_DELAY);

    display_win(TIME_WIN);
    purse += TIME_WIN;
    display_purse();

    sprintf(display_buffer, "NEW BEST * %s ms", int_buffer);
  } else {
    micros_to_ms(int_buffer, best_mean);
    sprintf(display_buffer, "* Best Time %s ms", int_buffer);
  }
  while(button_led_prompt(display_buffer) == -1);
}

void main_menu(){
  bool states[] = {false, true, false, true};
  switch(button_led_prompt(load_f_string(F("Slot    Time")), states)){
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

// keep stats in eeprom, have option to erase

// options

// bilboard options
// forced home or not
// sequential or random

// for slots, pick a secret combo and win the jackpot if hit

// press bet too quickly before it lights and when it shows the prompt the button is not lighted (as it it failed the debounce time)

// another bet option all