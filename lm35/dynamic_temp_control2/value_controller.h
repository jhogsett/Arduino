#include <trend_detector.h>

class ValueController{
public:
  ValueController(bool controller_type, 
                  int controller_pin, 
                  int controller_max, 
                  int controller_min, 
                  int controller_speed_steps, 
                  int controller_boost_time, 
                  float control_start, 
                  float control_max, 
                  int fast_window, 
                  int slow_window, 
                  int trend_window, 
                  float trend_sense, 
                  float settled_window, 
                  int trend_boost, 
                  float initial_sample);
  int surging(void);
  void control(float sample);
  void control_up(float sample);
  void control_down(float sample);
  static const bool UP_CONTROLLER=true;
  static const bool DOWN_CONTROLLER=false;
  TrendDetector *_trend_detector;
  int _current_controller_speed_step;
   
private:
  int _current_controller_speed;
  bool _controller_status;
  int _controller_pin;
  bool _controller_type;
  int _controller_max;
  int _controller_min;
  float _control_start;
  float _control_max;
  int _control_boost_time;
  int _controller_speed_steps;
  int _controller_speed_step;
  int _controller_boost_time;
  int _trend_boost;
  float _control_range;

  void set_controller_off(void);
  void set_controller_speed(int speed);
};

// type: false=down, true=up
ValueController::ValueController(bool controller_type, 
                                int controller_pin, 
                                int controller_max, 
                                int controller_min, 
                                int controller_speed_steps, 
                                int controller_boost_time, 
                                float control_start, 
                                float control_max, 
                                int fast_window, 
                                int slow_window, 
                                int trend_window, 
                                float trend_sense, 
                                float settled_window, 
                                int trend_boost, 
                                float initial_sample){
  _controller_type = controller_type;
  _controller_pin = controller_pin;
  _controller_max = controller_max;
  _controller_min = controller_min;
  _controller_speed_steps = controller_speed_steps;
  _controller_boost_time = controller_boost_time;
  _control_start = control_start;
  _control_max = control_max;
  _trend_boost = trend_boost;
  
  if(_controller_type == UP_CONTROLLER)
    _control_range = _control_start - _control_max;
  else
    _control_range = _control_max - _control_start;
  _controller_speed_step = (int((_controller_max - _controller_min) / _controller_speed_steps));
 
 _trend_detector = new TrendDetector(fast_window, slow_window, trend_window, trend_sense, settled_window, initial_sample);
}

void ValueController::set_controller_off(void){
  digitalWrite(_controller_pin, LOW);
  _current_controller_speed = 0;
  _current_controller_speed_step = 0;
}

void ValueController::set_controller_speed(int speed){
  int prev_step = _current_controller_speed_step;

  if(speed != _current_controller_speed_step){

    if(speed <= 0){
      set_controller_off();

    } else {
      if(speed > _controller_speed_steps)
        speed = _controller_speed_steps;
        
      _current_controller_speed_step = speed;
      _current_controller_speed = (speed * _controller_speed_step) + _controller_min;

      // if going up in speed, temporarily set to the maximum
      // TODO make optional
      if(_current_controller_speed_step > prev_step){
        analogWrite(_controller_pin, _controller_max);
        delay(_controller_boost_time);
      }
      
      analogWrite(_controller_pin, _current_controller_speed);
    }
  }
}

void ValueController::control(float sample){
  if(_controller_type == UP_CONTROLLER)
    control_up(sample);
  else
    control_down(sample);
}

// -1 for surging down, 1 for surging up, 
int ValueController::surging(void){
  if(!_trend_detector->settled())
   if(_trend_detector->mean() > 0.0)
    return 1;
  else if(_trend_detector->mean() < 0.0)
    return -1;
  return 0;
}

void ValueController::control_up(float sample){
  _trend_detector->sample(sample);
  bool trend_boost = surging() == -1;
  int under_boost = 0;

  if(_trend_detector->fast_mean() < _control_start){
    float temp = _trend_detector->fast_mean();
    if(temp < _control_max)
      temp = _control_max;

    // map the capped UNDERHEAT amount to 0.0 - 1.0
    float under_value = (_control_start - temp) / _control_range;
    
    // map the UNDERHEAT value to the heat level range
    under_boost = 1 + int(under_value * _controller_speed_steps);
  
  } else {
    under_boost = 0;
  }

  set_controller_speed(int(under_boost + (trend_boost ? _trend_boost : 0)));
}

void ValueController::control_down(float sample){
  _trend_detector->sample(sample);
  bool trend_boost = surging() == 1;
  int over_boost = 0;
  
  if(_trend_detector->fast_mean() > _control_start){
    float temp = _trend_detector->fast_mean();
    if(temp > _control_max)
      temp = _control_max;

    // map the capped overheat amount to 0.0 - 1.0
    float over_value = (temp - _control_start) / _control_range;
    
    // map the overheat value to the controller speed range
    over_boost = 1 + int(over_value * _controller_speed_steps);
  
  } else {
    over_boost = 0;
  }

  set_controller_speed(int(over_boost + (trend_boost ? _trend_boost : 0)));
}

