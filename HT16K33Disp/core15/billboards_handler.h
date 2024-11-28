#ifndef __BILLBOARDS_HANDLER_H__
#define __BILLBOARDS_HANDLER_H__
#include <HT16K33Disp.h>
#include "billboard.h"

class BillboardsHandler
{
public:  
  BillboardsHandler(char * buffer, byte num_templates, char ** templates, int blanking_time, byte home_times, int show_delay=0, int frame_delay=0);
  void update_buffer(char *string);
  void run(unsigned long time, HT16K33Disp * display, char *string);
  bool handle_blanking(HT16K33Disp * display, unsigned long time);
  bool terminate_blanking(unsigned long time);
  void process_display(HT16K33Disp * display, char *string);
  void process_billboard_switch();
  byte run_times();
  
private:
  char *_buffer;
  byte _num_billboards;
  Billboard *_billboard;
  char ** _templates;
  int _blanking_time;
  byte _home_times;
  
  bool _running;
  char *_current_template;
  byte _n_current_template;
  bool _showing_home;
  bool _blanking;
  unsigned long _blanking_until;
};

BillboardsHandler::BillboardsHandler(char * buffer, byte num_templates, char ** templates, int blanking_time, byte home_times, int show_delay=0, int frame_delay=0){
  _buffer = buffer;
  _num_billboards = num_templates;
  _billboard = new Billboard(buffer, show_delay, frame_delay);
  _templates = templates;
  _blanking_time = blanking_time;
  _home_times = home_times;
  
  _running = false;
  _current_template = NULL;
  _n_current_template = 0;
  _showing_home = false;
  _blanking = false;
  _blanking_until = 0;;
}

void BillboardsHandler::update_buffer(char *string) {
  sprintf_P(_buffer, (char *)pgm_read_ptr(&(_templates[_n_current_template])), string);
}

void BillboardsHandler::run(unsigned long time, HT16K33Disp * display, char *string){
  if(handle_blanking(display, time))
    return;
  if(terminate_blanking(time))
    return;
  process_display(display, string);
  _running = _billboard->step(display, time);
}

// start blanking period when the current billboard stops running
bool BillboardsHandler::handle_blanking(HT16K33Disp * display, unsigned long time){
  if (!_running && !_blanking) {
    display->clear();
    _blanking = true;
    _blanking_until = time + _blanking_time;
    return true;
  }
  return false;
}

// end blanking period
bool BillboardsHandler::terminate_blanking(unsigned long time){
  if(_blanking) {
    if (time > _blanking_until)
      _blanking = false;
    else
      return true;
  }
  return false;
}

// start next billboard, alternating home (first billboard) and random other billboards
void BillboardsHandler::process_display(HT16K33Disp * display, char *string){
  if (!_running) {
    process_billboard_switch();
    update_buffer(string);
    _billboard->begin(display, run_times());
  }
}

void BillboardsHandler::process_billboard_switch(){
  if (!_showing_home) {
    _showing_home = true;
    _n_current_template = 0;
  } else {
    _showing_home = false;
    _n_current_template = random(1, _num_billboards);
  }

}

byte BillboardsHandler::run_times(){
  byte times;
  if(_showing_home)
    times = _home_times;
  else
    times = 1;
  return times;
}

#endif
