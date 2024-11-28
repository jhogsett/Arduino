#ifndef __BILLBOARDS_HANDLER_H__
#define __BILLBOARDS_HANDLER_H__
#include <HT16K33Disp.h>
#include "billboard.h"

class BillboardsHandler
{
public:  
  BillboardsHandler(HT16K33Disp * display, char * buffer, byte num_billboards, char ** templates, int home_times, int blanking_time);
  void update_buffer(char *string);
  void run(unsigned long time, char *data);
  
private:
  HT16K33Disp * _display;
  char *_buffer;
  byte _num_billboards;
  char ** _templates;
  int _blanking_time;
  byte _home_times;

  Billboard *_billboard;  
  bool _running;
  byte _n_current_billboard;
  bool _showing_home;
  bool _blanking;
  unsigned long _blanking_until;
};

BillboardsHandler::BillboardsHandler(HT16K33Disp * display, char * buffer, byte num_billboards, char ** templates, int home_times, int blanking_time){
  _display = display;
  _buffer = buffer;
  _num_billboards = num_billboards;
  _templates = templates;
  _blanking_time = blanking_time;
  _home_times = home_times;
  
  _billboard = new Billboard(_display, _buffer);
  _running = false;
  _n_current_billboard = 0;
  _showing_home = false;
  _blanking = false;
  _blanking_until = 0;;
}

void BillboardsHandler::update_buffer(char *string) {
  sprintf_P(_buffer, (char *)pgm_read_ptr(&(_templates[_n_current_billboard])), string);
}

void BillboardsHandler::run(unsigned long time, char *data){
  // start blanking period when the current billboard stops running
  if (!_running && !_blanking) {
    _display->clear();
    _blanking = true;
    _blanking_until = time + _blanking_time;
    return;
  }

  // end blanking period
  if(_blanking) {
    if (time > _blanking_until)
      _blanking = false;
    else
      return;
  }

  // start next billboard, alternating home (first billboard) and random other billboards
  if (!_running) {
    if (!_showing_home) {
      _showing_home = true;
      _n_current_billboard = 0;
    } else {
      _showing_home = false;
      _n_current_billboard = random(1, _num_billboards);
    }

    // _current_billboard = &_billboards[_n_current_billboard];
    // update_buffer(data);
    _billboard->begin(1);
  }

  _running = _billboard->step();
}

#endif
