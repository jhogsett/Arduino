#include "Wire.h"
#include <SPI.h>
#include "I2CKeyPad.h"
#include <MD_AD9833.h>
#include "types.h"

const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad(KEYPAD_ADDRESS);
char keymap[19] = "D#0*C987B654A321INF";  //  N = NoKey, F = Fail

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 6;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins

#define SILENT_FREQ 1000000

#define ROW1 697.0
#define ROW2 770.0
#define ROW3 852.0
#define ROW4 941.0
#define COL1 1209.0
#define COL2 1336.0
#define COL3 1477.0
#define COL4 1633.0

#define DIAL_TONE_A 350.0
#define DIAL_TONE_B 440.0

int rows[4];
int cols[4];

int mapr[16];
int mapc[16];

bool begin_keypad(I2CKeyPad& keypad, char * keymap, const uint8_t address = 0x20){
  if(!keyPad.begin()){
    return false;
  }

  keyPad.loadKeyMap(keymap);
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin();
  Wire.setClock(400000);

  rows[0] = ROW1;
  rows[1] = ROW2;
  rows[2] = ROW3;
  rows[3] = ROW4;

  cols[0] = COL1;
  cols[1] = COL2;
  cols[2] = COL3;
  cols[3] = COL4;

  mapr[ 0] = 3; // D
  mapr[ 1] = 3; // #
  mapr[ 2] = 3; // 0
  mapr[ 3] = 3; // *
  mapr[ 4] = 2; // C
  mapr[ 5] = 2; // 9
  mapr[ 6] = 2; // 8
  mapr[ 7] = 2; // 7
  mapr[ 8] = 1; // B
  mapr[ 9] = 1; // 6
  mapr[10] = 1; // 5
  mapr[11] = 1; // 4
  mapr[12] = 0; // A
  mapr[13] = 0; // 3
  mapr[14] = 0; // 2
  mapr[15] = 0; // 1

  mapc[ 0] = 3; // D
  mapc[ 1] = 2; // #
  mapc[ 2] = 1; // 0
  mapc[ 3] = 0; // *
  mapc[ 4] = 3; // C
  mapc[ 5] = 2; // 9
  mapc[ 6] = 1; // 8
  mapc[ 7] = 0; // 7
  mapc[ 8] = 3; // B
  mapc[ 9] = 2; // 6
  mapc[10] = 1; // 5
  mapc[11] = 0; // 4
  mapc[12] = 3; // A
  mapc[13] = 2; // 3
  mapc[14] = 1; // 2
  mapc[15] = 0; // 1

  if(!begin_keypad(keyPad, keymap)){
    Serial.println("Failed to begin keypad");
  }

  AD1.begin();
  AD2.begin();
  AD3.begin();
  AD1.setFrequency(0, SILENT_FREQ);
  AD1.setMode(MD_AD9833::MODE_SINE);

  AD2.setFrequency(0, SILENT_FREQ);
  AD2.setMode(MD_AD9833::MODE_SINE);

  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setMode(MD_AD9833::MODE_SINE);

}

#define CALL_NONE 0
// 867-5209
#define CALL_LOCAL 1
#define LOCAL_COUNT 7
// 1-800-555-1212
#define CALL_LONG 2
#define LONG_PREFIX '1'
#define LONG_COUNT 11
// 0-44-8302-1212
#define CALL_INTL 3
#define INTL_PREFIX '0'
#define INTL_COUNT 11

#define STATE_IDLE 0
#define STATE_DOCALL 1
#define STATE_DIALTONE 2
#define STATE_DIALING 3
#define STATE_SENDING 4

#define CMD_DOCALL 'A'

void dial_tone(){
  AD1.setFrequency(0, 350.0);
  AD2.setFrequency(0, 440.0);
}

void dial_key(int key){
  AD1.setFrequency(0, rows[mapr[key]]);
  AD2.setFrequency(0, cols[mapc[key]]);
}

void halt_dial(){
  AD1.setFrequency(0, SILENT_FREQ);
  AD2.setFrequency(0, SILENT_FREQ);
}


void busy_tone(int times=4){
  for(int i = 0; i < times; i++){
    AD1.setFrequency(0, 480.0);
    AD2.setFrequency(0, 620.0);
    delay(500);

    AD1.setFrequency(0, SILENT_FREQ);
    AD2.setFrequency(0, SILENT_FREQ);
    delay(500);
  }
}

void ring_tone(int times=4){
  for(int i = 0; i < times; i++){
    AD1.setFrequency(0, 480.0);
    AD2.setFrequency(0, 440.0);
    delay(2000);

    AD1.setFrequency(0, SILENT_FREQ);
    AD2.setFrequency(0, SILENT_FREQ);

    if(i < times-1)
      delay(4000);
  }
}

void reorder_tone(int times=8){
  for(int i = 0; i < times; i++){
    AD1.setFrequency(0, 480.0);
    AD2.setFrequency(0, 620.0);
    delay(250);

    AD1.setFrequency(0, SILENT_FREQ);
    AD2.setFrequency(0, SILENT_FREQ);
    delay(250);
  }
}


void error_tone(){
  AD1.setFrequency(0, 913.8);
  delay(380);

  AD1.setFrequency(0, 1428.5);
  delay(276);

  AD1.setFrequency(0, 1776.7);
  delay(380);
  
  AD1.setFrequency(0, SILENT_FREQ);
}

void call_outcome(){
  int outcome = random(0, 4);
  switch(outcome){
    case 0:
      busy_tone();      
      break;
    case 1:
      ring_tone();
      break;
    case 2:
      reorder_tone();
      break;
    case 3:
      error_tone();
      // delay(4000);
      // offhook_tone();
      break;
  }

}


#define MIN_KEYPRESS_TIME 20

#define STATE_IDLE 0
#define STATE_MAYBE_KEY_PRESS 1
#define STATE_LEGIT_KEY_PRESS 2
#define STATE_CONTINUED_KEY_PRESS 3
#define STATE_MAYBE_KEY_RELEASE 4
#define STATE_LEGIT_KEY_RELEASE 5
#define STATE_CONTINUED_KEY_RELEASE 0

int key_press_state = STATE_IDLE;
unsigned long key_press_time = 0;
char pressed_key = -1;
char pressed_char = '\0';


void action_pressed(char key, char ch){
  Serial.print("Pressed: ");
  Serial.println(ch);
  dial_key(key);
}

void action_released(char key, char ch){
  Serial.print("Released: ");
  Serial.println(ch);
  halt_dial();
}

int keypad_event_step(KeypadAction press_action, KeypadAction release_action){
  bool key_pressed = keyPad.isPressed();

  switch(key_press_state){
    case STATE_IDLE:
      // if key pressed, and it is found to be a real key, start a new key press legitimacy check
      if(key_pressed and keyPad.getKey() != I2C_KEYPAD_NOKEY){
        key_press_state = STATE_MAYBE_KEY_PRESS;
        key_press_time = millis();
      }
      // otherwise, continue waiting for a key press
      break;

    case STATE_MAYBE_KEY_PRESS:
      // if key pressed, check the time
      if(key_pressed){
        // if key has been pressed for long enough consider it legit
        if(key_press_time + MIN_KEYPRESS_TIME <= millis()){
          key_press_state = STATE_LEGIT_KEY_PRESS;
        } 
        // otherwise, continue waiting
      } 
      // if key not pressed, it wasn't a legit key press
      else {
          key_press_state = STATE_IDLE;
      }
      break;

    case STATE_LEGIT_KEY_PRESS:
      // otherwise, act on the key press and continue with a legit key press
      pressed_key = keyPad.getLastKey();
      pressed_char = keyPad.getLastChar();
      key_press_state = STATE_CONTINUED_KEY_PRESS;
      if(press_action != NULL){
        press_action(pressed_key, pressed_char);
      }
      break;

    case STATE_CONTINUED_KEY_PRESS:
      // if key released, start a new key release legitimacy check
      if(!key_pressed){
        key_press_state = STATE_MAYBE_KEY_RELEASE;
        key_press_time = millis();
      }
      break;

    case STATE_MAYBE_KEY_RELEASE:
      // if key released, check the time
      if(!key_pressed){
        // if key has been released for long enough consider it legit
        if(key_press_time + MIN_KEYPRESS_TIME <= millis()){
          key_press_state = STATE_LEGIT_KEY_RELEASE;
        } 
        // otherwise, continue waiting
      } 
      // if key not released, it wasn't a legit key release
      else {
          key_press_state = STATE_CONTINUED_KEY_PRESS;
      }
      break;

    case STATE_LEGIT_KEY_RELEASE:
      // otherwise, act on the key release and continue with a legit key relase (idle)
      pressed_key = I2C_KEYPAD_NOKEY;
      key_press_state = STATE_IDLE;
      if(release_action != NULL){
        release_action(pressed_key, pressed_char);
      }
      break;
  }

  return key_press_state;
}

bool keypad_state_wait(int state=STATE_LEGIT_KEY_RELEASE, KeypadAction press_action=NULL, KeypadAction release_action=NULL){
  if(keypad_event_step(press_action, release_action) == state){
    return true;
  }
  return false;
}

char keypad_char_wait(char * valid_chars, int state=STATE_LEGIT_KEY_RELEASE, KeypadAction press_action=NULL, KeypadAction release_action=NULL){
  if(valid_chars != NULL){
    if(!keypad_state_wait(state, press_action, release_action)){
      return NULL;
    }
    char * chariter = valid_chars;
    while(*chariter != '\0'){
      char ch = keyPad.getLastChar();

      if(ch == *chariter){
        return ch;
      }
      chariter++;
    }
  }
  return NULL;
}

char wait_for_char(char * valid_chars, unsigned long timeoutms=1000, int completion_state=STATE_IDLE){
  unsigned long timeout_time = millis() + timeoutms;
  char ch;
  while(millis() < timeout_time){
    ch = keypad_char_wait(valid_chars, STATE_LEGIT_KEY_PRESS);
    if(ch != NULL){
      // should the timeout apply here or get stuck forever on a pressed key?
      while(millis() < timeout_time){
        if(keypad_state_wait(completion_state)){
          return ch;
        }
      }
    }
  }
  return NULL;
}

void loop()
{
  bool do_edge = false;

  if(do_edge){
    Serial.println("start level");
    char ch = wait_for_char("ABCD");
    if(ch == NULL){
      Serial.println("timeout");
    } else {
      Serial.println(ch);
    }
  }

  if(!do_edge){
    Serial.println("start edge");
    char ch = wait_for_char("ABCD", 1000, STATE_CONTINUED_KEY_PRESS);
    if(ch == NULL){
      Serial.println("timeout");
    } else {
      Serial.println(ch);
      while(!keypad_state_wait(STATE_IDLE));
    }
  }

  // switch(keypad_event_step(action_pressed, action_released)){
  //   case STATE_IDLE:
  //   case STATE_MAYBE_KEY_PRESS:
  //     break;
  //   case STATE_LEGIT_KEY_PRESS:
  //     Serial.println("legit key press");
  //     break;
  //   case STATE_CONTINUED_KEY_PRESS:
  //   case STATE_MAYBE_KEY_RELEASE:
  //     break;
  //   case STATE_LEGIT_KEY_RELEASE:
  //     Serial.println("legit key release");
  // }
}




