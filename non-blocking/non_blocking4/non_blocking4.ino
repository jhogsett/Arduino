#include "types.h"


// unsigned long start_time = 0;
// unsigned long duration = 0;

// void start_timer(unsigned long _duration){
//   start_time = millis();
//   duration = _duration;
// }

// bool timer_elapsed(){
//   return (millis() - start_time) >= duration;
// }

// using NonBlockingAction = void (*)(void * p);
void action_1(uint32_t data){
  Serial.println("action 1");
}

void action_2(uint32_t data){
  Serial.println("action 2");
}

void action_3(uint32_t data){
  Serial.println("action 3");
}

void action_4(uint32_t data){
  Serial.println("action 4");
}

NonBlockingAction actions[4] = { action_1, action_2, action_3, action_4};
int times[4] = { 1000, 200, 500, 100 };

int _num_actions;
int _num_cycles;
uint32_t _data;
int _action_iter;
int _state;
NonBlockingAction* _actions;
int* _times;
NonBlockingAbort _abort_action;
NonBlockingLoop _loop;
NonBlockingAction _action;
int _time;
NonBlockingTimer _timer;
bool _running;
bool _aborted;

#define STATE_START_LOOP 0
#define STATE_START_CYCLE 1
#define STATE_START_ACTION 2
#define STATE_RUNNING 3
#define STATE_NEXT_ACTION 4
#define STATE_NEXT_CYCLE 5
#define STATE_END_LOOP 6
#define STATE_ABORTED 7


void restart_sequence(){
  _action_iter = 0;
  _state = STATE_START_LOOP;
  _running = false;
  _aborted = false;
}

void start_sequence(NonBlockingAction* actions, int* times, int num_actions, int num_cycles, uint32_t data, NonBlockingAbort abort_action = NULL){
  _actions = actions;
  _times = times;
  _num_actions = num_actions;
  _num_cycles = num_cycles;
  _data = data;
  _abort_action = abort_action;
  // _action_iter = 0;
  // _state = STATE_START_LOOP;
  // _running = false;
  // _aborted = false;
  restart_sequence();
}


bool sequence_aborted(){
  return _aborted;
}

bool step_sequence(){
  if(_abort_action && _abort_action(NULL)){
    _state = STATE_ABORTED;
  }

  switch(_state){
    case STATE_START_LOOP:
      _loop.start(_num_cycles);
      _running = true;
      _state = STATE_START_CYCLE;
      break;

    case STATE_START_CYCLE:
      _action_iter = 0;
      _state = STATE_START_ACTION;
      break;

    case STATE_START_ACTION:
      _action = _actions[_action_iter];
      _time = _times[_action_iter];
      _timer.start(_time);
      if(_action != NULL){
        _action(_data);
      }
      _state = STATE_RUNNING;
      break;      

    case STATE_RUNNING:
      if(_timer.elapsed()){
        _state = STATE_NEXT_ACTION;
      }
      break;

    case STATE_NEXT_ACTION:
      _action_iter += 1;
      _state = (_action_iter >= _num_actions) ? STATE_NEXT_CYCLE : STATE_START_ACTION;
      break;

    case STATE_NEXT_CYCLE:
      _state = _loop.cycle() ? STATE_START_CYCLE : STATE_END_LOOP;
      break;

    case STATE_END_LOOP:
      _running = false;
      break;

    case STATE_ABORTED:
      _aborted = true;
      _running = false;
      break;
  }

  return _running;
}

bool abort_sequence(uint32_t data){
  if(Serial.available()){
    while(Serial.available() && Serial.read());
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  start_sequence(actions, times, 4, 5, NULL, abort_sequence);
}

void loop() {
  if(!step_sequence()){
    if(sequence_aborted()){
      Serial.println("sequence aborted");
    } else {
      Serial.println("sequence ended");
    }
    restart_sequence();
    delay(2000);
  }
}
