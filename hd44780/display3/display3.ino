
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
#include <MD_AD9833.h>
#include <SPI.h>
#include "audio_group.h"

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 9;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 8;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC4 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
// MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

#define SILENTFREQ 100000.0

void silence(){
  AD1.setFrequency(0, SILENTFREQ);
  AD2.setFrequency(0, SILENTFREQ);
  AD3.setFrequency(0, SILENTFREQ);
  // AD4.setFrequency(0, SILENT_FREQ);
}

// group IDs are 1 based
AudioGroup group1(&lcd, &AD1, 1, 5233L, 2, 0, AudioGroup::STATE_MUTED);
AudioGroup group2(&lcd, &AD2, 2, 6593L, 2, 0, AudioGroup::STATE_MUTED);
AudioGroup group3(&lcd, &AD3, 3, 7939L, 2, 0, AudioGroup::STATE_MUTED);
AudioGroup *groups[3] = {&group1, &group2, &group3};

int handle_group(AudioGroup * group, int data){
  switch(data){
    case 0:
      // decrement
      group->step_frequency(-1);
      break;
    case 1:
      // button press
      group->toggle_state(groups, 3);
      break;
    case 2:
      // increment
      group->step_frequency(1);
      break;
    case 3:
      // button repeat
      // fader.on();
      break;
  }
  group->show();
  group->update_generator();
}

void loop()
{
  for(int i = 0; i < 3; i++){
    groups[i]->show(i == 2);
  }  
  
  groups[0]->show_sep();

  char buffer[10];
  byte read = 0;
  if((read = Serial.readBytesUntil('\n', buffer, 9)) != 0){
    buffer[read] = '\0';

    int id = buffer[0] - '0';
    int data = (buffer[1] - '0');

    if(id > 0 && id < 4 && data >= 0 and data <= 3){
      handle_group(groups[id-1], data);
    }
  }
}


void setup()
{
  Serial.begin(230400);
  
  int status;

	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}

	// initalization was successful, the backlight should be on now

	// Print a message to the LCD
	// lcd.print("Hello, World!");
  


  uint8_t line_sep[8] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
  uint8_t dots_sep[8] = {0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00};
  uint8_t far_dots_sep[8] = {0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x00};
  // uint8_t far_far_dots_sep[8] = {0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00};

  // vertical brackets  
  // uint8_t top_sep[8] = {0x0e,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
  // uint8_t mid_sep[8] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
  // uint8_t bot_sep[8] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x0e};

	lcd.createChar(1, line_sep);
	lcd.createChar(2, dots_sep);
	lcd.createChar(3, far_dots_sep);
	// lcd.createChar(4, far_far_dots_sep);
  
  AD1.begin();
  AD1.setMode(MD_AD9833::MODE_SINE);
  AD1.setFrequency(0, SILENTFREQ);

  AD2.begin();
  AD2.setMode(MD_AD9833::MODE_SINE);
  AD2.setFrequency(0, SILENTFREQ);

  AD3.begin();
  AD3.setMode(MD_AD9833::MODE_SINE);
  AD3.setFrequency(0, SILENTFREQ);
  
}
