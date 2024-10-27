#define RINGPIN 12
#define US_ON 2000
#define US_OFF 4000
#define UK_ON 400
#define UK_GAP 200
#define UK_OFF 2000
#define RAND_PERIOD 14400
#define SAMPLE_TIME 1000

void setup() {
  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(RINGPIN, OUTPUT);

  ring(2, 1);
}

void ring_us(int times){
  for(int i = 0; i < times; i++){
    digitalWrite(RINGPIN, HIGH);
    delay(US_ON);
    digitalWrite(RINGPIN, LOW);
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



void ring(int type, int times){
  if(type == 1){
    return ring_us(times);
  } else {
    return ring_uk(times);
  }
}


void loop() {
  int i = random(RAND_PERIOD);
  Serial.println(i);
  if(i == 0){
    ring(1, 6);
  } else {
    delay(SAMPLE_TIME);
  }
}
