#include "types.h"

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

NonBlockingSequence sequence;

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
  sequence.start(actions, times, 4, 5, NULL, abort_sequence);
}

void loop() {
  if(!sequence.step()){
    if(sequence.aborted()){
      Serial.println("sequence aborted");
    } else {
      Serial.println("sequence ended");
    }
    sequence.restart();
    delay(2000);
  }
}
