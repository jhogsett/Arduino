#ifndef __LED_HANDLER_H__
#define __LED_HANDLER_H__

class LEDHandler
{
public:
  LEDHandler(byte first_pin, byte num_leds, byte *intensity, int show_time=0, int blank_time=0);
  void begin(unsigned long time, byte style, int show_time=0, int blank_time=0);
  void step(unsigned long time);

  static const byte STYLE_PLAIN    = 0x00;
  static const byte STYLE_RANDOM   = 0x01;
  static const byte STYLE_BLANKING = 0x02;
  
  static const int DEFAULT_SHOW_TIME  = 250;
  static const int DEFAULT_BLANK_TIME = 250;
  
private:  
  byte _first_pin;
  byte _num_leds;
  byte *_intensity;  // array of bytes matching led count, 0 for digitalWrite, 1-255 for analogWrite
  int _show_time;
  int _blank_time;
  byte _style;

  char _frame;
  unsigned long _next_frame;
  int _num_frames;
  bool _blanking;
  char _active;                // virtual current active led or other state
};

LEDHandler::LEDHandler(byte first_pin, byte num_leds, byte *intensity, int show_time=0, int blank_time=0){
  _first_pin = first_pin;
  _num_leds = num_leds;
  _intensity = intensity;
  _show_time = show_time;
  _blank_time = blank_time;

  _style = 0;  
}

void LEDHandler::begin(unsigned long time, byte style, int show_time=0, int blank_time=0){
  _show_time = show_time ? show_time : DEFAULT_SHOW_TIME;
  _blank_time = blank_time ? blank_time : DEFAULT_BLANK_TIME;

  _style = style;

  // with one LED the random style cannot be used
  if(_num_leds < 2)
    _style &= ~STYLE_RANDOM;

  _frame = -1;
  // blanking = false;
  _next_frame = time; // + _show_time;

  if(_style & STYLE_BLANKING)
    _num_frames = _num_leds * 2;
  else
    _num_frames = _num_leds;

  _active = -1;
  _blanking = false;
}

void LEDHandler::step(unsigned long time){
  if(time >= _next_frame){
    if(_active != -1)
      digitalWrite(_active + _first_pin, LOW);

    _frame++;

    bool blanking_period = (_style & STYLE_BLANKING) && (_frame % 2);
    
    if(blanking_period){
      
    } else {
      if(_style & STYLE_RANDOM){
        byte r;
        while( (r = random(_num_leds)) == _active );
        _active = r;
      } else {
        _active++;
        if(_active >= _num_leds)
          _active = 0;
      }

      if(_intensity[_active] == 0)
        digitalWrite(_active + _first_pin, HIGH);
      else
        analogWrite(_active + _first_pin, _intensity[_active]);
    }

    if(blanking_period)
      _next_frame = time + _blank_time;
    else
      _next_frame = time + _show_time;

    if(_frame >= _num_frames){
      _frame = 0;
    }  
  }
}

#endif
