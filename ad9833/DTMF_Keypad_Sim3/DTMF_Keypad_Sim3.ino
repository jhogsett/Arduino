#include "Wire.h"
#include <SPI.h>
#include "I2CKeyPad.h"
#include <MD_AD9833.h>

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

void setup()
{
  Serial.begin(115200);

  // Serial.println();
  // Serial.println(__FILE__);
  // Serial.print("I2C_KEYPAD_LIB_VERSION: ");
  // Serial.println(I2C_KEYPAD_LIB_VERSION);
  // Serial.println();

  Wire.begin();
  // Wire.setClock(400000);

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

  if (keyPad.begin() == false)
  {
    error_tone();
    while (1);
  }

  keyPad.loadKeyMap(keymap);

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

#define MIN_KEYPRESS_TIME 50
long unsigned int key_press_time = 0;

int last_key = -1;
int call_state = STATE_IDLE;
int digit_count = 0;
int call_type = CALL_NONE;
bool key_pressed = false;

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

// int last_key = -1;
char ch;
void loop()
{
  if(call_state == STATE_DOCALL){
    call_state = STATE_DIALTONE;
    dial_tone();
    // Serial.println("start");
  }

  if (keyPad.isPressed())
  {
    int key = keyPad.getLastKey();
    // ch = keyPad.getLastChar();     //  note we want the translated char

    // if(!key_pressed){
    //   // Serial.println("key pressed");
    //   // Serial.println(key);

    //   key_pressed = true;

    if(key != last_key){
      last_key = key;

        // seems to be an odd quirk of the keypad library that
        // this must be called only once, only if the key has changed,
        // even if the character is not needed 
        ch = keyPad.getChar();     //  note we want the translated char
      // }

      key_press_time = millis();

    // if(key != last_key){
    //   last_key = key;

      // if(call_state == STATE_DIALTONE){
      //   call_state = STATE_DIALING;
      
      //   switch(ch){
      //     case LONG_PREFIX:
      //       call_type = CALL_LONG;
      //       digit_count = LONG_COUNT;
      //       break;
      //     case INTL_PREFIX:
      //       call_type = CALL_INTL;
      //       digit_count = INTL_COUNT;
      //       break;
      //     default:
      //       call_type = CALL_LOCAL;
      //       digit_count = LOCAL_COUNT;
      //       break;
      //   }
      // }

        // Serial.print(key);
        // Serial.print(" \t");
        // Serial.println(ch);
        // delay(100);
      // }

      if(call_state >= STATE_DOCALL){
      //   switch(ch){
      //     case '*':
      //     case '#':
      //     case 'A':
      //     case 'B':
      //     case 'C':
      //     case 'D':
      //       return;
      //   }
        dial_key(key);
      }
    }
  }
  else{
    // if(key_pressed){
    //   key_pressed = false;

    if(last_key != -1){
      last_key = -1;
      // }

      if(call_state >= STATE_DOCALL){
        // switch(ch){
        //   case '*':
        //   case '#':
        //   case 'A':
        //   case 'B':
        //   case 'C':
        //   case 'D':
        //     return;
        // }
        halt_dial();
      }
      // Serial.println("not pressed");

      if(millis() >= key_press_time + MIN_KEYPRESS_TIME){

        if(call_state == STATE_IDLE){
          switch(ch){
            case CMD_DOCALL:
              call_state = STATE_DOCALL;
              return;
          }          
        }

        if(call_state == STATE_DIALTONE){
          call_state = STATE_DIALING;
        
          switch(ch){
            case LONG_PREFIX:
              call_type = CALL_LONG;
              digit_count = LONG_COUNT;
              break;
            case INTL_PREFIX:
              call_type = CALL_INTL;
              digit_count = INTL_COUNT;
              break;
            default:
              call_type = CALL_LOCAL;
              digit_count = LOCAL_COUNT;
              break;
          }
        }

    // if(last_key != -1){
    //   last_key = -1;

      // Serial.print("Keypress Stopped");

        digit_count--;

        if(digit_count == 0){
          call_state = STATE_SENDING;
          delay(1000);
          call_outcome();
          delay(2000);
          call_state = STATE_IDLE;
        }
      }
    }
  }

}

