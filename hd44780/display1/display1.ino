
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

void setup()
{
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
}

class AudioGroup
{
public:
  AudioGroup(byte id, long frequency, long step, int phase, byte state){
    _id = id;
    _frequency = frequency;
    _step = step;
    _phase = phase;
    _state = state;   
  }

  void decimalize(long value, char *buffer){
    long main = value / 10L;
    int dec = value % 10L;
    sprintf(buffer, "%ld.%d", main, dec);
  }

  void show_right_aligned(byte col, byte row, const char *buffer, byte max_width){
    byte width = strlen(buffer);
    if(width <= max_width){
      col += (max_width - width);
    }
    lcd.setCursor(col, row);
    lcd.write(buffer);
  }

  void show_centered(byte col, byte row, const char *buffer, byte max_width){
    byte width = strlen(buffer);
    if(width <= max_width){
      col += (max_width - width) / 2;
    }
    lcd.setCursor(col, row);
    lcd.write(buffer);
  }

  // long represents a value in 1/10ths
  void show_fixed_point_long(long value, byte col, byte row, char *buffer, byte max_width){
    decimalize(value, buffer);
    show_right_aligned(col, row, buffer, max_width);
  }

  void show_frequency(byte col, byte row, char *buffer, byte max_width){
    show_fixed_point_long(_frequency, col, row, buffer, max_width);
  }

  void show_step(byte col, byte row, char *buffer, byte max_width){
    show_fixed_point_long(_step, col, row, buffer, max_width);
  }

  void show_phase(byte col, byte row, char *buffer, byte max_width){
    show_fixed_point_long(_phase, col, row, buffer, max_width-1);
    lcd.write(223);
  }
  


  void show_state(byte col, byte row, byte max_width){
    switch(_state){
      case STATE_NORMAL:
        show_centered(col, row, "Norm", max_width);
        break;
      case STATE_MUTED:
        show_centered(col, row, "Mute", max_width);
        break;
      case STATE_SOLO:
        show_centered(col, row, "Solo", max_width);
        break;
    }
  }

  // 0=top, 1=middle, 2=bottom
  void show_sep(byte col, byte row, byte max_width, char sep){
    lcd.setCursor(col + max_width, row);
    lcd.write(sep);
    // lcd.write('|'); // most acceptable
    // lcd.write(' '); // acceptable
    // lcd.write(255); // acceptable white full block (looks nice but too much contrast)
    // lcd.write('_'); // unacceptable
    // lcd.write(165); // unacceptable centered period (easily confused with decimal point)
  }

  void show(bool last_group=false){
    char buffer[10];
    byte col = (_id - 1) * GROUP_WIDTH;  
    byte max_width = GROUP_WIDTH-1;

    show_frequency(col, 0, buffer, max_width);    
    if(!last_group)
      show_sep(col, 0, max_width, 1);
    show_step(col, 1, buffer, max_width);    
    if(!last_group)
      show_sep(col, 1, max_width, 2);
    show_phase(col, 2, buffer, max_width);    
    if(!last_group)
      show_sep(col, 2, max_width, 3);
    show_state(col, 3, max_width);    
    if(!last_group)
      show_sep(col, 3, max_width, 3);
  }

  static const int GROUP_WIDTH = 7;

  static const int STATE_NORMAL = 0;
  static const int STATE_MUTED = 1;
  static const int STATE_SOLO = 2;

private:
  byte _id;
  long _frequency; // in 1/10 Hz
  long _step;      // in 1/10 Hz
  int _phase;      // in 1/10 degree
  byte _state;
    
};


// group IDs are 1 based
AudioGroup group1(1, 8530L, 10L, 0, AudioGroup::STATE_NORMAL);
AudioGroup group2(2, 9600L, 20L, 0, AudioGroup::STATE_MUTED);
AudioGroup group3(3, 20833L, 50L, 0, AudioGroup::STATE_SOLO);
AudioGroup groups[3] = {group1, group2, group3};

void loop()
{
  for(int i = 0; i < 3; i++){
    groups[i].show(i == 2);
  }  
  
  while(true);
}
