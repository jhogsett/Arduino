

#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>
#include <trend_detector.h>

#define SENSOR_PIN A2

#define RANDOM_SEED_PIN A0
static RandomSeed<RANDOM_SEED_PIN> randomizer;

HT16K33Disp disp1(0x70, 1);
// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

#include <AHT20.h>

AHT20 aht20;

#define FAST_WINDOW 200
#define SLOW_WINDOW 2000
#define TREND_WINDOW 200
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define FIRST_LED 9
#define LAST_LED 11
#define RED_LED 9
#define GREEN_LED 10
#define BLUE_LED 11

TrendDetector *trend_detector_main;
TrendDetector *trend_detector_high;
TrendDetector *trend_detector_low;

#define REPORT_RATE 1000
int report = REPORT_RATE;

void setup() {  
  Serial.begin(115200);
  
  pinMode(SENSOR_PIN, INPUT);
  for(int i = FIRST_LED; i <= LAST_LED; i++)
    pinMode(i, OUTPUT);

  randomizer.randomize();

  byte brightness1[1] = { 4 };

  Wire.begin();

  disp1.init(brightness1);
  // disp2.init(brightness2);
  // disp3.init(brightness3);
  
  aht20.begin();

  float temp = sample_temp();
  trend_detector_main = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);
  trend_detector_high = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);
  trend_detector_low = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);

}

bool running1, running2, running3 = false;

#define SAMPLES 20
#define SAMPLE_DEL 5
bool sampled;

float sample_temp(){
  float temp_c, humid, temp_f;
  temp_c = aht20.getTemperature();
  humid = aht20.getHumidity();

  temp_f = temp_c * (9.0 / 5.0) + 32.0;
  return temp_f;
}

void loop() {

  float temp = sample_temp();

  // trend_detector_main->sample(temp);

  // float mean = trend_detector_main->slow_mean();

  // if(temp > mean){
  //   trend_detector_high->sample(temp);
  // } else if(temp < mean) {
  //   trend_detector_low->sample(temp);
  // }

  // float range = trend_detector_high->slow_mean() - trend_detector_low->slow_mean();
  // float spread = range / 256.0;
  // float base = trend_detector_low->slow_mean();
  // float val = (trend_detector_main->fast_mean() - base) * spread;
  // float ival = val * 256.0;
  // int i = int(ival);
  
  // if(i < 0){
  //   i == 0;
  // } else if(i > 255){
  //   i == 255;
  // }

  // analogWrite(RED_LED, i);

  unsigned long time = millis();

  // // float temp = sample_temp();
  char buffer[11];
  
  // // temp = sample_temp();

  float mean = temp;

  int itemp = int(mean * 10.0);
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
