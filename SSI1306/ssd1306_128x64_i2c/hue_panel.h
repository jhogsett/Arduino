#ifndef __HUE_PANEL_H__
#define __HUE_PANEL_H__

#include "panel_handler.h"

class HuePanel : public PanelHandler
{
public:
  HuePanel(Adafruit_SSD1306 * display, byte x, byte y, byte w, byte h, PanelHandler ** sub_panels, byte num_panels);
  virtual void on_draw(byte xoff, byte yoff, bool flush);

private:  

  // in 100ths of a degree
  int _hue;

};

HuePanel::HuePanel(Adafruit_SSD1306 * display, byte x, byte y, byte w, byte h, PanelHandler ** sub_panels, byte num_panels) 
  : PanelHandler(display, x, y, w, h, sub_panels, num_panels){
  _hue = 0;
}

void HuePanel::on_draw(byte xoff, byte yoff, bool flush){
  // if(flush){
  //   // erase area owned by panel
  //   _display->setCursor(xoff + _x, yoff + _y);
  //   _display->fillRect(xoff + _x, yoff + _y, _w, _h, SSD1306_BLACK);
  // }
  
  if(_active)
    _display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  else
    _display->setTextColor(SSD1306_WHITE);
  
  int huei = _hue / 10;
  int huef = _hue % 10;
  
  char buf[22];
  sprintf_P(buf, PSTR("HUE  %3d.%d"), huei, huef);
 
  // int cxoff = (_w - text_width(4, TEXT_SIZE)) / 2;
  // int cyoff = (_h - text_height(1, TEXT_SIZE)) / 2;
  
  // Serial.println(cxoff + xoff + _x);
  // Serial.println(cyoff + yoff + _y);
  // Serial.println();
  
  // _display->setCursor(cxoff + xoff + _x, cyoff + yoff + _y);
  _display->setCursor(xoff + _x, yoff + _y);
  Serial.println(xoff + _x);
  Serial.println(yoff + _y);
  Serial.println();
  
  _display->setTextSize(2);
  _display->print(buf);

  // if(_num_panels){
  //   for(byte i = 0; i < _num_panels; i++){
  //     _sub_panels[i]->on_draw(xoff + _x, yoff + _y, false);
  //   }
  // }
  
  // if(flush)
  //   _display->display();
}

#endif