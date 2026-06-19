#include "Wire.h"
#include "I2CKeyPad.h"
#include "types.h"

const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad(KEYPAD_ADDRESS);
char keymap[19] = "D#0*C987B654A321INF";  //  N = NoKey, F = Fail

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

  if(!begin_keypad(keyPad, keymap)){
    Serial.println("Failed to begin keypad");
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
}

void action_released(char key, char ch){
  Serial.print("Released: ");
  Serial.println(ch);
}

void keypad_event_step(KeypadAction press_action, KeypadAction release_action){
  bool key_pressed = keyPad.isPressed();

  switch(key_press_state){
    case STATE_IDLE:
      
      // if key pressed, and it is found to be a real key, start a new key press legitimacy check
      if(key_pressed and keyPad.getKey() != I2C_KEYPAD_NOKEY){
        key_press_state = STATE_MAYBE_KEY_PRESS;
        key_press_time = millis();
        // Serial.println("maybe key press");
      }
      // otherwise, continue waiting for a key press
      break;

    case STATE_MAYBE_KEY_PRESS:

      // if key pressed, check the time
      if(key_pressed){
        // if key has been pressed for long enough consider it legit
        if(key_press_time + MIN_KEYPRESS_TIME <= millis()){
          key_press_state = STATE_LEGIT_KEY_PRESS;
          // Serial.println("legit key press");
        } 

        // otherwise, continue waiting
      } 
      // if key not pressed, it wasn't a legit key press
      else {
          key_press_state = STATE_IDLE;
          // Serial.println("unreal key press");
      }

      break;

    case STATE_LEGIT_KEY_PRESS:

      // maybe this block should be left out?
      // if key released, start a new key release legitimacy check
      // if(!key_pressed){
      //   key_press_state = STATE_MAYBE_KEY_RELEASE;
      //   key_press_time = millis();
      // }

      // otherwise, act on the key press and continue with a legit key press
      pressed_key = keyPad.getLastKey();
      pressed_char = keyPad.getLastChar();
      // Serial.print("KEY PRESSED: ");
      // Serial.println(pressed_char);
      key_press_state = STATE_CONTINUED_KEY_PRESS;
      press_action(pressed_key, pressed_char);

      break;

    case STATE_CONTINUED_KEY_PRESS:

      // if key released, start a new key release legitimacy check
      if(!key_pressed){
        key_press_state = STATE_MAYBE_KEY_RELEASE;
        key_press_time = millis();
        // Serial.println("maybe key release");
      }

      break;

    case STATE_MAYBE_KEY_RELEASE:

      // if key released, check the time
      if(!key_pressed){
        // if key has been released for long enough consider it legit
        if(key_press_time + MIN_KEYPRESS_TIME <= millis()){
          key_press_state = STATE_LEGIT_KEY_RELEASE;
          // Serial.println("legit key release");
        } 

        // otherwise, continue waiting
      } 
      // if key not released, it wasn't a legit key release
      else {
          key_press_state = STATE_CONTINUED_KEY_PRESS;
          // Serial.println("unreal key release");
      }

      break;

    case STATE_LEGIT_KEY_RELEASE:

      // maybe this block should be left out?
      // if key released, start a new key release legitimacy check
      // if(!key_pressed){
      //   key_press_state = STATE_MAYBE_KEY_RELEASE;
      //   key_press_time = millis();
      // }

      // otherwise, act on the key release and continue with a legit key relase (idle)
      pressed_key = I2C_KEYPAD_NOKEY;
      key_press_state = STATE_IDLE;
      // Serial.println("KEY RELEASE");
      release_action(pressed_key, pressed_char);

      break;
  }
}

void loop()
{
  keypad_event_step(action_pressed, action_released);
}




