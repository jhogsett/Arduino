

constexpr int SENSOR_PIN = 3;

constexpr int BUZZER_PIN = 2;
constexpr int BUZZ_TIMES_OPENED = 3;
constexpr int BUZZ_WAIT_OPENED = 12500;
constexpr int BUZZ_TIMES_CLOSED = 7;
constexpr int BUZZ_WAIT_CLOSED = 1500;
constexpr int BUZZ_INT = 100;

bool last_state = false;

void buzz_opened(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZ_INT);
  digitalWrite(BUZZER_PIN, LOW);
  // delay(BUZZ_INT);

  delay(BUZZ_WAIT_OPENED);

  for(int i = 0; i < BUZZ_TIMES_OPENED; i++){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZ_INT);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZ_INT);
  }
}

void buzz_closed(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZ_INT);
  digitalWrite(BUZZER_PIN, LOW);
  // delay(BUZZ_INT);

  delay(BUZZ_WAIT_CLOSED);

  for(int i = 0; i < BUZZ_TIMES_CLOSED; i++){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZ_INT);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZ_INT);
  }
}

void setup() {
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);

  last_state = digitalRead(SENSOR_PIN);
}

void loop() {
  bool current_state = digitalRead(SENSOR_PIN);

  if(current_state != last_state){
    last_state = current_state;
    switch(current_state){
      case LOW:
        buzz_closed();
        break;
      case HIGH:
        buzz_opened();
        break;
    }

    delay(1000);
  }




}
