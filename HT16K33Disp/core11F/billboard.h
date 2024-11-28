#ifndef __BILLBOARD_H__
#define __BILLBOARD_H__
#include <HT16K33Disp.h>

// todo billboard types: static, scroll, blink

class Billboard
{
public:
  Billboard(HT16K33Disp * display, char * buffer, int show_delay=0, int frame_delay=0);
  void begin(byte times);
  void reset_round();
  bool step();

  HT16K33Disp * _display;
  char * _buffer;
  byte _times;
  int _show_delay;
  int _frame_delay;

private:
  int _frames;
  byte _rem_times;
};

Billboard::Billboard(HT16K33Disp * display, char * buffer, int show_delay=0, int frame_delay=0){
  _display = display;
  _buffer = buffer;
  _show_delay = show_delay;
  _frame_delay = frame_delay;
}

void Billboard::begin(byte times){
  Serial.println("begin");
  _rem_times = _times;
  reset_round();
}

void Billboard::reset_round(){
  _frames = _display->begin_scroll_string(_buffer, _show_delay, _frame_delay);
}

bool Billboard::step(){
  Serial.println("step");
  if(_display->step_scroll_string())
    return true;
  else {
    if(--_rem_times == 0)
      return false;
    else {
      reset_round();
      return true;
    }
  }
}

#endif
