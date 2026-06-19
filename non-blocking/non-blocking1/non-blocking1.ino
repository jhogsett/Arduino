void setup() {
  Serial.begin(115200);
  Serial.println();

}

int loop_cycles = 0;
int loop_cycle= -1;

void start_loop(int cycles){
  loop_cycles = cycles;
  loop_cycle = 0;
}

bool loop_active(){
  return loop_cycle <= loop_cycles;
}

bool step_loop(){
  loop_cycle += 1;
  return loop_active();
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

int state = STATE_IDLE;

void loop() {
  switch(state){
    case STATE_IDLE:
      Serial.println("begin");
      start_loop(5);
      state = STATE_START_LOOP;
      break;

    case STATE_START_LOOP:
      Serial.println("  start loop");
      state = STATE_START_DELAY;
      break;

    case STATE_START_DELAY:
      Serial.println("    start delay");
      start_timer(2000);
      state = STATE_IN_DELAY;
      break;

    case STATE_IN_DELAY:
      // Serial.println("in delay");
      state = (timer_elapsed() ? START_DELAY_END : STATE_IN_DELAY);
      break;

    case START_DELAY_END:
      Serial.println("    delay end");
      state = STATE_CYCLE_LOOP;
      break;

    case STATE_CYCLE_LOOP:
      Serial.println("  cycle loop");
      state = (step_loop() ? STATE_START_LOOP : STATE_END_LOOP);
      break;

    case STATE_END_LOOP:
      Serial.println("end loop");
      state = STATE_DONE;
      break;

    case STATE_DONE:    
      break;
  }
}
