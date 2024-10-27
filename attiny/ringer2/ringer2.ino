#include <EEPROM.h>
#include <random_seed.h>

#define RANDOM_SEED_PIN A1
static RandomSeed<RANDOM_SEED_PIN> randomizer;

#define RINGPIN 0
#define LEDPIN 1
#define US_ON 2000
#define US_OFF 4000
#define SIG_ON 150
#define SIG_OFF 150

#define US_ON_PART_MIN 500
#define US_ON_PART_MAX 2000
#define MIN_TIMES 2
#define MAX_TIMES 8
#define BAUD 9600
#define SAMPLE_TIME 1000

#define MODE_SLOW   0
#define MODE_MEDIUM 1
#define MODE_FAST   2
#define MODE_DEMO 3
#define MODE_FIRST 0
#define MODE_LAST 3

#define SLOW_PERIOD 21600
#define MEDIUM_PERIOD 3600
#define FAST_PERIOD 600
#define DEMO_PERIOD 60

// #define RAND_PERIOD 900

int mode = MODE_FIRST;
int period = SLOW_PERIOD;

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

void ring_signal(int times){
  for(int i = 0; i < times; i++){
    digitalWrite(RINGPIN, HIGH);
    delay(SIG_ON);

    digitalWrite(RINGPIN, LOW);
    delay(SIG_OFF);
  }
}

bool show_led = false;
void loop() {
  int i = random(period);
  if(i == 0){
    int times = random(MIN_TIMES, MAX_TIMES + 1);
    int last_dur = random(US_ON_PART_MIN, US_ON_PART_MAX + 1);
    ring_us(times, last_dur);
  } else {
    delay(SAMPLE_TIME);
  }
  show_led = !show_led;
  digitalWrite(LEDPIN, show_led ? HIGH : LOW);
}

void setup() {
  randomizer.randomize();

  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  pinMode(RINGPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  digitalWrite(RINGPIN, LOW);

  mode = EEPROM.read(0);
  mode++;

  if(mode < MODE_FIRST || mode > MODE_LAST){
    mode = MODE_FIRST;
  }
  EEPROM.write(0, mode);

  switch(mode){
    case MODE_SLOW:
      period = SLOW_PERIOD;
      ring_signal(1);
      break;
    case MODE_MEDIUM:
      period = MEDIUM_PERIOD;
      ring_signal(2);
      break;
    case MODE_FAST:
      period = FAST_PERIOD;
      ring_signal(3);
      break;
    case MODE_DEMO:
      period = DEMO_PERIOD;
      ring_signal(4);
      break;
  }

  delay(1000);
  ring_us(1);
}
