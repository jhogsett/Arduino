#ifndef __PANEL_HANDLER_H__
#define __PANEL_HANDLER_H__

#include <Adafruit_SSD1306.h>

class PanelHandler
{
public:  
  PanelHandler(Adafruit_SSD1306 * display, byte x, byte y, byte w, byte h, PanelHandler ** sub_panels, byte num_panels);

  Adafruit_SSD1306 * _display;
  byte _x;
  byte _y;  
  byte _w;
  byte _h;  
  bool _active;
  
  const int EVENT_DECREMENT = 0;
  const int EVENT_PRESS = 1;
  const int EVENT_INCREMENT = 2;
  const int EVENT_LONG = 3;
  
  void PanelHandler::on_draw(byte xoff, byte yoff, bool flush=true);
    
private:

  void PanelHandler::activate_sub_panel(byte panel);
  int PanelHandler::find_active_sub_panel();
  bool PanelHandler::activate(bool active=true);
  bool PanelHandler::on_event(bool top_level=false);
  byte PanelHandler::text_width(byte num_chars, byte factor=1);
  byte PanelHandler::text_height(byte num_lines, byte factor=1);

  PanelHandler **_sub_panels;
  byte _num_panels;
};


PanelHandler::PanelHandler(Adafruit_SSD1306 * display, byte x, byte y, byte w, byte h, PanelHandler ** sub_panels, byte num_panels){
  _display = display;
  _x = x;
  _y = y;
  _w = w;
  _h = h;
  _active = false;
  _sub_panels = sub_panels;
  _num_panels = num_panels;
}    

void PanelHandler::activate_sub_panel(byte panel){
  if(_num_panels > 0){
    for(byte i = 0; i < _num_panels; i++){
      _sub_panels[i]->activate(false);
    }
    _sub_panels[panel]->activate(true);
  }
}

// returns -1 if no panels
int PanelHandler::find_active_sub_panel(){
  if(_num_panels == 0){
    return -1;
  } else {
    for(byte i = 0; i < _num_panels; i++){
      if(_sub_panels[i]->_active){
        return i;
      }
    }
  }

// no active panel or only one sub panel   
if(!_sub_panels[0]->_active)
    activate_sub_panel(0);

return 0;
}

bool PanelHandler::activate(bool active){
  bool was_active = _active;
  _active = active;
  
  // if(!was_active){
    
  // }
}

// return true if the display should be refreshed
bool PanelHandler::on_event(bool top_level){
  if(!_active){
    activate();
  }
  
  if(_num_panels > 0){
    int active_panel = this->find_active_sub_panel();
    
    // if there are sub panels
      // find the active sub panel
      
    // if an encoder event
      // send to the active sub panel

    // if a press event
      // if there are 2 or more sub panels
        // toggle the current active sub panel
        
      // if only 1 sub panel
        // send to the active sub panel
        
      // if no sub panels
        // consumed or ignored by this panel

    // if a long event                  
        // consumed or ignored by this panel
  }
}

byte PanelHandler::text_width(byte num_chars, byte factor){
  return 6 * num_chars * factor;
}

byte PanelHandler::text_height(byte num_lines, byte factor){
  return 8 * num_lines * factor;
}

#define TEXT_SIZE 2

void PanelHandler::on_draw(byte xoff, byte yoff, bool flush){
  if(flush){
    // erase area owned by panel
    _display->setCursor(xoff + _x, yoff + _y);
    _display->fillRect(xoff + _x, yoff + _y, _w, _h, SSD1306_BLACK);
  }
  
  if(_active)
    _display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  else
    _display->setTextColor(SSD1306_WHITE);
  
  char buf[10];
  sprintf_P(buf, PSTR("%04x"), (unsigned long)this);
 
  int cxoff = (_w - text_width(4, TEXT_SIZE)) / 2;
  int cyoff = (_h - text_height(1, TEXT_SIZE)) / 2;
  
  Serial.println(cxoff + xoff + _x);
  Serial.println(cyoff + yoff + _y);
  Serial.println();
  
  _display->setCursor(cxoff + xoff + _x, cyoff + yoff + _y);
  
  _display->setTextSize(TEXT_SIZE);
  _display->print((unsigned long)this, HEX);

  if(_num_panels){
    for(byte i = 0; i < _num_panels; i++){
      _sub_panels[i]->on_draw(xoff + _x, yoff + _y, false);
    }
  }
  
  if(flush)
    _display->display();
}

#endif