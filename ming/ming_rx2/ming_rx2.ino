
#include <Arduino.h>

#include <Wire.h>
#include <HT16K33Disp.h>

#define RX_PIN 6
#define LED_PIN 13
#define MIN_LEGIT_TIME 50000
#define RESET_AVG_SAMPLES 25

HT16K33Disp *disp1, *disp2, *disp3;
byte device_display_count = 0;

#define FIRST_DISPLAY 0x70
#define LAST_DISPLAY  0x77
#define MAX_DISPLAYS  8

int count_displays(){
  int num_devices = 0;
  for (byte address = FIRST_DISPLAY; address <= LAST_DISPLAY; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
      num_devices++;
    }
  return num_devices;
}


void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(RX_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  device_display_count = count_displays();
  switch(device_display_count){
    case 1:
    {
      byte brightness[1] = { 9 }; // Amber
      disp1 = new HT16K33Disp(0x70, 1);
      disp1->init(brightness);
      break;
    }
    case 2:
    {
      byte brightness[2] = { 15, 15 }; // Red
      disp1 = new HT16K33Disp(0x70, 2);
      disp2 = new HT16K33Disp(0x71, 1);
      disp1->init(brightness);
      break;
    }
    case 3:
    {
      byte brightness[3] = { 1, 1, 1 }; // Green
      disp1 = new HT16K33Disp(0x70, 3);
      disp2 = new HT16K33Disp(0x71, 1);
      disp3 = new HT16K33Disp(0x72, 1);
      disp1->init(brightness);
      break;
    }
  }
}

void loop() {
  bool state, last_state = false;
  unsigned long time, maxtime = 0L;
  unsigned long mintime = (unsigned long)-1L;
  int sample_count = 0;
  char buffer[20] = {"\0"};
  bool running1 = false;

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

          int rdiff = diff / 1000L;
          int rmintime = mintime / 1000L;
          int rmaxtime = maxtime / 1000L;

          sprintf(buffer, "%4d%4d%4d", rdiff, rmintime, rmaxtime);

          Serial.print("Duration, Min, Max: ");
          Serial.print(rdiff);
          Serial.print(", ");
          Serial.print(rmintime);
          Serial.print(", ");
          Serial.print(rmaxtime);
          Serial.println();
        }
      } else {
        digitalWrite(LED_PIN, HIGH);
        time = micros();
      }

      last_state = state;
    }

    unsigned long dtime = millis();
    if(!running1)
      disp1->begin_scroll_string(buffer, 100, 100);

    running1 = disp1->step_scroll_string(dtime);  
  }
}
