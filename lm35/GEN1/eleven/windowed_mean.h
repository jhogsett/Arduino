#ifndef WINDOWED_MEAN_H
#define WINDOWED_MEAN_H
#include "Arduino.h"

class WindowedMean
{
public:
  WindowedMean(long window_length, float primed_value);
  float sample(float sample);
  float mean(void);

private:
  long window_length;
  float mean_accum;
  float last_mean;

};

// long WindowedMean::window_length;
// long WindowedMean::mean_accum;
// long WindowedMean::last_mean;

WindowedMean::WindowedMean(long window_length, float primed_value){
  window_length = window_length;
  mean_accum = primed_value * window_length;
  last_mean = primed_value;
}

float WindowedMean::sample(float sample){
  mean_accum -= last_mean;
  mean_accum += sample;
  last_mean = mean_accum / window_length;
  return last_mean;
}

float WindowedMean::mean(void){
  return last_mean;
}
#endif
