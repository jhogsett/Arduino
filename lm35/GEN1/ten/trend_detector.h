#ifndef TREND_DETECTOR_H
#define TREND_DETECTOR_H
#include "Arduino.h"
#include "windowed_mean.h"

class TrendDetector
{
public:
  TrendDetector(long fast_window, long slow_window, long trend_window, float trend_sense, float settled_window, float primed_value);
  float sample(float sample);
  float short_mean(void);
  float long_mean(void);
  float trend_mean(void);
  bool settled(void);

private:
  WindowedMean *fast_mean;
  WindowedMean *slow_mean;
  WindowedMean *trend_mean;
  float trend_sense;
  float settled_window;
};

TrendDetector::TrendDetector(long fast_window, long slow_window, long trend_window, float trend_sense, float settled_window, float primed_value){
  this->fast_mean = new WindowedMean(fast_window, primed_value);
  this->slow_mean = new WindowedMean(slow_window, primed_value);
  this->trend_mean = new WindowedMean(trend_windows, 0.0);
  this->trend_sense = trend_sense;
  this->settled_window = settled_window;
}

float TrendDetector::sample(float sample){
  float trend;

  fast_mean->sample(sample);
  slow_mean->sample(sample);

  if(slow_mean->mean() > fast_mean->mean())
    trend = -this->trend_sense * (slow_mean->mean() - fast_mean->mean());
  else if(slow_mean->mean() < fast_mean->mean())
    trend = this->trend_sense * (fast_mean->mean() - slow_mean->mean());

  trend_mean->sample(trend);
  return this->trend_mean->mean();
}

float TrendDetector::fast_mean(void){
  return this->fast_mean->mean();
}

float TrendDetector::slow_mean(void){
  return this->slow_mean->mean();
}

float TrendDetector::mean(void){
  return this->trend_mean->mean();
}

bool TrendDetector::settled(void){
  return this->trend_mean->mean() >= -this->settled_window && this->trend_mean->mean() <= this->settled_window;  
}
#endif
