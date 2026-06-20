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

NonBlockingAction actions[4] = { action_1, NULL, action_2, NULL};
int times[4] = { 1000, 200, 500, 100 };

int _num_actions;
int _num_cycles;
uint32_t _data;
int _action_iter;
int _state;
NonBlockingAction* _actions;
int* _times;
NonBlockingLoop _loop;

#define STATE_START_LOOP 0
#define STATE_START_CYCLE 1
#define STATE_START_ACTION 2
#define STATE_RUNNING 3
#define STATE_NEXT_ACTION 4
#define STATE_NEXT_CYCLE 5
#define STATE_END_LOOP 6

void start_sequence(NonBlockingAction* actions, int* times, int num_actions, int num_cycles, uint32_t data){
  _actions = actions;
  _times = times;
  _num_actions = num_actions;
  _num_cycles = num_cycles;
  _data = data;
  _action_iter = 0;
}

// need abort functionality

NonBlockingAction action;
int time;
NonBlockingTimer timer;

void step_sequence(){
  switch(_state){
    case STATE_START_LOOP:
      Serial.print("num cycles: ");
      Serial.println(_num_cycles);
      _loop.start(_num_cycles);
      _state = STATE_START_CYCLE;
      break;

    case STATE_START_CYCLE:
      _action_iter = 0;
      _state = STATE_START_ACTION;
      break;

    case STATE_START_ACTION:
      Serial.println("start action");
      action = _actions[_action_iter];
      time = _times[0];
      Serial.print("time: ");
      Serial.println(time);
      timer.start(time);
      if(action != NULL){
        action(_data);
      }
      _state = STATE_RUNNING;
      break;      

    case STATE_RUNNING:
      if(timer.elapsed()){
        Serial.println("here");
        _state = STATE_NEXT_ACTION;
      }
      break;

    case STATE_NEXT_ACTION:
      Serial.println("next action");
      _action_iter += 1;
      _state = (_action_iter >= _num_actions) ? STATE_NEXT_CYCLE : STATE_START_ACTION;
      break;

    case STATE_NEXT_CYCLE:
      _state = _loop.cycle() ? STATE_START_CYCLE : STATE_END_LOOP;
      break;

    case STATE_END_LOOP:
      Serial.println("END");
      break;
  }
}



// loop over the arrays in an outer loop


// #define STATE_IDLE 0
// #define STATE_START_LOOP 1
// #define STATE_START_DELAY 2
// #define STATE_IN_DELAY 3
// #define START_DELAY_END 4
// #define STATE_CYCLE_LOOP 5
// #define STATE_END_LOOP 6
// #define STATE_DONE 7
// #define STATE_ABORT 8

// int state = STATE_IDLE;

// NonBlockingLoop loop1;
// NonBlockingTimer timer1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  start_sequence(actions, times, 4, 5, NULL);

}

void loop() {
  step_sequence();

//   if(Serial.available()){
//     state = STATE_ABORT;
//   }

//   switch(state){
//     case STATE_IDLE:
//       Serial.println("begin loop");
//       loop1.start(5);
//       state = STATE_START_LOOP;
//       break;

//     case STATE_START_LOOP:
//       Serial.print("  begin loop cycle ");
//       Serial.println(loop1.cycle());
//       state = STATE_START_DELAY;
//       break;

//     case STATE_START_DELAY:
//       Serial.println("    begin delay");
//       timer1.start(500);
//       state = STATE_IN_DELAY;
//       break;

//     case STATE_IN_DELAY:
//       // Serial.println("in delay");
//       state = (timer1.elapsed() ? START_DELAY_END : STATE_IN_DELAY);
//       break;

//     case START_DELAY_END:
//       Serial.println("    end delay");
//       state = STATE_CYCLE_LOOP;
//       break;

//     case STATE_CYCLE_LOOP:
//       Serial.println("  end loop cycle");
//       state = (loop1.cycle() ? STATE_START_LOOP : STATE_END_LOOP);
//       break;

//     case STATE_END_LOOP:
//       Serial.println("end loop");
//       state = STATE_DONE;
//       break;

//     case STATE_ABORT:
//       Serial.println("user aborted");
//       while(Serial.available() && Serial.read());
//       state = STATE_DONE;
//       break;

//     case STATE_DONE:    
//       Serial.println("\r\n");
//       delay(1000);
//       state = STATE_IDLE;
//       break;

//   }

}
