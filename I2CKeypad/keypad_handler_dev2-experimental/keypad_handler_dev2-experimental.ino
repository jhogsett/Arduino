#include "Wire.h"
#include "I2CKeyPad.h"

const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad(KEYPAD_ADDRESS);
char keymap[19] = "D#0*C987B654A321INF";  //  N = NoKey, F = Fail


typedef struct _KeypadState{
  bool pressed;
  char key;
  char ch;
  unsigned long press_time;

  char _last_key;
} KeypadState;

bool begin_keypad(I2CKeyPad& keypad, char * keymap, const uint8_t address = 0x20){
  if(!keyPad.begin()){
    return false;
  }

  keyPad.loadKeyMap(keymap);
  return true;
}

void init_keypad_state(KeypadState& state){
  bool pressed = false;
  char key = -1;
  char ch = '\0';
  state.press_time = 0;

  state._last_key = -1;
}

void get_keypad_state(I2CKeyPad& keypad, KeypadState& state){

}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin();
  Wire.setClock(400000);

  // if (keyPad.begin() == false)
  // {
  //   // error_tone();
  //   while (1);
  // }

  // keyPad.loadKeyMap(keymap);

  // keyPad.loadKeyMap(keymap);
  if(!begin_keypad(keyPad, keymap)){
    Serial.println("Failed to begin keypad");
  }
}

// long unsigned int key_press_time = 0;
// long unsigned int key_release_time = 0;

int last_key = -1;
bool in_keypress = false;
char ch;

#define MIN_KEYPRESS_TIME 50

// key press states
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

void loop()
{
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
          Serial.println("unreal key press");
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
      ch = keyPad.getLastChar();
      Serial.print("KEY PRESSED: ");
      Serial.println(ch);
      key_press_state = STATE_CONTINUED_KEY_PRESS;

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
          Serial.println("unreal key release");
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
      Serial.println("KEY RELEASE");
      key_press_state = STATE_IDLE;

      break;


  }




  // if (keyPad.isPressed())
  // {
  //   int key = keyPad.getKey();

  //   // this is the key value sometimes immediately on releasing the key
  //   if(key == I2C_KEYPAD_NOKEY){
  //     // Serial.print("x");
  //     // in_keypress = false;
  //     // last_key = -1;
  //     return;
  //   }

  //   // key_press_time = millis();
  //   // Serial.println(key);

  //   // if(key_release_time != 0L && millis() < key_release_time + MIN_KEYPRESS_TIME){
  //   //   // key released prematurely and re-pressed
  //   // }

  //   if(key != last_key){
  //     last_key = key;

  //   // if(!in_keypress){
  //     key_press_time = millis();
  //     in_keypress = true;

  //     ch = keyPad.getLastChar();     //  note we want the translated char

  //     // Serial.print(key);
  //     // Serial.print(" ");
  //     Serial.println(ch);
  //     // delay(100);

  //   } else {
  //     if(!keyPad.isPressed()){
  //       // last_key = -1;
  //       Serial.print(".");
  //     }
  //   }
  // }
  // else{
  //   if(last_key != -1){
  //     last_key = -1;
  //   // if(in_keypress){
  //   //   in_keypress = false;

  //     // key_release_time = millis();

  //     if(millis() < key_press_time + MIN_KEYPRESS_TIME){
  //       Serial.println("Keypress too short");
  //       return;
  //     }

  //     Serial.println("Keypress Accepted");
  //   }
  // }

}

