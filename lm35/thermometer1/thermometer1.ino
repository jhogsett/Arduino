#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>

#define SENSOR_PIN A1

#define RANDOM_SEED_PIN A0
static RandomSeed<RANDOM_SEED_PIN> randomizer;

HT16K33Disp disp1(0x70, 1);
// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

void setup() {  
  pinMode(SENSOR_PIN, INPUT);

  randomizer.randomize();

  byte brightness1[1] = { 15 };

  Wire.begin();

  disp1.init(brightness1);
  // disp2.init(brightness2);
  // disp3.init(brightness3);
}

bool running1, running2, running3 = false;

float sample_temp(){
  int temp_adc_val;
  float temp_val, temp_c, temp_f;

  temp_adc_val = analogRead(SENSOR_PIN);

  temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  temp_c = (temp_val / 10.0);	/* LM35 gives output of 10mv/°C */
  temp_f = temp_c * (9.0 / 5.0) + 32.0;

  // temp_f = (temp_val / 10.0);	

  return temp_f;
}

void loop() {
  unsigned long time = millis();

  float temp = sample_temp();
  char buffer[11];
  
  int itemp = int(temp * 10.0);
  int templ = itemp / 10;
  int temps = itemp % 10;
  
  sprintf(buffer, "%3d.%1d", templ, temps);

    if(!running1)
      disp1.begin_scroll_string(buffer, 100, 100);

    // if(!running2)
    //   disp2.begin_scroll_string("0x71 WORKS    ", 100, 100);

    // if(!running3)
    //   disp3.begin_scroll_string("0x72 WORKS    ", 100, 100);

    // running2 = disp2.step_scroll_string(time);
    // running3 = disp3.step_scroll_string(time);
    

    // disp1.show_string(buffer);
    
    running1 = disp1.step_scroll_string(time);

}
