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

#define MIN_KEYPRESS_TIME 10
long unsigned int key_press_time = 0;
long unsigned int key_release_time = 0;

int last_key = -1;
bool in_keypress = false;
char ch;

void loop()
{
  if (keyPad.isPressed())
  {
    int key = keyPad.getKey();

    // this is the key value sometimes immediately on releasing the key
    if(key == I2C_KEYPAD_NOKEY){
      // Serial.print("x");
      // in_keypress = false;
      // last_key = -1;
      return;
    }

    // key_press_time = millis();
    // Serial.println(key);

    // if(key_release_time != 0L && millis() < key_release_time + MIN_KEYPRESS_TIME){
    //   // key released prematurely and re-pressed
    // }

    if(key != last_key){
      last_key = key;

    // if(!in_keypress){
      key_press_time = millis();
      in_keypress = true;

      ch = keyPad.getLastChar();     //  note we want the translated char

      // Serial.print(key);
      // Serial.print(" ");
      Serial.println(ch);
      // delay(100);

    } else {
      if(!keyPad.isPressed()){
        // last_key = -1;
        Serial.print(".");
      }
    }
  }
  else{
    if(last_key != -1){
      last_key = -1;
    // if(in_keypress){
    //   in_keypress = false;

      // key_release_time = millis();

      if(millis() < key_press_time + MIN_KEYPRESS_TIME){
        Serial.println("Keypress too short");
        return;
      }

      Serial.println("Keypress Accepted");
    }
  }

}

