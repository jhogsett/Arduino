#define SAMPLE_PIN A0

#include <windowed_mean.h>

void setup() {
  Serial.begin(115200);

}

int sample(){
  int sample1 = analogRead(SAMPLE_PIN);
  int sample2 = analogRead(SAMPLE_PIN);
  return abs(sample2 - sample1);
}

void loop() {
  WindowedMean mean(100, sample());

  while(true){
    mean.sample(sample());
    Serial.println(mean.mean());
  }
}
