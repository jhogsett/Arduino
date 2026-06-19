//
//    FILE: I2Ckeypad_keymap.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo key mapping
//     URL: https://github.com/RobTillaart/I2CKeyPad
//
//  PCF8574
//    pin p0-p3 rows
//    pin p4-p7 columns
//  4x4 or smaller keypad.


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

  if (keyPad.begin() == false)
  {
    error_tone();
    while (1);
  }

  keyPad.loadKeyMap(keymap);

  AD1.begin();
  AD2.begin();
  AD3.begin();
  AD1.setFrequency(0, DIAL_TONE_A);
  AD1.setMode(MD_AD9833::MODE_SINE);

  AD2.setFrequency(0, DIAL_TONE_B);
  AD2.setMode(MD_AD9833::MODE_SINE);

  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setMode(MD_AD9833::MODE_SINE);
}

int lastKey = -1;

void loop()
{
  if (keyPad.isPressed())
  {
    int key = keyPad.getLastKey();

    if(key != lastKey){
      lastKey = key;

      char ch = keyPad.getChar();     //  note we want the translated char

      // Serial.print(key);
      // Serial.print(" \t");
      // Serial.println(ch);
      // delay(100);

      AD1.setFrequency(0, rows[mapr[key]]);
      AD2.setFrequency(0, cols[mapc[key]]);
    }
  }
  else{
    if(lastKey != -1){
      lastKey = -1;

      // Serial.print("Keypress Stopped");

      AD1.setFrequency(0, SILENT_FREQ);
      AD2.setFrequency(0, SILENT_FREQ);
    }
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

