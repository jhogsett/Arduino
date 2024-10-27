#include <random_seed.h>
#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

#define R0_ON  12
#define R0_OFF 11
#define R1_ON  10 
#define R1_OFF  9
#define R2_ON   8
#define R2_OFF  7
#define R3_ON   6
#define R3_OFF  5
#define R4_ON   4
#define R4_OFF  3
#define RP_OFF  2
#define FIRST_PAIR 3
#define LAST_PAIR 11
#define PULSE_WIDTH 8


// the setup function runs once when you press reset or power the board
void setup() {
  randomizer.randomize();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(R0_ON, OUTPUT);
  pinMode(R0_OFF, OUTPUT);
  pinMode(R1_ON, OUTPUT);
  pinMode(R1_OFF, OUTPUT);
  pinMode(R2_ON, OUTPUT);
  pinMode(R2_OFF, OUTPUT);
  pinMode(R3_ON, OUTPUT);
  pinMode(R3_OFF, OUTPUT);
  pinMode(R4_ON, OUTPUT);
  pinMode(R4_OFF, OUTPUT);
  pinMode(RP_OFF, OUTPUT);
}


void pulse(int pin){
  digitalWrite(pin, HIGH);
  delay(PULSE_WIDTH);
  digitalWrite(pin, LOW);

}

void pulse_all(int offset){
  for(int i=FIRST_PAIR; i <= LAST_PAIR; i += 2){
    digitalWrite(i + offset, HIGH);
  }
  delay(PULSE_WIDTH);
  for(int i=FIRST_PAIR; i <= LAST_PAIR; i += 2){
    digitalWrite(i + offset, LOW);
  }
}

void pulse_some(int offset, int count){
  int _count = 0;
  for(int i=FIRST_PAIR; i <= LAST_PAIR; i += 2){
    if(++_count <= count){
      digitalWrite(i + offset, HIGH);
    } else {
      // dummy for timing
      digitalWrite(i + offset, LOW);
    }
  }
  delay(PULSE_WIDTH);
  for(int i=FIRST_PAIR; i <= LAST_PAIR; i += 2){
    digitalWrite(i + offset, LOW);
  }
}

void loop() {
  // delay(353);
  // pulse_all(0);
  // delay(313);
  // pulse_all(1);

  // delay(353);
  // pulse_some(0, 5);
  // delay(313);
  // pulse_some(1, 5);
  for(int c = 1; c <= 5; c++){
    for(int i = 0; i < 4; i++){
      delay(290);
      pulse_some(0, c);
      delay(310);
      pulse_some(1, c);
    }
    // delay(500);
  }
  digitalWrite(RP_OFF, HIGH);
  while(true);
}















