#ifndef __PLAY_DATA_H
#define __PLAY_DATA_H

#include <EEPROM.h>

#define DEFAULT_PURSE 1000

// General game play
#define ROUND_DELAY 750

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

#endif
