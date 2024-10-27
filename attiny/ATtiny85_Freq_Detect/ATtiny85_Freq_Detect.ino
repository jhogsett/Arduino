unsigned long Start_Time = 0;
unsigned long End_Time = 0;

void setup()
{
  pinMode(0, OUTPUT);
}
void loop()
{
  Start_Time = micros();
  delayMicroseconds(5);
  End_Time = micros();


  if ((End_Time - Start_Time) >= 50) {
    // Slow blink for 1-MHz
    digitalWrite(0, HIGH);
    delay(1000);
    digitalWrite(0, LOW);
    delay(1000);
  }
  else {
    // Fast blink for 8-MHz
    digitalWrite(0, HIGH);
    delay(250);
    digitalWrite(0, LOW);
    delay(250);
    digitalWrite(0, HIGH);
    delay(250);
    digitalWrite(0, LOW);
    delay(250);
  }
}
