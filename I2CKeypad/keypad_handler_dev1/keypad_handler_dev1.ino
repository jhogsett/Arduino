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

  if (keyPad.begin() == false)
  {
    // error_tone();
    while (1);
  }

  keyPad.loadKeyMap(keymap);

  // // keyPad.loadKeyMap(keymap);
  // if(!begin_keypad(keyPad, keymap)){
  //   Serial.println("Failed to begin keypad");
  // }
}


int lastKey = -1;

void loop()
{
  if (keyPad.isPressed())
  {
    int key = keyPad.getKey();

    // this is the key value sometimes immediately on releasing the key
    if(key == I2C_KEYPAD_NOKEY){
      // Serial.println("x");
      return;
    }

    // Serial.println(key);

    if(key != lastKey){
      lastKey = key;

      char ch = keyPad.getLastChar();     //  note we want the translated char

      Serial.print(key);
      Serial.print(" \t");
      Serial.println(ch);
      // delay(100);

    }
  }
  else{
    if(lastKey != -1){
      lastKey = -1;

      Serial.println("Keypress Stopped");
    }
  }

}

