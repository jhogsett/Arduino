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
#define PULSE_WIDTH 7


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

void pulse_pair(int pair, int offset){
  int pin = (pair * 2) + FIRST_PAIR + offset;
  pulse(pin);
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

#define ACCEL1 0.95
#define ACCEL2 0.96
#define TICK_UP 500
#define TICK_DOWN 375
#define TIMES_PER_SCALE 5
#define ACCEL_TIMES 2

void scene1(){
  int tick_up = TICK_UP;
  int tick_dn = TICK_DOWN;  
  int times = TIMES_PER_SCALE;

  for(int c = 1; c <= 5; c++){
    for(int i = 0; i < times; i++){
      delay(tick_up);
      pulse_some(0, c);
      delay(tick_dn);
      pulse_some(1, c);

      tick_up = int(tick_up * ACCEL2);
      tick_dn = int(tick_dn * ACCEL2);
    }

    tick_up = int(tick_up * ACCEL1);
    tick_dn = int(tick_dn * ACCEL1);
    times += ACCEL_TIMES;
  }
}

// #define SCENE2_TIMES 400
#define SCENE2_DELAY 25000
#define SCENE2_ACCEL 0.96

void scene2(int times){
  int del = SCENE2_DELAY;

  for(int i = 0; i < times; i++){
    int rp = random(FIRST_PAIR, LAST_PAIR+2);
    pulse(rp);
    delayMicroseconds(del);
    del = int(del * SCENE2_ACCEL);
  }  
}

#define SCENE3_MAX 5000
#define SCENE3_MAX_DIFF 50
#define SCENE3_TIMES 500000

void scene3(){
  int maxs[5];
  
  for(int i = 0; i < 5; i++)
    maxs[i] = SCENE3_MAX - (i * SCENE3_MAX_DIFF);

  int currents[5] = { 0, 0, 0, 0, 0};
  int counts[5] = { 0, 0, 0, 0, 0};

  for(long k = 0; k < SCENE3_TIMES; k++){
    // Serial.println(k);
    for(int j = 0; j < 5; j++){
      counts[j]++;
      if(counts[j] >= maxs[j]){
        counts[j] = 0;

        if(currents[j] == 0)
          currents[j] = 1;
        else
          currents[j] = 0;

        pulse_pair(j, currents[j]);
      }
    }
  }
}

#define SCENE4_TIMES 1
#define SCENE4_DEL1 4
#define SCENE4_TIMES1 40
#define SCENE4_DEL2 5
#define SCENE4_TIMES2 15
#define SCENE4_DEL3 2
#define SCENE4_TIMES3 70

void scene4(){
  for(int k = 0; k < SCENE4_TIMES; k++){
    for(int i = 0; i < SCENE4_TIMES1; i++){
      pulse_all(0);
      delay(SCENE4_DEL1);
      pulse_all(1);
      delay(SCENE4_DEL1);
    }
    for(int i = 0; i < SCENE4_TIMES2; i++){
      pulse_all(0);
      delay(SCENE4_DEL2);
      pulse_all(1);
      delay(SCENE4_DEL2);
    }
    for(int i = 0; i < SCENE4_TIMES3; i++){
      pulse_all(0);
      delay(SCENE4_DEL3);
      pulse_all(1);
      delay(SCENE4_DEL3);
    }
    // delay(500);
  }
}

// #define SCENE5_TIMES 4
#define SCENE5_DEL1 7
#define SCENE5_TIMES1 35
#define SCENE5_DELAY 500

void scene5(int times){
  for(int k = 0; k < times; k++){
    for(int i = 0; i < SCENE5_TIMES1; i++){
      pulse_all(0);
      delay(SCENE5_DEL1);
      pulse_all(1);
      delay(SCENE5_DEL1);
    }
    delay(500);
  }
}


#define TICK_UP_S9 52
#define TICK_DOWN_S9 38
#define TIMES_PER_SCALE_S9 15

void scene9(){
  int tick_up = TICK_UP_S9;
  int tick_dn = TICK_DOWN_S9;  
  int times = TIMES_PER_SCALE_S9;

  for(int c = 5; c > 0; c--){
    for(int i = times-1; i >= 0 ; i--){
      delay(tick_up);
      pulse_some(0, c);
      delay(tick_dn);
      pulse_some(1, c);

      tick_up = int(tick_up / ACCEL2);
      tick_dn = int(tick_dn / ACCEL2);
    }

    tick_up = int(tick_up / ACCEL1);
    tick_dn = int(tick_dn / ACCEL1);
    times -= ACCEL_TIMES;
  }
}

#define SCENE10_DELAY 2

void scene10(){
  for(int i = 1; i < 20; i++){
    int del = SCENE10_DELAY * i;
    pulse_pair(4, 0);
    delay(del);
    pulse_pair(4, 1);
    delay(del);
  }
}

void endit(){
  digitalWrite(RP_OFF, HIGH);
  while(true);
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

  // for(int c = 1; c <= 5; c++){
  //   for(int i = 0; i < 4; i++){
  //     delay(TICK_UP);
  //     pulse_some(0, c);
  //     delay(TICK_DOWN);
  //     pulse_some(1, c);
  //   }
  //   // delay(500);
  // }

  scene1();

  delay(1000);
  scene3();

  delay(1000);

  scene5(3);
  scene2(1000);

  delay(100);
  scene4();
  delay(100);

  delay(2000);
  scene9();
  delay(1000);

  scene10();
  endit();
}















