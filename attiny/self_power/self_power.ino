long shutdownTimer = 0;

#define LED_PIN 0
#define SENSE_PIN 1
#define POWER_PIN 2

#define POWER_ON_TIME 5000

void setup() {
  // hold the power mosfet ON
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);

  // initialize pin 13 as output 
  // to blink the on-board LED
  pinMode(LED_PIN, OUTPUT);

  // initialize the button status pin
  pinMode(SENSE_PIN, INPUT_PULLUP);
}

void loop() {
  // wait for the shut-down timer
  while (millis() - shutdownTimer < POWER_ON_TIME) {
    // if the button is pressed 
    // (pin 10 is pulled to ground)
    // then extend shutdownTimer
    // and blink the on-board LED
    if (!digitalRead(SENSE_PIN)) {
      shutdownTimer = millis();
      blinkLed();
    }
  }
  // when the time is up then shut it down
  digitalWrite(POWER_PIN, LOW);
  // give it time to shut down
  delay(1000);
}

void blinkLed() {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
}
