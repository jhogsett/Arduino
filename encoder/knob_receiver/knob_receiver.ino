void setup() {
  Serial.begin(115200);
}

void loop() {
  char buffer[10];
  byte read = 0;
  if((read = Serial.readBytesUntil('\n', buffer, 9)) != 0){
    buffer[read] = '\0';
    Serial.println(buffer);
    int id = buffer[0] - '0';
    int data = buffer[1] - '0';
    Serial.println(id);
    Serial.println(data);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);

    if(id > 0 && id < 4 && data >= 0 and data <= 3){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
