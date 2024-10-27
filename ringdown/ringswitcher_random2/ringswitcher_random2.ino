#include <random_seed.h>
#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

#define RINGPIN 3
#define LEDPIN 13
#define US_ON 2000
#define US_OFF 4000
#define UK_ON 400
#define UK_GAP 200
#define UK_OFF 2000
#define RAND_PERIOD 21600
#define SAMPLE_TIME 1000
#define US_ON_PART_MIN 500
#define US_ON_PART_MAX 2000
#define MIN_TIMES 2
#define MAX_TIMES 8
#define BAUD 9600
#define UK_RING_PROB 100

void ring_us(int times, int last_dur=0){
  for(int i = 0; i < times; i++){

    if(last_dur == 0 or i < times-1) {
      digitalWrite(RINGPIN, HIGH);
      delay(US_ON);
      digitalWrite(RINGPIN, LOW);
    } else {
      digitalWrite(RINGPIN, HIGH);
      delay(last_dur);
      digitalWrite(RINGPIN, LOW);
    }

    if(i < times-1)
      delay(4000);
  }
}

void ring_uk(int times){
  for(int i = 0; i < times; i++){
    digitalWrite(RINGPIN, HIGH);
    delay(UK_ON);
    digitalWrite(RINGPIN, LOW);

    delay(UK_GAP);

    digitalWrite(RINGPIN, HIGH);
    delay(UK_ON);
    digitalWrite(RINGPIN, LOW);

    if(i < times-1)
      delay(2000);
  }
}

void ring(int type, int times, int last_dur=0){
  if(type == 1){
    return ring_us(times, last_dur);
  } else {
    return ring_uk(times);
  }
}



void loop() {
  bool show_led = false;
  int i = random(RAND_PERIOD);
  // Serial.println(i);
  if(i == 0){
    // int type = random(UK_RING_PROB + 1);
    // if(type == 0){
    //   type = 2;
    // } else {
    //   type = 1;
    // }
    int type = 1;
    int times = random(MIN_TIMES, MAX_TIMES + 1);
    int last_dur = random(US_ON_PART_MIN, US_ON_PART_MAX + 1);
    Serial.print(times);
    Serial.print(", ");
    Serial.print(type);
    Serial.print(", ");
    Serial.println(last_dur);
    ring(type, times, last_dur);
    // show_led = !show_led;
    // digitalWrite(LEDPIN, show_led ? HIGH : LOW);
  } else {
    show_led = !show_led;
    digitalWrite(LEDPIN, show_led ? HIGH : LOW);
    delay(SAMPLE_TIME / 2);
    show_led = !show_led;
    digitalWrite(LEDPIN, show_led ? HIGH : LOW);
    delay(SAMPLE_TIME / 2);
  }
}



void setup() {
  Serial.begin(BAUD);

  randomizer.randomize();

  // put your setup code here, to run once:
  pinMode(RINGPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(RINGPIN, LOW);

 ring(2, 2);
}

