
#include <Arduino.h>

#define RX_PIN 6
#define LED_PIN 13
#define MIN_LEGIT_TIME 100000
#define RESET_AVG_SAMPLES 25

void setup() {
  Serial.begin(115200);
  pinMode(RX_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  bool state, last_state = false;
  unsigned long time, maxtime = 0L;
  unsigned long mintime = (unsigned long)-1L;
  int sample_count = 0;

  while(true){
    state = digitalRead(RX_PIN);

    if(state != last_state){
      if(state == false){
        digitalWrite(LED_PIN, LOW);
        unsigned long diff = micros() - time;

        if(diff >= MIN_LEGIT_TIME){
          if(++sample_count > RESET_AVG_SAMPLES){
            sample_count = 1;
            Serial.println("Resetting Bounds");
            maxtime = 0L;
            mintime = (unsigned long)-1L;
          }

          if(diff < mintime)
            mintime = diff;
          if(diff > maxtime)
            maxtime = diff;
          Serial.print("Duration, Min, Max: ");
          Serial.print(diff / 1000L);
          Serial.print(", ");
          Serial.print(mintime / 1000L);
          Serial.print(", ");
          Serial.print(maxtime / 1000L);
          Serial.println();
        }
      } else {
        digitalWrite(LED_PIN, HIGH);
        time = micros();
      }

      last_state = state;
    }
  }

}
