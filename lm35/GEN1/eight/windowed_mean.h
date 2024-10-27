#ifndef WINDOWED_MEAN_H
#define WINDOWED_MEAN_H
#include "Arduino.h"

class WindowedMean
{
public:
  void WindowedMean(long window_length, long primed_value);
  float sample(long sample);
  float mean(void);

private:
  long window_length;
  float mean_accum;
  float last_mean;

};

// long WindowedMean::window_length;
// long WindowedMean::mean_accum;
// long WindowedMean::last_mean;

void WindowedMean::WindowedMean(long window_length, float primed_value){
  this->window_length = window_length;
  this->mean_accum = primed_value * _window_length;
  this->last_mean = primed_value;
}

float WindowedMean::sample(float sample){
  this->mean_accum -= this->last_mean;
  this->mean_accum += sample;
  this->last_mean = this->mean_accum / this->window_length;
  return this->last_mean;
}

float WindowedMean::mean(void){
  return this->last_mean;
}
#endif
