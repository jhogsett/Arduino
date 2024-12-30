#ifndef __PRICE_HANDLER_H__
#define __PRICE_HANDLER_H__

class PriceHandler
{
public:
  PriceHandler(char *buffer, long price_down_time, int price_down_amount, int price_min);
  void begin(int price, unsigned long time);
  bool step(unsigned long time);
  void reset(unsigned long time);
  void refresh_price();
  void boost_price(byte factor, int price_max);
  void format_price(int price, char *buffer);
  int price();
  
private:
  char * _buffer;  
  long _price_down_time;
  int _price_down_amount;
  int _price_min;
  int _price;
  long _next_price_down;
};

PriceHandler::PriceHandler(char *buffer, long price_down_time, int price_down_amount, int price_min){
  _buffer = buffer;
  _price_down_time = price_down_time;
  _price_down_amount = price_down_amount;
  _price_min = price_min;
}

void PriceHandler::begin(int price, unsigned long time) {
  _price = price;
  reset(time);
}

bool PriceHandler::step(unsigned long time) {
  if(_price > _price_min){
    if (time >= _next_price_down) {
      _price -= _price_down_amount;
      if (_price < _price_min)
        _price = _price_min;
      else
        _next_price_down = time + _price_down_time;
      return true;
    }
  }
  return false;
}

void PriceHandler::reset(unsigned long time) {
  _next_price_down = time + _price_down_time;
}

void PriceHandler::refresh_price() {
  format_price(_price, _buffer);
}

void PriceHandler::boost_price(byte factor, int price_max){
  _price = _price * factor;
  if(_price > price_max)
    _price = price_max;
}

int PriceHandler::price(){
  return _price;
}

void PriceHandler::format_price(int price, char *buffer) {
  int dollars = price / 100;
  char cents = price - (dollars * 100);
  if (dollars > 10)
    sprintf(buffer, "$%2d.%02d", dollars, cents);
  else if (dollars > 0)
    sprintf(buffer, "$%1d.%02d", dollars, cents);
  else
    sprintf(buffer, "$ .%02d", cents);
}
#endif
