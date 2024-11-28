#ifndef __BILLBOARDS_HANDLER_H__
#define __BILLBOARDS_HANDLER_H__
#include <HT16K33Disp.h>
#include "billboard.h"

class BillboardsHandler
{
public:  
  BillboardsHandler(char * buffer, byte num_billboards, Billboard *billboards, char ** templates, int blanking_time);
  void update_billboard(char *string);
  void run(unsigned long time, HT16K33Disp * display, char *string);
  
private:
  char *_buffer;
  
  byte _num_billboards;
  Billboard *_billboards;
  char ** _templates;
  int _blanking_time;
  
  bool _running;
  char *_current_template;
  Billboard *_current_billboard;
  byte _n_current_billboard;
  bool _showing_home;
  bool _blanking;
  unsigned long _blanking_until;
};

BillboardsHandler::BillboardsHandler(char * buffer, byte num_billboards, Billboard *billboards, char ** templates, int blanking_time){
  _buffer = buffer;
  _num_billboards = num_billboards;
  _billboards = billboards;
  _templates = templates;
  _blanking_time = blanking_time;
  
  _running = false;
  _current_template = NULL;
  _current_billboard = NULL;
  _n_current_billboard = 0;
  _showing_home = false;
  _blanking = false;
  _blanking_until = 0;;
}

void BillboardsHandler::update_billboard(char *string) {
  sprintf_P(_buffer, (char *)pgm_read_ptr(&(_templates[_n_current_billboard])), string);
}

void BillboardsHandler::run(unsigned long time, HT16K33Disp * display, char *string){
  // start blanking period when the current billboard stops running
  if (!_running && !_blanking) {
    display->clear();
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
    _current_billboard = &_billboards[0];
    update_billboard(string);
    _current_billboard->begin(3);
  }

  _running = _current_billboard->step();
}

#endif
