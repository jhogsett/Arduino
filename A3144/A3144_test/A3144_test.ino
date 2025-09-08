void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
}

void loop() {
  digitalWrite(3, digitalRead(2) ? LOW : HIGH);
}
