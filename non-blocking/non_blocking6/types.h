
using NonBlockingAction = void (*)(uint32_t data);
using NonBlockingAbort = bool (*)(uint32_t data);

class NonBlockingTimer
{
public:
  void start(uint32_t duration)
  {
    _start_time = millis();
    _duration = duration;
  }

  bool elapsed()
  {
    return (millis() - _start_time) >= _duration;
  }

private:
  uint32_t _start_time;
  uint32_t _duration;
};

class NonBlockingLoop
{
public:
  void start(uint32_t cycles)
  {
    _cycles = cycles;
    _cycle = 0;
  }

  bool active()
  {
    return _cycle < _cycles;
  }

  bool cycle()
  {
    _cycle += 1;
    return active();
  }

  uint32_t cycle() const { return _cycle; }

private:
  uint32_t _cycles;
  uint32_t _cycle;
};

class NonBlockingSequence
{
public:
  NonBlockingSequence(NonBlockingAction* actions, int* times, int num_actions, NonBlockingAbort abort_action){
    _actions = actions;
    _times = times;
    _num_actions = num_actions;
    _abort_action = abort_action;
  }

  void start(int num_cycles, uint32_t data = NULL){
    _num_cycles = num_cycles;
    _data = data;
    restart();
  }

  void restart(){
    _action_iter = 0;
    _state = STATE_START_LOOP;
    _running = false;
    _aborted = false;
  }

  bool aborted(){
    return _aborted;
  }

  bool step(){
    // if(_abort_action && _abort_action(NULL)){
    //   _state = STATE_ABORTED;
    // }

    if(_abort_action){
      if(_abort_action(NULL)){
        _state = STATE_ABORTED;
      }
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

  enum State : byte {
    STATE_START_LOOP,
    STATE_START_CYCLE,
    STATE_START_ACTION,
    STATE_RUNNING,
    STATE_NEXT_ACTION,
    STATE_NEXT_CYCLE,
    STATE_END_LOOP,
    STATE_ABORTED
  };

private:
  int _num_actions;
  int _num_cycles;
  uint32_t _data;
  int _action_iter;
  State _state;
  NonBlockingAction* _actions;
  int* _times;
  NonBlockingAbort _abort_action;
  NonBlockingLoop _loop;
  NonBlockingAction _action;
  int _time;
  NonBlockingTimer _timer;
  bool _running;
  bool _aborted;
};



