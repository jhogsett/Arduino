void setup() {
  Serial.begin(115200);
  Serial.println();

}

int _loop_cycles = 0;
int _loop_cycle = -1;

void start_loop(int cycles){
  _loop_cycles = cycles;
  _loop_cycle = 0;
}

bool loop_active(){
  return _loop_cycle < _loop_cycles;
}

bool step_loop(){
  _loop_cycle += 1;
  return loop_active();
}

int loop_cycle(){
  return _loop_cycle;
}

unsigned long start_time = 0;
unsigned long duration = 0;

void start_timer(unsigned long _duration){
  start_time = millis();
  duration = _duration;
}

bool timer_elapsed(){
  return (millis() - start_time) >= duration;
}

#define STATE_IDLE 0
#define STATE_START_LOOP 1
#define STATE_START_DELAY 2
#define STATE_IN_DELAY 3
#define START_DELAY_END 4
#define STATE_CYCLE_LOOP 5
#define STATE_END_LOOP 6
#define STATE_DONE 7
#define STATE_ABORT 8

int state = STATE_IDLE;

void loop() {
  if(Serial.available()){
    state = STATE_ABORT;
  }

  switch(state){
    case STATE_IDLE:
      Serial.println("begin loop");
      start_loop(5);
      state = STATE_START_LOOP;
      break;

    case STATE_START_LOOP:
      Serial.print("  begin loop cycle ");
      Serial.println(loop_cycle());
      state = STATE_START_DELAY;
      break;

    case STATE_START_DELAY:
      Serial.println("    begin delay");
      start_timer(500);
      state = STATE_IN_DELAY;
      break;

    case STATE_IN_DELAY:
      // Serial.println("in delay");
      state = (timer_elapsed() ? START_DELAY_END : STATE_IN_DELAY);
      break;

    case START_DELAY_END:
      Serial.println("    end delay");
      state = STATE_CYCLE_LOOP;
      break;

    case STATE_CYCLE_LOOP:
      Serial.println("  end loop cycle");
      state = (step_loop() ? STATE_START_LOOP : STATE_END_LOOP);
      break;

    case STATE_END_LOOP:
      Serial.println("end loop");
      state = STATE_DONE;
      break;

    case STATE_ABORT:
      Serial.println("user aborted");
      while(Serial.available() && Serial.read());
      state = STATE_DONE;
      break;

    case STATE_DONE:    
      Serial.println("\r\n");
      delay(1000);
      state = STATE_IDLE;
      break;

  }

}
