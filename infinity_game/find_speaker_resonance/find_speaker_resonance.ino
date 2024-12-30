#define SPIN 2
#define DELAY 300

#define START 200
#define END 1000
#define TIMES 100
#define TDELAY 100


void setup() {
  Serial.begin(115200);
  pinMode(SPIN, OUTPUT);

}

void loop() {
  for(int i = START; i <= END; i++){
    Serial.println(i);
    
    for(int j = 0; j < TIMES; j++){
      digitalWrite(SPIN, HIGH);
      delayMicroseconds(i);
      digitalWrite(SPIN, LOW);
      delayMicroseconds(i);
    }

    for(int j = 0; j < TIMES; j++){
      digitalWrite(SPIN, LOW);
      delayMicroseconds(i);
      digitalWrite(SPIN, LOW);
      delayMicroseconds(i);
    }
    

  }
}
