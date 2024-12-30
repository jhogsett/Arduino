#define SPIN 13


#define INV_FREQ 339

// #define INV_FREQ 337
// #define INV_FREQ 338
// #define INV_FREQ 340
// #define INV_FREQ 341
// #define INV_FREQ 342
// #define INV_FREQ 350
// #define INV_FREQ 380
// #define INV_FREQ 400

#define PULSES 100

#define BEEP_TIMES 4
#define BEEP_DELAY 1250

void setup() {
  Serial.begin(115200);
  pinMode(SPIN, OUTPUT);

}

void beep(int inv_freq, int times){
  for(int j = 0; j < times; j++){
    digitalWrite(SPIN, HIGH);
    delayMicroseconds(inv_freq);
    digitalWrite(SPIN, LOW);
    delayMicroseconds(inv_freq);
  }
}

void beep_gap(int inv_freq, int times){
  for(int j = 0; j < times; j++){
    digitalWrite(SPIN, LOW);
    delayMicroseconds(inv_freq);
    digitalWrite(SPIN, LOW);
    delayMicroseconds(inv_freq);
  }
}

void loop() {
  for(int i = 0; i < BEEP_TIMES; i++){
    beep(INV_FREQ, PULSES);    
    beep_gap(INV_FREQ, PULSES);
  }
  delay(BEEP_DELAY);    
}
