#ifndef __PROMPTS_H__
#define __PROMPTS_H__

typedef void (*voidFuncPtr)(void);

// Billboard display related
#define HOME_TIMES 5
#define BLANKING_TIME 600

// Prompt related
#define TITLE_SHOW_TIMES 3

// display handling
#define DISPLAY_SHOW_TIME   700
#define DISPLAY_SCROLL_TIME 90
#define OPTION_FLIP_SCROLL_TIME 100

#define NUM_BILLBOARDS 6
const char template0[] PROGMEM = "InfinityGame"; 
const char template1[] PROGMEM = "Press Any Button to Play"; 
const char template2[] PROGMEM = "Play Silly Slots"; 
const char template3[] PROGMEM = "Play The TimeGame"; 
const char template4[] PROGMEM = "%s"; 
const char template5[] PROGMEM = "LONG PRESS for OPTIONS"; 
const char *const templates[] PROGMEM = {template0, template1, template2, template3, template4, template5};

BillboardsHandler billboards_handler(display_buffer, NUM_BILLBOARDS, templates, BLANKING_TIME, HOME_TIMES, false, DISPLAY_SHOW_TIME, DISPLAY_SCROLL_TIME);

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

// // buttons can be NULL
// int simple_prompt(char *prompt, bool *buttons){
//   unsigned long prompt_timeout = millis() + PROMPT_TIMEOUT;
//   unsigned long time;

//   while((time = millis()) < prompt_timeout){
//     switch(button_led_prompt(prompt, buttons)){
//       case -1:
//         return;
//       case 0:
//         return;
//       case 1:
//         rude = false;
//         break;
//       case 2:
//         rude = random(2) == 0 ? true : false;
//         break;
//       case 3:
//         rude = true;
//         break;
//     }
//   }
// }

// TODO button_led_prompt() blocks, so the loop here might not be needed (would be if there were LEDS or the display to run here)
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

int hour, minute, second;

void increment_time(byte seconds=1, byte minutes=0, byte hours=0){
  second += seconds;
  if(second > 59){
    minute++;
    second = 0;
  }
  minute += minutes;
  if(minute > 59){
    hour++;
    minute = 0;
  }
  hour += hours;
  if(hour > 12)
    hour = 1;
}

void render_clock_string(){
  if(hour < 10)
    sprintf(display_buffer, load_f_string(F("   %1d %02d %02d  ")), hour, minute, second);
  else
    sprintf(display_buffer, load_f_string(F("  %2d %02d %02d  ")), hour, minute, second);
}

int clock_prompt(byte hours, byte minutes, byte seconds){
  unsigned long next_second = millis() + 1000;
  hour = hours;
  minute = minutes;
  second = seconds;

  render_clock_string();
  display.show_string(display_buffer);
  while(true){
    unsigned long time = millis();

    if(time >= next_second){
      increment_time();
      render_clock_string();
      display.show_string(display_buffer);
      next_second = time + 1000;
    }    

    if(button_pressed()){
      all_leds.activate_leds(button_states, true);

      int long_press_state;
      while((long_press_state = wait_on_long_press()) == 0);
      all_leds.deactivate_leds(true);

      if(long_press_state == 1)
        return;
      else {
        if(button_states[GREEN_ID])
          increment_time(0, 0, 1);
        else if(button_states[AMBER_ID])
          increment_time(0, 1, 0);
        else if(button_states[RED_ID])
          second = 0;
        render_clock_string();
        display.show_string(display_buffer);
      }
    }
  }    
}

#endif
